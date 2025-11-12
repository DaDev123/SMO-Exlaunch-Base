
#include "utilheader.hpp"

#include "MoviePlayer.hpp"
#include "MovieTestLayout.hpp"

#include "Library/Scene/SceneObjHolder.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"

#include <cstdio>
#include <cstring>

namespace {
NERVE_IMPL(MovieTestLayout, Decode)
NERVE_IMPL(MovieTestLayout, WaitForInput)
NERVE_IMPL(MovieTestLayout, Play)

NERVES_MAKE_NOSTRUCT(MovieTestLayout, Decode);
NERVES_MAKE_STRUCT(MovieTestLayout, WaitForInput, Play);
}  // namespace

namespace al {
    al::ISceneObj* createSceneObj(const al::IUseSceneObjHolder*, int);
    void updateTextureInfo(nn::ui2d::TextureInfo*, const agl::TextureData&);
    nn::ui2d::TextureInfo* createTextureInfo();
    const agl::TextureData* getRed2DTexture();
    void setPaneTexture(al::IUseLayout*, char const*, nn::ui2d::TextureInfo const*);
}

MovieTestLayout::MovieTestLayout(const char* name, const al::LayoutInitInfo& initInfo)
    : al::LayoutActor(name) {
    al::initLayoutActor(this, initInfo, name, 0);
    initNerve(&NrvMovieTestLayout.Decode, 0);

    mMoviePlayer = static_cast<MoviePlayer*>(al::createSceneObj(this, 0x24));
    mTexture = al::createTextureInfo();
}

void MovieTestLayout::appear() {
    al::setNerve(this, &NrvMovieTestLayout.Decode);

    mMoviePlayer->play("content:/MovieData/old_1.mp4");

    al::LayoutActor::appear();
}

void MovieTestLayout::exeDecode() {
    mMoviePlayer->update();
    if (mMoviePlayer->isDecode()) {
        al::setNerve(this, &NrvMovieTestLayout.WaitForInput);
    }
}

void MovieTestLayout::exeWaitForInput() {
    if (al::isFirstStep(this)) {
        al::hidePane(this, "TestAsset");
    }
    // if (al::isActionEnd(this, 0)) {
    //     al::setNerve(this, &nrvMovieTestLayoutWait);
    // }
    if (al::isPadTriggerRight(-1)) {
        al::setNerve(this, &NrvMovieTestLayout.Play);
    }
}

void MovieTestLayout::exePlay() {
    if (al::isFirstStep(this)) {
        al::showPane(this, "TestAsset");
    }

    mMoviePlayer->update();
    auto* texture = mMoviePlayer->getTexture();
    al::updateTextureInfo(mTexture, *texture);
    al::setPaneTexture(this, "TestAsset", mTexture);
    if (mMoviePlayer->isLooped()) {
        mMoviePlayer->stop();
        kill();
        appear();
    }
}


