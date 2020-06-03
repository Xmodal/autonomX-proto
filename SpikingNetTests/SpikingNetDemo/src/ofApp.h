#pragma once

#include <iomanip>
#include <fstream>
#include <time.h>
#include <sys/stat.h>
#include <math.h>
#include <string>
#include <cmath>

#include "ofMain.h"

#include "Parameters.h"
#include "SpikingNet.h"
#include "SpikeNetWriter.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void setFrameRate(int & fps_);

    ofFbo fbo;

private:
    void drawNeuron();
    void drawSynapse();
    void updateHistory();
    void drawHistory();
    void updateFbo();
    void drawSubDisplay();
    void drawSubNeuron();
    
    const static int DRAW_SIZE = 20;
    const int HISTORY_SIZE = 50;
    std::vector<std::array<float, DRAW_SIZE>> history;
    

    SpikingNet spike_net;
    SpikeNetWriter writer_weight, writer_spike;
    ofxPanel gui;
    ofxToggle view_mode;
    ofxIntSlider fps;

};
