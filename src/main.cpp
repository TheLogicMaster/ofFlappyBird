#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
#ifdef TARGET_LINUX
    ofGLFWWindowSettings settings;
    settings.resizable = false;
    settings.title = "ofFlappyBird";
    settings.setSize(288, 512);
    ofCreateWindow(settings);
#else
    ofSetupOpenGL(288, 512, OF_WINDOW);
#endif
	ofRunApp( new ofApp());
}
