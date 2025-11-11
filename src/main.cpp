#include <exlaunch.hpp>

#include "Library/Bgm/BgmLineFunction.h"
#include "Project/Scene/SceneInitInfo.h"
#include "Scene/StageScene.h"

HOOK_DEFINE_TRAMPOLINE(StageSceneControlHook) {
    static void Callback(StageScene *stageScene, const al::SceneInitInfo& info) {
        if (al::isPlayingBgm(stageScene)) {
            al::stopAllBgm(stageScene, 0);
        }
        Orig(stageScene, info);
    }
};




#include "Library/LiveActor/CreateActorFunction.h"

#include "Scene/ProjectActorFactory.h"



#include "PuppetActor.h"



const al::NameToCreator<al::ActorCreatorFunction> sCustomActorFactoryEntries[] = {

    

    {"PuppetActor", al::createActorFunction<PuppetActor>}

};



HOOK_DEFINE_TRAMPOLINE(ActorFactoryHook) {

    static void Callback(ProjectActorFactory* actorFactory) {

        Orig(actorFactory);



        s32 customActorEntriesCount = sizeof(sCustomActorFactoryEntries) / sizeof(sCustomActorFactoryEntries[0]);

        al::NameToCreator<al::ActorCreatorFunction>* factoryEntries =

            new al::NameToCreator<al::ActorCreatorFunction>[actorFactory->mNumFactoryEntries + customActorEntriesCount];



        for (s32 i = 0; i < actorFactory->mNumFactoryEntries; i++) {

            factoryEntries[i] = actorFactory->mFactoryEntries[i];

        }

        for (s32 i = 0; i < customActorEntriesCount; i++) {

            factoryEntries[actorFactory->mNumFactoryEntries + i] = sCustomActorFactoryEntries[i];

        }



        actorFactory->mFactoryEntries = factoryEntries;

        actorFactory->mNumFactoryEntries += customActorEntriesCount;

    }

};


extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking environment. */
    exl::hook::Initialize();
    StageSceneControlHook::InstallAtSymbol("_ZN10StageScene7controlEv");
    ActorFactoryHook::InstallAtSymbol("_ZN19ProjectActorFactoryC2Ev");
}
