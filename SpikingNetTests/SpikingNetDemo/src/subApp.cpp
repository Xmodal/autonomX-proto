//
//  subApp.cpp
//  Gacs_ICC2018
//
// Copyright 2020, Atsushi Masumori, Alexandre Saunier, Simon Demeule
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

