/**
 * @file server/Client.hpp
 * @author CraftyBoss (https://github.com/CraftyBoss)
 * @brief main class responsible for handing all client-server related communications, as well as any gamemodes.
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include "Keyboard.hpp"
#include "Library/LiveActor/ActorInitInfo.h"
#include "Library/LiveActor/ActorSceneInfo.h"
#include "Library/Thread/AsyncFunctorThread.h"
#include "Library/Thread/FunctorV0M.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Layout/LayoutInitInfo.h"
#include "Library/Play/Layout/SimpleLayoutAppearWaitEnd.h"
#include "Library/Play/Layout/WindowConfirm.h" //original was WindowConfirmWait.h
#include "utilheader.hpp"
#include "Library/Layout/LayoutActor.h"
#include "Library/Camera/CameraPoser.h"
#include "Library/Camera/CameraPoserFunction.h"

#include <container/seadPtrArray.h>
#include "System/GameDataHolderAccessor.h"
#include "Player/PlayerActorHakoniwa.h"
#include "Scene/StageScene.h"
#include "Player/PlayerFunction.h"
#include "System/GameDataHolderWriter.h"
#include "System/GameDataFunction.h"

#include <heap/seadExpHeap.h>

#include <heap/seadDisposer.h>
#include <math/seadVector.h>
#include <math/seadMatrix.h>
#include <prim/seadSafeString.h>
#include <prim/seadSafeString.hpp>
#include <gfx/seadCamera.h>
#include <basis/seadNew.h>
#include <container/seadSafeArray.h>
#include <thread/seadMutex.h>

#include <nn/account.h>
#include "types.h"

#include "server/SocketClient.hpp"
#include "syssocket/sockdefines.h"
#include "Keyboard.hpp"

#include <cstddef>
#include <stdlib.h>

struct UIDIndexNode {
    nn::account::Uid uid;
    int puppetIndex;
};

class Client {
    SEAD_SINGLETON_DISPOSER(Client)

    public:
        Client();

        void init(al::LayoutInitInfo const &initInfo, GameDataHolderAccessor holder);

        bool startThread();
        void readFunc();
        static void restartConnection();

        static bool isSocketActive() { return sInstance ? sInstance->mSocket->isConnected() : false; };

        static void sendPlayerInfPacket(const PlayerActorBase *player, bool isYukimaru);
        static void sendGameInfPacket(const PlayerActorHakoniwa *player, GameDataHolderAccessor holder);
        static void sendGameInfPacket(GameDataHolderAccessor holder);
        static void sendCostumeInfPacket(const char *body, const char *cap);
        
        static void update();

        static void clearArrays();

        static bool isFirstConnect() { return sInstance ? sInstance->mIsFirstConnect : false;}

        static const char *getClientName() { return sInstance ? sInstance->mUsername.cstr() : "Player"; }

        static sead::Heap *getClientHeap() { return sInstance ? sInstance->mHeap : nullptr; }

        static int getMaxPlayerCount() { return sInstance ? sInstance->maxPuppets + 1 : 10;}

        static const int getCurrentPort();

        static int getConnectCount() {
            if (sInstance)
                return sInstance->mConnectCount;
            return 0;
        }

        static Keyboard* getKeyboard();

        static const char* getCurrentIP();

        static nn::account::Uid getClientId() { return sInstance ? sInstance->mUserID : nn::account::Uid::EmptyId;}
        
        static sead::FixedSafeString<0x20> getUsername() { return sInstance ? sInstance->mUsername : sead::FixedSafeString<0x20>::cEmptyString;}

        static void setStageInfo(GameDataHolderAccessor holder);

        static void setLastUsedIP(const char* ip);

        static void setLastUsedPort(const int port);
    
        static void setSceneInfo(const al::ActorInitInfo& initInfo, const StageScene *stageScene);

        
        static bool openKeyboardIP();
        static bool openKeyboardPort();

        static void showConnect();

        static void showConnectError(const char16_t* msg);

        static void hideConnect();

        
        // public for debug purposes
        SocketClient *mSocket;

    private:
        void updatePlayerConnect(PlayerConnect *packet);
        void sendToStage(ChangeStagePacket* packet);
        void disconnectPlayer(PlayerDC *packet);

        
        bool startConnection();

        // --- General Server Members ---

        al::AsyncFunctorThread *mReadThread = nullptr; // processes data queued in the SocketClient's RecvQueue
        
        int mConnectCount = 0;

        nn::account::Uid mUserID;

        sead::FixedSafeString<0x20> mUsername;

        bool mIsConnectionActive = false;

        // --- Server Syncing Members ---


        Keyboard* mKeyboard = nullptr; // keyboard for setting server IP

        hostname mServerIP;

        int mServerPort = 0;

        bool waitForGameInit = true;
        bool mIsFirstConnect = true;

        // --- Game Layouts ---

        al::WindowConfirmWait* mConnectionWait;

        al::SimpleLayoutAppearWaitEnd *mConnectStatus;

        // --- Game Info ---

        bool isClientCaptured = false;

        bool isSentCaptureInf = false;

        bool isSentHackInf = false;

        al::ActorSceneInfo*
            mSceneInfo  = nullptr;  // TODO: create custom scene info class with only the info we actually need

        const StageScene *mCurStageScene = nullptr;
    
        sead::FixedSafeString<0x40> mStageName;

        GameDataHolderAccessor mHolder;

        u8 mScenario = 0;

        sead::ExpHeap *mHeap = nullptr; // Custom FrameHeap used for all Client related memory
    
};
