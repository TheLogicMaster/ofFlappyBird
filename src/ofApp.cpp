#include "ofApp.h"

#ifdef TARGET_LINUX

#include <X11/XKBlib.h>
#include <X11/Xatom.h>

#endif

void ofApp::setup() {
    ofBackground(255, 255, 255);

    ofSetVerticalSync(true);

    // Images
    backgroundImages[0].load("sprites/background-day.png");
    backgroundImages[1].load("sprites/background-night.png");
    baseImage.load("sprites/base.png");
    messageImage.load("sprites/message.png");
    pipeImages[0].load("sprites/pipe-green.png");
    pipeImages[1].load("sprites/pipe-red.png");
    gameoverImage.load("sprites/gameover.png");
    birdImages[0][0].load("sprites/redbird-downflap.png");
    birdImages[0][1].load("sprites/redbird-midflap.png");
    birdImages[0][2].load("sprites/redbird-upflap.png");
    birdImages[1][0].load("sprites/bluebird-downflap.png");
    birdImages[1][1].load("sprites/bluebird-midflap.png");
    birdImages[1][2].load("sprites/bluebird-upflap.png");
    birdImages[2][0].load("sprites/yellowbird-downflap.png");
    birdImages[2][1].load("sprites/yellowbird-midflap.png");
    birdImages[2][2].load("sprites/yellowbird-upflap.png");
    // Number images
    char name[14];
    for (int i = 0; i <= 9; i++) {
        sprintf(name, "sprites/%d.png", i);
        numberImages[i].load(name);
    }

    //Player animations
    for (int i = 0; i < 3; i++)
        playerAnimations[i] = Animation(birdImages[i], playerAnimCycle, 3);

    // Sounds
    wingSound.load("audio/wing.wav");
    dieSound.load("audio/die.wav");
    hitSound.load("audio/hit.wav");
    pointSound.load("audio/point.wav");
    swooshSound.load("audio/swoosh.wav");

    // Generate player hitmasks
    for (int i = 0; i < 3; i++)
        playerMasks[i] = Hitmask(birdImages[0][i]);

    // Pipe hitmasks
    pipeMaskBottom = Hitmask(pipeImages[pipeColorIndex]);
    pipeMaskTop = Hitmask(pipeImages[pipeColorIndex]);

    //for (auto &column: pipeMaskTop)
    //    reverse(column.begin(), column.end());
    pipeMaskTop.verticalFlip();

    // Ground mask
    groundMask = Hitmask(baseImage);

    // Change window icon
#ifdef TARGET_LINUX
    ofPixels &iconPixels = birdImages[0][0].getPixels();
    int length = 2 + iconPixels.getWidth() * iconPixels.getHeight();
    vector<unsigned long> buffer(length);
    buffer[0] = iconPixels.getWidth();
    buffer[1] = iconPixels.getHeight();
    for (size_t i = 0; i < iconPixels.getWidth() * iconPixels.getHeight(); i++) {
        buffer[i + 2] = iconPixels[i * 4 + 3] << 24;
        buffer[i + 2] += iconPixels[i * 4 + 0] << 16;
        buffer[i + 2] += iconPixels[i * 4 + 1] << 8;
        buffer[i + 2] += iconPixels[i * 4 + 2];
    }
    XChangeProperty(ofGetX11Display(), ofGetX11Window(), XInternAtom(ofGetX11Display(), "_NET_WM_ICON", False),
                    XA_CARDINAL, 32,
                    PropModeReplace, (const unsigned char *) buffer.data(), length);
    XFlush(ofGetX11Display());
#endif

    maxBaseShift = backgroundImages[0].getWidth() - baseImage.getWidth();

    playerVelocity = TITLE_BIRD_SPEED;
    playerPosition = TITLE_BIRD_HEIGHT;
}

void ofApp::update() {
    bool pipeNeeded = false;

    if (gameState != 2) {
        // Shift base
        baseShift += PIPE_VEL * ofGetLastFrameTime();
        if (baseShift < maxBaseShift)
            baseShift = 0;

        // Animate player
        playerAnimTime += ofGetLastFrameTime();
        if (playerAnimTime > PLAYER_ANIM_LENGTH) {
            playerAnimations[playerColorIndex].increment();
            playerAnimTime = 0;
        }
    }

    if (gameState == 1) {
        // Collision testing
        ofRectangle playerRect = ofRectangle();
        playerRect.setFromCenter(PLAYER_X, playerPosition, birdImages[0][0].getWidth(), birdImages[0][0].getHeight());
        // Ground
        ofRectangle groundRect = ofRectangle(0, ofGetHeight() - baseImage.getHeight() + 4, ofGetWidth(),
                                             baseImage.getHeight() + 4);
        //pixelCollision(playerRect, groundRect, playerMasks[playerAnimFrame], groundMask)
        if (playerMasks[playerAnimations[playerColorIndex].getFrame()].checkCollision(playerRect, groundRect, groundMask)) {
            gameState = 2;
            hitSound.play();
        }
        // Pipes
        for (auto &pipe: pipes) {
            ofRectangle pipeRectTop = ofRectangle(pipe.x - pipeImages[pipeColorIndex].getWidth() / 2.f,
                                                  pipe.y - PIPE_GAP / 2.f - pipeImages[pipeColorIndex].getHeight(),
                                                  pipeImages[pipeColorIndex].getWidth(),
                                                  pipeImages[pipeColorIndex].getHeight());
            ofRectangle pipeRectBottom = ofRectangle(pipe.x - pipeImages[pipeColorIndex].getWidth() / 2.f,
                                                     pipe.y + PIPE_GAP / 2.f,
                                                     pipeImages[pipeColorIndex].getWidth(),
                                                     pipeImages[pipeColorIndex].getHeight());
            //pixelCollision(playerRect, pipeRectBottom, playerMasks[playerAnimFrame], pipeMaskBottom) || pixelCollision(playerRect, pipeRectTop, playerMasks[playerAnimFrame], pipeMaskTop)
            if (playerMasks[playerAnimations[playerColorIndex].getFrame()].checkCollision(playerRect, pipeRectBottom, pipeMaskBottom)
                || playerMasks[playerAnimations[playerColorIndex].getFrame()].checkCollision(playerRect, pipeRectTop, pipeMaskTop)) {
                gameState = 2;
                hitSound.play();
                dieSound.play();
            }
        }
    }

    if (gameState > 0)
        glm::clamp(playerVelocity, MIN_VEL, MAX_VEL);
    playerPosition += playerVelocity * ofGetLastFrameTime();
    playerPosition = glm::clamp(playerPosition, -15.f, ofGetHeight() - 100.f);

    switch (gameState) {
        case 0: // Title screen
            if (abs(TITLE_BIRD_HEIGHT - playerPosition) > TITLE_BIRD_OSCILLATION)
                playerVelocity = copysign(TITLE_BIRD_SPEED, TITLE_BIRD_HEIGHT - playerPosition);
            break;
        case 1: // Game screen
            playerVelocity += GRAVITY_ACC * ofGetLastFrameTime();
            if (playerRotation < 45)
                playerRotation += ANGULAR_ACC_ALIVE * ofGetLastFrameTime();

            // Pipe movement
            // Erase offscreen pipes
            if (!pipes.empty() && pipes.at(0).x < -pipeImages[pipeColorIndex].getWidth() / 2.)
                pipes.erase(pipes.begin());
            for (auto &pipe : pipes) {
                float prev = pipe.x;
                pipe.x += PIPE_VEL * ofGetLastFrameTime();
                // Add new pipe when passing coordinate
                if (prev > PIPE_SPACING && pipe.x <= PIPE_SPACING)
                    pipeNeeded = true;
                // Add to score when a pipe passes this location
                if (prev > 10 && pipe.x <= 10) {
                    score++;
                    pointSound.play();
                }
            }
            if (pipeNeeded)
                addPipe();
            break;
        case 2: // Gameover screen
            if (playerPosition < ofGetHeight() - baseImage.getHeight()) {
                playerVelocity += GRAVITY_ACC * ofGetLastFrameTime();
                if (playerRotation < 90)
                    playerRotation += ANGULAR_ACC_DEAD * ofGetLastFrameTime();
            } else
                playerVelocity = 0;
            break;
        default:

            break;
    }
}

void ofApp::draw() {
    backgroundImages[backgroundIndex].draw(0, 0);

    if (gameState > 0)
        for (ofVec2f pipe: pipes) {
            ofPushMatrix();
            pipeImages[pipeColorIndex].setAnchorPercent(0.5, 0);
            ofTranslate(pipe.x, pipe.y + PIPE_GAP / 2.f);
            pipeImages[pipeColorIndex].draw(0, 0);
            ofTranslate(0, -PIPE_GAP);
            ofRotateDeg(180);
            pipeImages[pipeColorIndex].draw(0, 0);
            ofPopMatrix();
        }

    baseImage.draw((int) baseShift, ofGetHeight() - baseImage.getHeight() + 4);

    switch (gameState) {
        case 0: // Title screen
            messageImage.setAnchorPercent(0.5, 0.5);
            messageImage.draw(ofGetWidth() / 2.f, ofGetHeight() / 2.f - 60);
            break;
        case 1: // Game screen

            break;
        case 2: // Gameover screen
            gameoverImage.setAnchorPercent(0.5, 0.5);
            gameoverImage.draw(ofGetWidth() / 2.f, ofGetHeight() / 2.f - 50);
            break;
    }

    if (gameState > 0)
        drawScore();

    playerAnimations[playerColorIndex].draw(PLAYER_X, playerPosition, playerRotation);
}

void ofApp::drawScore() {
    int digits = score > 0 ? floor(log10(score) + 1) : 1;
    int width = 25;
    for (int i = 0; i < digits; i++) {
        int num = score > 0 ? (int) (score / pow(10, digits - i - 1)) % 10 : 0;
        numberImages[num].draw(ofGetWidth() / 2.f - digits * width / 2. + i * width, 50);
    }
}

void ofApp::addPipe() {
    pipes.emplace_back(ofVec2f(350, PIPE_MIN + rand() % (PIPE_MAX - PIPE_MIN)));
}

void ofApp::randomizeColors() {
    playerColorIndex = rand() % 3;
    pipeColorIndex = rand() % 2;
    backgroundIndex = rand() % 2;
}

void ofApp::keyPressed(int key) {
    switch (key) {
        case OF_KEY_ESC:
            OF_EXIT_APP(0);
        case ' ':
        case OF_KEY_UP:
            click();
            break;
        default:
            break;
    }
}

void ofApp::keyReleased(int key) {
    if (key == OF_KEY_UP || key == ' ')
        clicked = false;
}

void ofApp::click() {
    if (clicked)
        return;
    clicked = true;
    wingSound.play();
    switch (gameState) {
        case 0:
            score = 0;

            // Might not need
            playerAnimTime = 0;
            baseShift = 0;

            addPipe();
            gameState = 1;
            playerVelocity = -40;
            playerRotation = -45;
            break;
        case 1:
            playerVelocity = MIN_VEL;
            playerRotation = -45;
            break;
        case 2:
            pipes.clear();
            playerVelocity = TITLE_BIRD_SPEED;
            playerPosition = TITLE_BIRD_HEIGHT;
            playerRotation = 0;
            gameState = 0;
            randomizeColors();
            break;
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    if (button == OF_MOUSE_BUTTON_LEFT)
        click();
}

void ofApp::mouseReleased(int x, int y, int button) {
    clicked = false;
}