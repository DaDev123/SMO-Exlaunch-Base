#include <exlaunch.hpp>

#include "Library/Bgm/BgmLineFunction.h"
#include "Project/Scene/SceneInitInfo.h"
#include "Scene/StageScene.h"
#include "Library/Scene/SceneObjUtil.h"

class MoviePlayer {
public:
    void play(const char* name);
};

static bool hasAttempted = false;

HOOK_DEFINE_TRAMPOLINE(StageSceneControl) {
    static void Callback(StageScene *stageScene) {
        Orig(stageScene);
        
        if (!hasAttempted) {
            // Just check if the object exists without trying to play
            al::ISceneObj* obj = al::getSceneObj(stageScene, 0x24);
            // Log or breakpoint here to see if obj is valid
            hasAttempted = true;
        }
        
        if (al::isPlayingBgm(stageScene)) {
            al::stopAllBgm(stageScene, 0);
        }
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();
    StageSceneControl::InstallAtSymbol("_ZN10StageScene7controlEv");
}
