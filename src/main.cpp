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
        
        // Use reinterpret_cast since we're casting from ISceneObj*
        al::ISceneObj* obj = al::getSceneObj(stageScene, 0x24);
        if (obj) {
            MoviePlayer* player = reinterpret_cast<MoviePlayer*>(obj);
            player->play("glitch");
        }
        
        if (al::isPlayingBgm(stageScene)) {
            al::stopAllBgm(stageScene, 0);
        }
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();
    StageSceneInit::InstallAtSymbol("_ZN10StageScene4initERKN2al13SceneInitInfoE");
}
