#include "utilheader.hpp"
#include <heap/seadDisposer.h>

class MoviePlayer : public al::ISceneObj, public sead::IDisposer {
public:
    MoviePlayer();
    ~MoviePlayer();
    void update();
    void draw(agl::DrawContext*) const;
    agl::TextureData* getTexture() const;
    void play(const char* movieLocation);
    void stop();
    bool isPlay() const;
    bool isDecode() const;
    bool isLooped() const;
};