#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;
    settings.resizable = false;
    settings.setSize(288, 512);
    ofCreateWindow(settings);
	ofRunApp( new ofApp());
}
