#include <exlaunch.hpp>

#include "Library/Bgm/BgmLineFunction.h"
#include "Project/Scene/SceneInitInfo.h"
#include "Scene/StageScene.h"
#include "Library/Scene/SceneObjUtil.h"
#include "MovieTestLayout.hpp"


HOOK_DEFINE_TRAMPOLINE(InstallCustomLayouts){
    static void Callback(StageSceneLayout* thisPtr, const char* char1, const al::LayoutInitInfo& layoutInitInfo, const al::PlayerHolder* playerHolder, const al::SubCameraRenderer* subCameraRenderer){
        Orig(thisPtr, char1, layoutInitInfo, playerHolder, subCameraRenderer);
        
        auto* movieLayout = new MovieTestLayout("TestLayout", layoutInitInfo);
        movieLayout->appear();
        
    }
};



extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();
    StageSceneControl::InstallAtSymbol("_ZN10StageScene7controlEv");
    InstallCustomLayouts::InstallAtSymbol("_ZN16StageSceneLayoutC1EPKcRKN2al14LayoutInitInfoEPKNS2_12PlayerHolderEPKNS2_17SubCameraRendererE");
}
