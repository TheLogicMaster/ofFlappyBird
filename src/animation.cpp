#include "animation.h"

Animation::Animation(ofImage* frames, int* pattern, int length) {
    this->frames = frames;
    this->pattern = pattern;
    this->length = length;
}

Animation::Animation() = default;

void Animation::draw(int x, int y, float rotation) {
    ofPushMatrix();
    ofTranslate(x, y);
    ofImage bird = frames[pattern[frame]];
    bird.setAnchorPercent(0.5, 0.5);
    ofRotateDeg(rotation);
    bird.draw(0, 0);
    ofPopMatrix();
}

void Animation::increment() {
    frame++;
    frame %= length;
}

int Animation::getFrame() {
    return frame;
}
