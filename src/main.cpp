#include <exlaunch.hpp>

if (al::isPlayingBgm(stageScene)) {
            al::stopAllBgm(stageScene, 0);
}

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking environment. */
    exl::hook::Initialize();
}
