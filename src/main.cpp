#include <exlaunch.hpp>

#include "Library/Bgm/BgmLineFunction.h"
#include "Project/Scene/SceneInitInfo.h"
#include "Scene/StageScene.h"
#include "Library/Scene/SceneObjUtil.h"

// Forward declare MoviePlayer since we don't have the header
class MoviePlayer {
public:
    void play(const char* name);
};

HOOK_DEFINE_TRAMPOLINE(StageSceneInit) {
    static void Callback(StageScene *stageScene, const al::SceneInitInfo& info) {
        Orig(stageScene, info);
        
        // Don't play immediately - the movie system might not be ready yet
        // Try moving this to control() with a frame delay
    }
};

static int frameDelay = 0;

HOOK_DEFINE_TRAMPOLINE(StageSceneControl) {
    static void Callback(StageScene *stageScene) {
        Orig(stageScene);
        
        if (frameDelay == 60) { // Wait 60 frames (~1 second)
            al::ISceneObj* obj = al::getSceneObj(stageScene, 0x24);
            if (obj) {
                MoviePlayer* player = reinterpret_cast<MoviePlayer*>(obj);
                player->play("glitch");
            }
            frameDelay++; // Prevent replaying
        } else if (frameDelay < 60) {
            frameDelay++;
        }
        
        if (al::isPlayingBgm(stageScene)) {
            al::stopAllBgm(stageScene, 0);
        }
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();
    StageSceneInit::InstallAtSymbol("_ZN10StageScene4initERKN2al13SceneInitInfoE");
    StageSceneControl::InstallAtSymbol("_ZN10StageScene7controlEv");
}
