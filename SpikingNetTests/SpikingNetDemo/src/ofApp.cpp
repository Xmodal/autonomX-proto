#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetBackgroundAuto(true);
    ofSetVerticalSync(false);
    ofSetFrameRate(30);
    ofBackground(0);
    
    spike_net.init();
    gui.setup();
    gui.add(view_mode.setup( "View mode" , true));
    gui.add(fps.setup("FPS", 30, 1, 300));
    fps.addListener(this, &ofApp::setFrameRate);

    
    // settings for log
    std::string save_dir = "/path/to/save/directory/";
    std::cout << save_dir + "spike_data_" << std::endl;
    
    if(ConstParams::SaveSpikes){
        writer_spike.open(save_dir + "spike_data_" + ofGetTimestampString() + ".txt");
    }
    
    if(ConstParams::SaveWeights && ofGetFrameNum() % 10000 == 0){
        writer_weight.open(save_dir + "weight_data_" + ofGetTimestampString() + ".txt");
    }
    
    // initialize history
    for(int i=0; i<HISTORY_SIZE; ++i){
        std::array<float, DRAW_SIZE> current_states;
        for(int i = 0; i<DRAW_SIZE; ++i)
            current_states[i] = 0.;
        history.push_back(current_states);
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    spike_net.update();
    
    if(rand()%100 == 0){//ofGetFrameNum()
        spike_net.stimulation(0, 100); // (input group id, stimlation strength)
        spike_net.stimulation(1, 100); // (input group id, stimlation strength)
    }
    
    // log
    if(ConstParams::SaveSpikes){
        writer_spike.writeSpikes(ofGetFrameNum(), spike_net.getSpikedIds());
    }
    
    if(ConstParams::SaveWeights && ofGetFrameNum() % 1000 == 0){
        writer_weight.writeWeights(ofGetFrameNum(), spike_net.getWeights());
    }
    updateHistory();
}

//--------------------------------------------------------------
void ofApp::drawNeuron(){
    
    int rectsize = 10;
    int draw_mode = 1;
    
    int position_x, position_y;
    
    int n2 = sqrt(ConstParams::Number_Of_Neurons);
    
    for(int i=0; i<ConstParams::Number_Of_Neurons; i++){
        
        //color
//        if(spike_net.neurons[i].isFiring()){
//            if(spike_net.neurons[i].getNeuronType() == ConstParams::Regular_Spiking_Demo || spike_net.neurons[i].getNeuronType() == ConstParams::Regular_Spiking){
//                ofSetColor(255,0,0,200);
//            }else{
//                ofSetColor(0,255,0,200);
//            }
//        }else {
//            ofSetColor(-spike_net.neurons[i].getV());
//        }
        
        ofSetColor(spike_net.neurons[i].getV()+100,200);
        
        // position
        if(view_mode == 1){
            int margin_y = 60;
            position_x = rectsize*(i%20)+20;
            position_y = rectsize*int(i/20)+20 + margin_y;
            ofDrawRectangle(position_x,position_y,rectsize-1,rectsize-1);
        }
    }
    
}
//--------------------------------------------------------------
void ofApp::drawSynapse(){
    
    int rectsize = 30;
    int draw_mode = 1;
    
    int n2 = sqrt(ConstParams::Number_Of_Neurons);
    
    for(int i=0; i<ConstParams::Number_Of_Neurons; ++i){// only excitatory synapse
        for(int j=0; j<ConstParams::Number_Of_Neurons; ++j){
            
            if(spike_net.weights[i][j] > 5){
                
                // color
                int color_value = spike_net.weights[i][j]*(255/ConstParams::Weight_Max);
                ofSetColor(color_value, color_value, 0);
                
                // positions in lattice mode
                if(view_mode == 0){
                    
                    int margin_y = 100;
                    int x1 = rectsize*(i%n2)+rectsize;
                    int y1 = rectsize*int(i/n2)+rectsize;
                    
                    int x2 = rectsize*(j%n2)+rectsize;
                    int y2 = rectsize*int(j/n2)+rectsize;
                    
                    ofDrawLine(x1,y1+margin_y, x2,y2+margin_y);
                    
                // positions in circular mode
                }else if(view_mode == 1){
                    
                    float node_size = 2*rectsize/3;
                    
                    float radius_x = ofGetWindowWidth()/2  - node_size;
                    float radius_y = ofGetWindowHeight()/2 - node_size;
                    float center_x = ofGetWindowWidth()/2;
                    float center_y = ofGetWindowHeight()/2;
                    
                    float d_angles = 360./ConstParams::Number_Of_Neurons;
                    
                    float x1 = center_x + (radius_x * cos(ofDegToRad(d_angles*i)));
                    float y1 = center_y + (radius_y * sin(ofDegToRad(d_angles*i)));
    
                    float x2 = center_x + (radius_x * cos(ofDegToRad(d_angles*j)));
                    float y2 = center_y + (radius_y * sin(ofDegToRad(d_angles*j)));
                    
                    ofDrawLine(x1, y1, x2, y2);
                    
                }
                
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    drawNeuron();
//    drawHistory();
//    drawSubNeuron();
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::drawSubNeuron(){
    
    int rectsize = 1;
    int draw_mode = 1;
    
    int position_x, position_y;
    
    int n2 = sqrt(ConstParams::Number_Of_Neurons);
    
    for(int i=0; i<ConstParams::Number_Of_Neurons; i++){
        
        //color
        //        if(spike_net.neurons[i].isFiring()){
        //            if(spike_net.neurons[i].getNeuronType() == ConstParams::Regular_Spiking_Demo || spike_net.neurons[i].getNeuronType() == ConstParams::Regular_Spiking){
        //                ofSetColor(255,0,0,200);
        //            }else{
        //                ofSetColor(0,255,0,200);
        //            }
        //        }else {
        //            ofSetColor(-spike_net.neurons[i].getV());
        //        }
        
        ofSetColor(spike_net.neurons[i].getV()+100,spike_net.neurons[i].getV()+100,spike_net.neurons[i].getV()+100);
        
        // position
        if(view_mode == 1){
            int margin_y = 0;
            position_y = rectsize*(i%20);
            position_x = rectsize*int(i/20);
            ofDrawRectangle(position_x,position_y,rectsize,rectsize);
        }
    }
    
}


//--------------------------------------------------------------
void ofApp::updateHistory(){
    
    std::array<float, DRAW_SIZE> current_states;
    
    //write colony data
    for(int i=500; i<520; i++){
        current_states[i-500] = (spike_net.neurons[i].getV()+100)*2.;
//        std::cout << current_states[i-500] << ",";
    }
//    std::cout << std::endl;
    
    //update history
    history.push_back(current_states);
    history.erase(history.begin());
    
}

//--------------------------------------------------------------
void ofApp::drawHistory(){
    int height = 30;
    int brightness = 255;
    int cell_size = 10;
    int cell_size_top = 30;
    
    height += cell_size_top;
    
    for(int i=history.size()-1; i>=0; --i){
//        int decay = (history.size()-1 - i)*gradation;
        for(int j=0; j<DRAW_SIZE; ++j){
            ofSetColor(history[i][j],history[i][j],history[i][j]);
            ofDrawRectangle(j*cell_size, height, cell_size, cell_size);
        }
        height += cell_size;
    }
}

//--------------------------------------------------------------
void ofApp::updateFbo(){
    fbo.begin();
    drawSubDisplay();
    fbo.end();
}

//--------------------------------------------------------------
void ofApp::drawSubDisplay(){
    int height = 0;
    int brightness = 255;
    int cell_size = 1;
    int cell_size_top = 0;
    
    height += cell_size_top;
    
    for(int i=history.size()-1; i>=0; --i){
        for(int j=0; j<DRAW_SIZE; ++j){
            ofSetColor(history[i][j],history[i][j],history[i][j]);
            ofDrawRectangle(height, j*cell_size, cell_size, cell_size);
        }
        height += cell_size;
    }

}

//--------------------------------------------------------------
void ofApp::setFrameRate(int & fps){
    ofSetFrameRate(fps);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
