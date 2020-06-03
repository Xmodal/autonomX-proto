#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	
    /*
    ofSetupOpenGL(420,110,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
    */
    
    
    ofGLFWWindowSettings settings;
    
    settings.decorated = false;
    settings.windowMode = OF_WINDOW;
    
    ofCreateWindow(settings);
    ofRunApp(new ofApp());

}
