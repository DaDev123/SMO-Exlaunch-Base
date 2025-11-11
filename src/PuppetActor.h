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

class PuppetActor : public al::LiveActor, public sead::IDisposer {
public:
    PuppetActor(const char* name);
    ~PuppetActor() override;
    virtual void init(al::ActorInitInfo const&) override;
    virtual void initAfterPlacement(void) override;
    virtual void control(void) override;
    virtual void movement(void) override;
    virtual void makeActorAlive(void) override;
    virtual void makeActorDead(void) override;
    virtual void calcAnim(void) override;

    void startAction(const char* actName);
    void hairControl();

    void setBlendWeight(int index, float weight) { al::setSklAnimBlendWeight(getCurrentModel(), weight, index); };

    bool isNeedBlending();

    al::LiveActor* getCurrentModel();

    void debugTeleportCaptures(const sead::Vector3f& pos);

    void debugTeleportCapture(const sead::Vector3f& pos, int index);

    void emitJoinEffect();

private:
    void changeModel(const char* newModel);

    bool setCapture(const char* captureName);

    void syncPose();

    PlayerCostumeInfo* mCostumeInfo = nullptr;
    PlayerModelHolder* mModelHolder = nullptr;

    bool mIs2DModel = false;
    bool mIsCaptureModel = false;
};

PlayerCostumeInfo* initMarioModelPuppet(al::LiveActor* player, const al::ActorInitInfo& initInfo,
                                        char const* bodyName, char const* capName, int subActorNum,
                                        al::AudioKeeper* audioKeeper);
PlayerHeadCostumeInfo* initMarioHeadCostumeInfo(al::LiveActor* player, const al::ActorInitInfo& initInfo, const char* headModelName, const char* capModelName, const char* headType, const char* headSuffix);
