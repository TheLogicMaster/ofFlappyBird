#include "ofApp.h"

void ofApp::setup() {
    ofBackground(255, 255, 255);

    ofSetVerticalSync(true);

    // Images
    backgroundImage.load("sprites/background-day.png");
    baseImage.load("sprites/base.png");
    messageImage.load("sprites/message.png");
    pipeImage.load("sprites/pipe-green.png");
    gameoverImage.load("sprites/gameover.png");
    birdImages[0].load("sprites/redbird-downflap.png");
    birdImages[1].load("sprites/redbird-midflap.png");
    birdImages[2].load("sprites/redbird-upflap.png");
    // Number images
    char name[14];
    for (int i = 0; i <= 9; i++) {
        sprintf(name, "sprites/%d.png", i);
        numberImages[i].load(name);
    }

    // Sounds
    wingSound.load("audio/wing.wav");
    dieSound.load("audio/die.wav");
    hitSound.load("audio/hit.wav");
    pointSound.load("audio/point.wav");
    swooshSound.load("audio/swoosh.wav");

    // Generate player hitmasks
    for (int i = 0; i < 3; i++)
        playerMasks[i] = getHitmask(birdImages[i]);

    // Pipe hitmasks
    pipeMaskBottom = getHitmask(pipeImage);
    pipeMaskTop = getHitmask(pipeImage);

    for (auto &column: pipeMaskTop)
        reverse(column.begin(), column.end());

    // Ground mask
    groundMask = getHitmask(baseImage);

    maxBaseShift = backgroundImage.getWidth() - baseImage.getWidth();

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
            playerAnimFrame = (playerAnimFrame + 1) % 4;
            playerAnimTime = 0;
        }

        // Collision testing
        ofRectangle playerRect = ofRectangle();
        playerRect.setFromCenter(PLAYER_X, playerPosition, birdImages[0].getWidth(), birdImages[0].getHeight());
        // Ground
        ofRectangle groundRect = ofRectangle(0, ofGetHeight() - baseImage.getHeight() + 4, ofGetWidth(),
                                             baseImage.getHeight() + 4);
        if (pixelCollision(playerRect, groundRect, playerMasks[playerAnimFrame], groundMask))
            gameState = 2;
        // Pipes
        for (auto &pipe: pipes) {
            ofRectangle pipeRectTop = ofRectangle(pipe.x - pipeImage.getWidth() / 2.f,
                                                  pipe.y - PIPE_GAP / 2.f - pipeImage.getHeight(), pipeImage.getWidth(),
                                                  pipeImage.getHeight());
            ofRectangle pipeRectBottom = ofRectangle(pipe.x - pipeImage.getWidth() / 2.f, pipe.y + PIPE_GAP / 2.f,
                                                     pipeImage.getWidth(), pipeImage.getHeight());
            if (pixelCollision(playerRect, pipeRectBottom, playerMasks[playerAnimFrame], pipeMaskBottom)
                || pixelCollision(playerRect, pipeRectTop, playerMasks[playerAnimFrame], pipeMaskTop))
                gameState = 2;
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
            if (!pipes.empty() && pipes.at(0).x < -pipeImage.getWidth() / 2.)
                pipes.erase(pipes.begin());
            for (auto &pipe : pipes) {
                float prev = pipe.x;
                pipe.x += PIPE_VEL * ofGetLastFrameTime();
                // Add new pipe when passing coordinate
                if (prev > PIPE_SPACING && pipe.x <= PIPE_SPACING)
                    pipeNeeded = true;
                // Add to score when a pipe passes this location
                if (prev > 10 && pipe.x <= 10)
                    score++;
            }
            if (pipeNeeded)
                addPipe();
            break;
        case 2: // Gameover screen
            if (playerRotation < 90)
                playerRotation += ANGULAR_ACC_DEAD * ofGetLastFrameTime();
            if (playerPosition < ofGetHeight() - baseImage.getHeight())
                playerVelocity += GRAVITY_ACC * ofGetLastFrameTime();
            else
                playerVelocity = 0;
            break;
        default:

            break;
    }
    //cameraOrbit += ofGetLastFrameTime() * 20.; // 20 degrees per second;
}

void ofApp::draw() {
    backgroundImage.draw(0, 0);

    if (gameState > 0)
        for (ofVec2f pipe: pipes) {
            ofPushMatrix();
            pipeImage.setAnchorPercent(0.5, 0);
            ofTranslate(pipe.x, pipe.y + PIPE_GAP / 2.f);
            pipeImage.draw(0, 0);
            ofTranslate(0, -PIPE_GAP);
            ofRotateDeg(180);
            pipeImage.draw(0, 0);
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

    ofPushMatrix();
    ofTranslate(PLAYER_X, playerPosition);
    int bird = playerAnimCycle[playerAnimFrame];
    birdImages[bird].setAnchorPercent(0.5, 0.5);
    ofRotateDeg(playerRotation);
    birdImages[bird].draw(0, 0);
    ofPopMatrix();

    //stringstream ss;
    //ss << "FPS: " << ofToString(ofGetFrameRate(), 0) << endl << endl;
    //ofDrawBitmapString(ss.str().c_str(), 20, 20);

}

void ofApp::drawScore() {
    int digits = score > 0 ? floor(log10(score) + 1) : 1;
    int width = 25;
    for (int i = 0; i < digits; i++) {
        int num = score > 0 ? (int) (score / pow(10, digits - i - 1)) % 10 : 0;
        numberImages[num].draw(ofGetWidth() / 2.f - digits * width / 2. + i * width, 50);
    }
}

vector<vector<bool>> ofApp::getHitmask(ofImage image) {
    ofPixels pixels = image.getPixels();
    vector<vector<bool>> hitmask = vector<vector<bool>>();
    for (int x = 0; x < pixels.getWidth(); x++) {
        vector<bool> column = vector<bool>();
        for (int y = 0; y < pixels.getHeight(); y++) {
            //printf("X: %d, Y: %d, R: %d, G: %d, B: %d, A: %d, Hue: %f, Brightness: %f, Saturation: %f\n", x, y, pixels.getColor(x, y).r, pixels.getColor(x, y).g, pixels.getColor(x, y).b, pixels.getColor(x, y).a, pixels.getColor(x, y).getHue(), pixels.getColor(x, y).getBrightness(), pixels.getColor(x, y).getSaturation());
            column.push_back(pixels.getColor(x, y).a != 0);
        }
        hitmask.push_back(column);
    }

    /*printf("\nSize: %lu\n", hitmask.size() * hitmask.at(0).size());
    for (int y = 0; y < hitmask.at(0).size(); y++) {
        for (auto &x : hitmask)
            printf("%d", x.at(y) ? 1 : 0);
        printf("\n");
    }*/
    return hitmask;
}

bool ofApp::pixelCollision(ofRectangle rect1, ofRectangle rect2, vector<vector<bool>> hitmask1,
                           vector<vector<bool>> hitmask2) {
    ofRectangle rect = rect1.getIntersection(rect2);

    if (rect.width == 0 || rect.height == 0)
        return false;

    int x1 = rect.x - rect1.x;
    int y1 = rect.y - rect1.y;
    int x2 = rect.x - rect2.x;
    int y2 = rect.y - rect2.y;

    for (int x = 0; x < rect.width; x++)
        for (int y = 0; y < rect.height; y++)
            if (hitmask1.at(x1 + x).at(y1 + y) && hitmask2.at(x2 + x).at(y2 + y))
                return true;
    return false;
}

void ofApp::addPipe() {
    pipes.emplace_back(ofVec2f(350, PIPE_MIN + rand() % (PIPE_MAX - PIPE_MIN)));
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
    switch (gameState) {
        case 0:
            score = 0;

            // Might not need
            playerAnimTime = 0;
            playerAnimFrame = 0;
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
            break;
    }
}

void ofApp::mouseMoved(int x, int y) {

}

void ofApp::mouseDragged(int x, int y, int button) {

}

void ofApp::mousePressed(int x, int y, int button) {
    if (button == OF_MOUSE_BUTTON_LEFT)
        click();
}

void ofApp::mouseReleased(int x, int y, int button) {
    clicked = false;
}

void ofApp::mouseEntered(int x, int y) {

}

void ofApp::mouseExited(int x, int y) {

}

void ofApp::windowResized(int w, int h) {

}

void ofApp::gotMessage(ofMessage msg) {

}

void ofApp::dragEvent(ofDragInfo dragInfo) {

}