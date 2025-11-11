#pragma once

#pragma region utils

#include "Library/Controller/InputFunction.h"
#include "Library/Camera/CameraUtil.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/Scene/SceneUtil.h"
#include "Library/Layout/LayoutUtil.h"
#include "Library/Math/MathUtil.h"

#pragma endregion


#include "Library/LiveActor/LiveActor.h"
#include "Library/Thread/FunctorV0M.h"
#include "Library/Thread/AsyncFunctorThread.h"

#include "utilheader.hpp"
#include "Library/Base/StringUtil.h"
#include "Library/Play/Layout/BalloonMessage.h"

#include "Player/PlayerFunction.h"
#include "Player/PlayerConst.h"
#include "Player/PlayerModelHolder.h"


class PuppetActor : public al::LiveActor {
    public:
        PuppetActor(const char *name);
        virtual void init(al::ActorInitInfo const &) override;
        virtual void initAfterPlacement(void) override;
        virtual void control(void) override;
        virtual void movement(void) override;
        virtual void makeActorAlive(void) override;
        virtual void makeActorDead(void) override;
        virtual void calcAnim(void) override;
        
        virtual void attackSensor(al::HitSensor *, al::HitSensor *) override;
        virtual bool receiveMsg(const al::SensorMsg*, al::HitSensor*, al::HitSensor*) override;

        virtual const char* getName() const override {
            if (mInfo)
                return mInfo->puppetName;
            return mName;
        }

        void initOnline(PuppetInfo *pupInfo);

        void startAction(const char *actName);
        void hairControl();

        void setBlendWeight(int index, float weight) { al::setSklAnimBlendWeight(getCurrentModel(), weight, index); };

        bool isNeedBlending();

        bool isInCaptureList(const char *hackName);

        PuppetInfo* getInfo() { return mInfo; }

        bool addCapture(PuppetHackActor *capture, const char *hackType);

        al::LiveActor* getCurrentModel();

        int getMaxCaptures() {return mCaptures->getEntryCount(); };

        void debugTeleportCaptures(const sead::Vector3f& pos);

        void debugTeleportCapture(const sead::Vector3f& pos, int index);

        void emitJoinEffect();

        bool mIsDebug = false;

// not a real symbol, func at 0x445A24
        static void initMarioAudio(al::LiveActor* player, const al::ActorInitInfo& initInfo,
                                   al::Resource* modelRes, bool isDemo,
                                   al::AudioKeeper* audioKeeper);
        // not a real symbol, func at 0x448B8C
        static void initMarioSubModel(al::LiveActor* subactor, const al::ActorInitInfo& initInfo,
                                      bool isInvisible, bool isDemo, bool isChromaKey,
                                      bool isCloset);
        // not a real symbol, func at 0x445128
        static PlayerHeadCostumeInfo* initMarioHeadCostumeInfo(al::LiveActor* player,
                                                               const al::ActorInitInfo& initInfo,
                                                               const char*, const char*,
                                                               const char*, const char*, bool, bool,
                                                               bool, bool, bool);
        // not a real symbol, func at 0x445DF4
        static void initMarioDepthModel(al::LiveActor *player, bool isDemo, bool isChromaKey);
        
    private:
        void changeModel(const char* newModel);

        bool setCapture(const char* captureName);

        void syncPose();

        PlayerCostumeInfo *mCostumeInfo = nullptr;
        PuppetInfo *mInfo = nullptr;
        PuppetCapActor *mPuppetCap = nullptr;
        PlayerModelHolder *mModelHolder = nullptr;
        HackModelHolder* mCaptures = nullptr;
        NameTag *mNameTag = nullptr;

        CaptureTypes::Type mCurCapture = CaptureTypes::Type::Unknown;

        bool mIs2DModel = false;

        bool mIsCaptureModel = false;

        float mClosingSpeed = 0;
};

        
PlayerCostumeInfo* initMarioModelPuppet(al::LiveActor* player, const al::ActorInitInfo& initInfo,
                                        char const* bodyName, char const* capName, int subActorNum,
                                        al::AudioKeeper* audioKeeper);
PlayerHeadCostumeInfo* initMarioHeadCostumeInfo(al::LiveActor* player, const al::ActorInitInfo &initInfo, const char* headModelName, const char* capModelName, const char* headType, const char* headSuffix);
