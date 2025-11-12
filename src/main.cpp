#include <exlaunch.hpp>

#include "Library/Bgm/BgmLineFunction.h"
#include "Project/Scene/SceneInitInfo.h"
#include "Scene/StageScene.h"
#include "Library/Scene/SceneObjHolder.h"
//#include "Library/Movie/MoviePlayer.h"

HOOK_DEFINE_TRAMPOLINE(StageSceneInit) {
    static void Callback(StageScene *stageScene, const al::SceneInitInfo& info) {
        // Call original init first
        Orig(stageScene, info);
        
        // Play video after scene is initialized
        al::MoviePlayer* player = static_cast<al::MoviePlayer*>(al::getSceneObj(stageScene, 0x24));
        if (player) {
            player->play("glitch");
        }
        
        // Stop BGM if playing
        if (al::isPlayingBgm(stageScene)) {
            al::stopAllBgm(stageScene, 0);
        }
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();
    StageSceneInit::InstallAtSymbol("_ZN10StageScene4initERKN2al13SceneInitInfoE");
}
