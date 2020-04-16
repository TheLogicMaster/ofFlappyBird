#pragma once

#include "ofImage.h"
#include <types/ofRectangle.h>

class Hitmask {

public:
    Hitmask();
    Hitmask(ofImage& image);

    bool checkCollision(ofRectangle rect, ofRectangle otherRect, Hitmask& otherMask);
    std::vector<std::vector<bool>> getMask();
    void verticalFlip();

private:
    std::vector<std::vector<bool>> mask;
};

