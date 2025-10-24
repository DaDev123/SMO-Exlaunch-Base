#include <exlaunch.hpp>

#include "Library/Bgm/BgmLineFunction.h"
HOOK_DEFINE_TRAMPOLINE(StageSceneInitHook) {
        static void Callback(StageScene *stageScene, const al::SceneInitInfo& info) {
              if (al::isPlayingBgm(stageScene)) {
                  al::stopAllBgm(stageScene, 0);
               }
           Orig(stageScene, info);
        }

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking environment. */
    exl::hook::Initialize();
    StageSceneInitHook::InstallAtSymbol("_ZN10StageScene4initERKN2al13SceneInitInfoE");
};
