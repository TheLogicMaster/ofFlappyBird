#pragma once

#include "ofMain.h"
#include "hitmask.h"
#include "animation.h"

#define PIPE_VEL -120.f // Speed at which pipes move
#define PIPE_GAP 100 // Gap between top and bottom pipes
#define PIPE_SPACING 200 // Distance between pipes
#define PIPE_MAX 270 // Pipe max height
#define PIPE_MIN 100 // Pipe min height
#define MAX_VEL 500.f // Clamped max velocity
#define MIN_VEL -250.f // Clamped min velocity
#define GRAVITY_ACC 700.f // Acceleration due to gravity
#define PLAYER_ANIM_LENGTH 0.15f // Length of frame
#define TITLE_BIRD_HEIGHT 250.f // Center height for oscillation
#define TITLE_BIRD_OSCILLATION 15.f // Magnitude of oscillation
#define TITLE_BIRD_SPEED 45.f // Speed of oscillation
#define ANGULAR_ACC_ALIVE 120.f // The rotational acceleration while alive
#define ANGULAR_ACC_DEAD 170.f // The rotational acceleration while dead
#define PLAYER_X 80 // The player's horizontal position on the screen

class ofApp : public ofBaseApp {

public:

    ofImage baseImage, messageImage, gameoverImage;
    ofImage numberImages[10], pipeImages[2], backgroundImages[2];
    ofImage birdImages[3][3];
    ofSoundPlayer wingSound, dieSound, hitSound, pointSound, swooshSound;
    int gameState;
    Hitmask playerMasks[3];
    Hitmask pipeMaskTop, pipeMaskBottom, groundMask;
    vector<ofVec2f> pipes;
    float playerRotation;
    float playerPosition, playerVelocity;
    int playerAnimCycle[4] = {0, 1, 2, 1};
    float playerAnimTime; // Time since
    Animation playerAnimations[3];
    int maxBaseShift;
    float baseShift;
    int playerColorIndex, pipeColorIndex, backgroundIndex;
    int score;
    bool clicked;

    // Event methods
    void setup() override;

    void update() override;

    void draw() override;

    void keyPressed(int key) override;

    void keyReleased(int key) override;

    void mousePressed(int x, int y, int button) override;

    void mouseReleased(int x, int y, int button) override;

    // Game methods
    void addPipe();

    void drawScore();

    void click();

    void randomizeColors();
};

