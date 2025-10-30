#include "server/Client.hpp"
#include "Library/Play/Layout/SimpleLayoutAppearWaitEnd.h"
//#include "al/util/LiveActorUtil.h"
#include "System/SaveDataAccessFunction.h"
#include <heap/seadHeapMgr.h>
#include "packets/Packet.h"

SEAD_SINGLETON_DISPOSER_IMPL(Client)

typedef void (Client::*ClientThreadFunc)(void);

/**
 * @brief Construct a new Client:: Client object
 * 
 */
Client::Client() {

    mHeap = sead::ExpHeap::create(0x50000, "ClientHeap", sead::HeapMgr::instance()->getCurrentHeap(), 8, sead::Heap::cHeapDirection_Forward, false);

    sead::ScopedCurrentHeapSetter heapSetter(
        mHeap);  // every new call after this will use ClientHeap instead of SequenceHeap

    mReadThread = new al::AsyncFunctorThread("ClientReadThread", al::FunctorV0M<Client*, ClientThreadFunc>(this, &Client::readFunc), 0, 0x1000, {0});

    mKeyboard = new Keyboard(nn::swkbd::GetRequiredStringBufferSize());

    mSocket = new SocketClient("SocketClient", mHeap);

    mConnectCount = 0;

    curCollectedShines.fill(-1);

    collectedShineCount = 0;

    mShineArray.allocBuffer(100, nullptr); // max of 100 shine actors in buffer

    nn::account::GetLastOpenedUser(&mUserID);

    nn::account::Nickname playerName;
    nn::account::GetNickname(&playerName, mUserID);
    //Logger::setLogName(playerName.name);  // set Debug logger name to player name

    mUsername = playerName.name;
    
    mUserID.print();

    //Logger::log("Player Name: %s\n", playerName.name);

    //Logger::log("%s Build Number: %s\n", playerName.name, TOSTRING(BUILDVERSTR));

}

/**
 * @brief initializes client class using initInfo obtained from StageScene::init
 * 
 * @param initInfo init info used to create layouts used by client
 */
void Client::init(al::LayoutInitInfo const &initInfo, GameDataHolderAccessor holder) {

    mConnectionWait = new (mHeap) al::WindowConfirmWait("ServerWaitConnect", "WindowConfirmWait", initInfo);
    
    mConnectStatus = new (mHeap) al::SimpleLayoutAppearWaitEnd("", "SaveMessage", initInfo, 0, false);

    mConnectionWait->setTxtMessage(u"Connecting to Server.");
    mConnectionWait->setTxtMessageConfirm(u"Failed to Connect!");

    al::setPaneString(mConnectStatus, "TxtSave", u"Connecting to Server.", 0);
    al::setPaneString(mConnectStatus, "TxtSaveSh", u"Connecting to Server.", 0);

    mHolder = holder;

    startThread();

    //Logger::log("Heap Free Size: %f/%f\n", mHeap->getFreeSize() * 0.001f, mHeap->getSize() * 0.001f);
}
/**
 * @brief starts client read thread
 * 
 * @return true if read thread was sucessfully started
 * @return false if read thread was unable to start, or thread was already started.
 */
bool Client::startThread() {
    if(mReadThread->isDone() ) {
        mReadThread->start();
        //Logger::log("Read Thread Sucessfully Started.\n");
        return true;
    }else {
        //Logger::log("Read Thread has already started! Or other unknown reason.\n");
        return false;
    }
}
/**
 * @brief restarts currently active connection to server
 * 
 */
void Client::restartConnection() {

    if (!sInstance) {
        //Logger::log("Static Instance is null!\n");
        return;
    }

    sead::ScopedCurrentHeapSetter setter(sInstance->mHeap);

    //Logger::log("Sending Disconnect.\n");

    PlayerDC *playerDC = new PlayerDC();

    playerDC->mUserID = sInstance->mUserID;

    sInstance->mSocket->queuePacket(playerDC);

    if (sInstance->mSocket->closeSocket()) {
        //Logger::log("Sucessfully Closed Socket.\n");
    }

    sInstance->mConnectCount = 0;

    sInstance->mIsConnectionActive = sInstance->mSocket->init(sInstance->mServerIP.cstr(), sInstance->mServerPort).isSuccess();

    if(sInstance->mSocket->getLogState() == SOCKET_LOG_CONNECTED) {

        //Logger::log("Reconnect Sucessful!\n");

    } else {
        //Logger::log("Reconnect Unsuccessful.\n");
    }
}
/**
 * @brief starts a connection using client's TCP socket class, pulling up the software keyboard for user inputted IP if save file does not have one saved.
 * 
 * @return true if successful connection to server
 * @return false if connection was unable to establish
 */
bool Client::startConnection() {

    bool isNeedSave = false;

    bool isOverride = al::isPadHoldZL(-1);

    if (mServerIP.isEmpty() || isOverride) {
        mKeyboard->setHeaderText(u"Save File does not contain an IP!");
        mKeyboard->setSubText(u"Please set a Server IP Below.");
        mServerIP = "127.0.0.1";
        Client::openKeyboardIP();
        isNeedSave = true;
    }

    if (!mServerPort || isOverride) {
        mKeyboard->setHeaderText(u"Save File does not contain a port!");
        mKeyboard->setSubText(u"Please set a Server Port Below.");
        mServerPort = 1027;
        Client::openKeyboardPort();
        isNeedSave = true;
    }

    if (isNeedSave) {
        SaveDataAccessFunction::startSaveDataWrite(mHolder.mData);
    }

    mIsConnectionActive = mSocket->init(mServerIP.cstr(), mServerPort).isSuccess();

    if (mIsConnectionActive) {

        //Logger::log("Sucessful Connection. Waiting to recieve init packet.\n");

        bool waitingForInitPacket = true;
        // wait for client init packet

        while (waitingForInitPacket) {

            Packet *curPacket = mSocket->tryGetPacket();

            if (curPacket) {
                
                if (curPacket->mType == PacketType::CLIENTINIT) {
                    InitPacket* initPacket = (InitPacket*)curPacket;

                    //Logger::log("Server Max Player Size: %d\n", initPacket->maxPlayers);

                    waitingForInitPacket = false;
                }

                free(curPacket);

            } else {
                //Logger::log("Recieve failed! Stopping Connection.\n");
                mIsConnectionActive = false;
                waitingForInitPacket = false;
            }
        }
    }

    return mIsConnectionActive;
}

/**
 * @brief Opens up OS's software keyboard in order to change the currently used server IP.
 * @returns whether or not a new IP has been defined and needs to be saved.
 */
bool Client::openKeyboardIP() {

    if (!sInstance) {
        //Logger::log("Static Instance is null!\n");
        return false;
    }

    // opens swkbd with the initial text set to the last saved IP
    sInstance->mKeyboard->openKeyboard(
        sInstance->mServerIP.cstr(), [](nn::swkbd::KeyboardConfig& config) {
            config.keyboardMode = nn::swkbd::KeyboardMode::ModeASCII;
            config.textMaxLength = MAX_HOSTNAME_LENGTH;
            config.textMinLength = 1;
            config.isUseUtf8 = true;
            config.inputFormMode = nn::swkbd::InputFormMode::OneLine;
        });

    hostname prevIp = sInstance->mServerIP;

    while (true) {
        if (sInstance->mKeyboard->isThreadDone()) {
            if(!sInstance->mKeyboard->isKeyboardCancelled())
                sInstance->mServerIP = sInstance->mKeyboard->getResult();
            break;
        }
        nn::os::YieldThread(); // allow other threads to run
    }

    bool isFirstConnect = prevIp != sInstance->mServerIP;

    sInstance->mSocket->setIsFirstConn(isFirstConnect);

    return isFirstConnect;
}

/**
 * @brief Opens up OS's software keyboard in order to change the currently used server port.
 * @returns whether or not a new port has been defined and needs to be saved.
 */
bool Client::openKeyboardPort() {

    if (!sInstance) {
        //Logger::log("Static Instance is null!\n");
        return false;
    }

    // opens swkbd with the initial text set to the last saved port
    char buf[6];
    nn::util::SNPrintf(buf, 6, "%u", sInstance->mServerPort);

    sInstance->mKeyboard->openKeyboard(buf, [](nn::swkbd::KeyboardConfig& config) {
        config.keyboardMode = nn::swkbd::KeyboardMode::ModeNumeric;
        config.textMaxLength = 5;
        config.textMinLength = 2;
        config.isUseUtf8 = true;
        config.inputFormMode = nn::swkbd::InputFormMode::OneLine;
    });

    int prevPort = sInstance->mServerPort;

    while (true) {
        if (sInstance->mKeyboard->isThreadDone()) {
            if(!sInstance->mKeyboard->isKeyboardCancelled())
                sInstance->mServerPort = ::atoi(sInstance->mKeyboard->getResult());
            break;
        }
        nn::os::YieldThread(); // allow other threads to run
    }

    bool isFirstConnect = prevPort != sInstance->mServerPort;

    sInstance->mSocket->setIsFirstConn(isFirstConnect);

    return isFirstConnect;
}

/**
 * @brief main thread function for read thread, responsible for processing packets from server
 * 
 */
void Client::readFunc() {

    if (waitForGameInit) {
        nn::os::YieldThread(); // sleep the thread for the first thing we do so that game init can finish
        nn::os::SleepThread(nn::TimeSpan::FromSeconds(2));
        waitForGameInit = false;
    }

    mConnectStatus->appear();

    al::startAction(mConnectStatus, "Loop", "Loop");

    if (!startConnection()) {

        //Logger::log("Failed to Connect to Server.\n");

        nn::os::SleepThread(nn::TimeSpan::FromNanoSeconds(250000000)); // sleep active thread for 0.25 seconds

        mConnectStatus->end();
                
        return;
    }

    nn::os::SleepThread(nn::TimeSpan::FromNanoSeconds(500000000)); // sleep for 0.5 seconds to let connection layout fully show (probably should find a better way to do this)

    mConnectStatus->end();

    while(mIsConnectionActive) {

        Packet *curPacket = mSocket->tryGetPacket();  // will block until a packet has been recieved, or socket disconnected

        if (curPacket) {

            switch (curPacket->mType)
            {
            case PacketType::PLAYERCON:
                updatePlayerConnect((PlayerConnect*)curPacket);
                break;
            case PacketType::PLAYERDC:
                //Logger::log("Received Player Disconnect!\n");
                curPacket->mUserID.print();
                disconnectPlayer((PlayerDC*)curPacket);
                break;
            case PacketType::CHANGESTAGE:
                sendToStage((ChangeStagePacket*)curPacket);
                break;
            case PacketType::CLIENTINIT: {
                InitPacket* initPacket = (InitPacket*)curPacket;
                //Logger::log("Server Max Player Size: %d\n", initPacket->maxPlayers);
                break;
            }
            default:
                //Logger::log("Discarding Unknown Packet Type.\n");
                break;
            }

            free(curPacket);

        }else { // if false, socket has errored or disconnected, so close the socket and end this thread.
            //Logger::log("Client Socket Encountered an Error! Errno: 0x%x\n", mSocket->socket_errno);
        }

    }

    //Logger::log("Client Read Thread ending.\n");
}

/**
 * @brief 
 * 
 * @param packet 
 */
void Client::updatePlayerConnect(PlayerConnect* packet) {
    packet->mUserID.print("Player Connected! ID");
    mConnectCount++;
}

/**
 * @brief 
 * 
 * @param packet 
 */
void Client::sendToStage(ChangeStagePacket* packet) {
    if (mSceneInfo && mSceneInfo->mSceneObjHolder) {

        GameDataHolderAccessor accessor(mSceneInfo->mSceneObjHolder);

        //Logger::log("Sending Player to %s at Entrance %s in Scenario %d\n", packet->changeStage,
                     packet->changeID, packet->scenarioNo);
        
        ChangeStageInfo info(accessor.mData, packet->changeID, packet->changeStage, false, packet->scenarioNo, static_cast<ChangeStageInfo::SubScenarioType>(packet->subScenarioType));
        info.setWipeType("FadeBlack");
        GameDataFunction::tryChangeNextStage(accessor, &info);
    }
}

/**
 * @brief 
 * 
 * @param packet 
 */
void Client::disconnectPlayer(PlayerDC *packet) {
    mConnectCount--;
}

/**
 * @brief 
 * 
 * @param holder 
 */
void Client::setStageInfo(GameDataHolderAccessor holder) {
    if (sInstance) {
        sInstance->mStageName = GameDataFunction::getCurrentStageName(holder);
        sInstance->mScenario = holder.mData->mGameDataFile->getScenarioNo();
    }
}

/**
 * @brief 
 * 
 */
void Client::update() {
    if (sInstance) {

    }
}

/**
 * @brief 
 * 
 */
void Client::clearArrays() {
    if(sInstance) {
        sInstance->mShineArray.clear();
    }
}

/**
 * @brief 
 * 
 * @return Keyboard* 
 */
Keyboard* Client::getKeyboard() {
    if (sInstance) {
        return sInstance->mKeyboard;
    }
    return nullptr;
}

/**
 * @brief 
 * 
 * @return const char* 
 */
const char* Client::getCurrentIP() {
    if (sInstance) {
        return sInstance->mServerIP.cstr();
    }
    return nullptr;
}

/**
 * @brief 
 * 
 * @return const int 
 */
const int Client::getCurrentPort() {
    if (sInstance) {
        return sInstance->mServerPort;
    }
    return -1;
}

/**
 * @brief sets server IP to supplied string, used specifically for loading IP from the save file.
 * 
 * @param ip 
 */
void Client::setLastUsedIP(const char* ip) {
    if (sInstance) {
        sInstance->mServerIP = ip;
    }
}

/**
 * @brief sets server port to supplied string, used specifically for loading port from the save file.
 * 
 * @param port 
 */
void Client::setLastUsedPort(const int port) {
    if (sInstance) {
        sInstance->mServerPort = port;
    }
}

/**
 * @brief creates new scene info and copies supplied info to the new info, as well as stores a const ptr to the current stage scene.
 * 
 * @param initInfo 
 * @param stageScene 
 */
void Client::setSceneInfo(const al::ActorInitInfo& initInfo, const StageScene *stageScene) {

    if (!sInstance) {
        //Logger::log("Client Null!\n");
        return;
    }

    sInstance->mSceneInfo = new al::ActorSceneInfo();

    memcpy(sInstance->mSceneInfo, &initInfo.mActorSceneInfo, sizeof(al::ActorSceneInfo));

    sInstance->mCurStageScene = stageScene;
}


void Client::showConnectError(const char16_t* msg) {
    if (!sInstance)
        return;

    sInstance->mConnectionWait->setTxtMessageConfirm(msg);

    al::hidePane(sInstance->mConnectionWait, "Page01");  // hide A button prompt

    if (!sInstance->mConnectionWait->mIsAlive) {
        sInstance->mConnectionWait->appear();

        sInstance->mConnectionWait->playLoop();
    }

    al::startAction(sInstance->mConnectionWait, "Confirm", "State");
}

void Client::showConnect() {
    if (!sInstance)
        return;
    
    sInstance->mConnectionWait->appear();

    sInstance->mConnectionWait->playLoop();
    
}

void Client::hideConnect() {
    if (!sInstance)
        return;

    sInstance->mConnectionWait->tryEnd();
}
