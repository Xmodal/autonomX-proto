//
//  subApp.cpp
//  Gacs_ICC2018
//
//  Created by Atsushi Masumori on 2018/05/30.
//

#include "subApp.hpp"

//--------------------------------------------------------------
void subApp::setup(){
    ofSetVerticalSync(false);
    // fullscreen on sub-display
    ofSetWindowPosition(1921,0);
    ofSetFullscreen(true);

}


//--------------------------------------------------------------
void subApp::update(){
    ofSetFrameRate(200);
}

//--------------------------------------------------------------
void subApp::draw(){
//    fboTexture.draw();
    mainApp->fbo.getTexture().draw(0, 0, ofGetWidth(), ofGetHeight());
}


//--------------------------------------------------------------
void subApp::keyPressed(int key){

}

//--------------------------------------------------------------
void subApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void subApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void subApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void subApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void subApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void subApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void subApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void subApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void subApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void subApp::dragEvent(ofDragInfo dragInfo){
    
}

