#pragma once

#include "ofMain.h"

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

class ofApp : public ofBaseApp{

	public:

        ofImage backgroundImage, baseImage, messageImage, pipeImage, gameoverImage;
        ofImage numberImages[10];
        ofImage birdImages[3];
        ofSoundPlayer wingSound, dieSound, hitSound, pointSound, swooshSound;
        int gameState;
        vector<vector<bool>> playerMasks[3];
		vector<vector<bool>> pipeMaskTop, pipeMaskBottom, groundMask;
        vector<ofVec2f> pipes;
        float playerRotation;
        float playerPosition, playerVelocity;
        int playerAnimCycle[4] = {0, 1, 2, 1};
        float playerAnimTime; // Time since
        int playerAnimFrame; // Animation frame
        int maxBaseShift;
        float baseShift;

        int score;
        bool clicked;

		void setup() override;
		void update() override;
		void draw() override;
		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;

		vector<vector<bool>> getHitmask(ofImage image);
		bool pixelCollision(ofRectangle rect1, ofRectangle rect2, vector<vector<bool>> hitmask1, vector<vector<bool>> hitmask2);
		void addPipe();
		void drawScore();
		void click();
};

