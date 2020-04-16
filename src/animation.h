#pragma once

#include "ofImage.h"
#include "ofGraphics.h"

class Animation {

public:
    Animation();
    Animation(ofImage* frames, int* pattern, int length);

    void draw(int x, int y, float rotation);
    void increment();
    int getFrame();

    ofImage* frames{};
    int* pattern{};

private:
    int frame = 0;
    int length = 0;
};

