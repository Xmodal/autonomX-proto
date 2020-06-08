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

#include "ofApp.h"

// PARAM
#define PORT_IN         7700+SNN_ID
#define PORT_OUT        7600+SNN_ID

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("SNN " + std::to_string(SNN_ID));
    ofSetBackgroundAuto(true);
    ofSetVerticalSync(false);
    ofSetFrameRate(30);
    ofBackground(100);
    
    std::cout.precision(11);
    
    // Init random seed so that all random numbers will be the same
    // in each oF instance.
    srand(0);
    
    spike_net.init(); //just to make sure there is no issue
    
    //------
    //OSC
    //------
    
#ifdef USE_OSC
    osc_sender_msx.setup("localhost", PORT_OUT);
    
    // inhibitory neurons are first, the rest is excitatory neurons
    // input and output neurons are always in the excitatory neurons
    
    // ------ inhibitory ------ | --------------------------- excitatory ------------------------------ |
    // ------ unassigned ------ | ------ input ------ | ------ output ------ | ------ unassigned ------ |
    
    //SUSCRIBE
    ofxSubscribeOsc(PORT_IN, "/SNN/stimulation", stimulation_val);
    ofxSubscribeOsc(PORT_IN, "/SNN/whole_stimulation", whole_stimulation_val);
    
    ofxSubscribeOsc(PORT_IN, "/SNN/neuron_getV_offset", neuron_getV_offset);
    ofxSubscribeOsc(PORT_IN, "/SNN/number_of_neurons", set_number_of_neurons);
    ofxSubscribeOsc(PORT_IN, "/SNN/set_network_type", set_network_type);
    ofxSubscribeOsc(PORT_IN, "/SNN/set_grid_network_width", set_grid_network_width);
    ofxSubscribeOsc(PORT_IN, "/SNN/set_inhibitory_neuron_type", set_inhibitory_neuron_type);
    ofxSubscribeOsc(PORT_IN, "/SNN/set_excitatory_neuron_type", set_excitatory_neuron_type);
    ofxSubscribeOsc(PORT_IN, "/SNN/set_inhibitory_portion", set_inhibitory_portion);// portion: fraction of global neurons dedicated to inhibitory (1/x, where x is the provided number), the rest is excitatory
    ofxSubscribeOsc(PORT_IN, "/SNN/set_input_portion", set_input_portion);
    ofxSubscribeOsc(PORT_IN, "/SNN/set_input_group_size", set_input_group_size);
    ofxSubscribeOsc(PORT_IN, "/SNN/set_output_portion", set_output_portion);        // portion: fraction of global neurons dedicated to output (1/x, where x is the provided number)
    ofxSubscribeOsc(PORT_IN, "/SNN/set_output_group_size", set_output_group_size);  // group size: subdivides the output neurons into x groups, from which the actual (x) output values are derived
    ofxSubscribeOsc(PORT_IN, "/SNN/set_number_of_connections", set_number_of_connections);
    ofxSubscribeOsc(PORT_IN, "/SNN/set_stp_flag", set_stp_flag);
    ofxSubscribeOsc(PORT_IN, "/SNN/set_stdp_flag", set_stdp_flag);
    ofxSubscribeOsc(PORT_IN, "/SNN/set_decay_rate", set_decay_rate);
    ofxSubscribeOsc(PORT_IN, "/SNN/init", [this](){
        initSNN();
    });
    
    ofxSubscribeOsc(PORT_IN, "/SNN_output/output_time_window", output_time_window);
    ofxSubscribeOsc(PORT_IN, "/SNN_output/spiked_scalar", spiked_scalar);
    
    ofxSubscribeOsc(PORT_IN, "/display/fps", fps);
    ofxSubscribeOsc(PORT_IN, "/display/display_size", display_size);
    
    ofxSubscribeOsc(PORT_IN, "/display/display_width", display_width);
    ofxSubscribeOsc(PORT_IN, "/display/display_height", display_height);
    
    ofxSubscribeOsc(PORT_IN, "/display/display_x", display_x);
    ofxSubscribeOsc(PORT_IN, "/display/display_y", display_y);
    
    ofxSubscribeOsc(PORT_IN, "/display/display_intensity", display_intensity);
    
#endif // USE_OSC
    
    ofSleepMillis(2000);
    
    set_number_of_neurons = 100;
    initSNN();
    
}
//--------------------------------------------------------------
void ofApp::update(){
    ofSetFrameRate(fps);
    ofSetWindowShape(display_width, display_height);
    ofSetWindowPosition(display_x, display_y);
    
    //----------
    //update parameters
    //----------
    for(i=0; i < ConstParams::Input_Group_Size; i++){
        spike_net.stimulation(i, stimulation_val[i]); // (input group id, stimlation strength)
    }
    if(whole_stimulation_val != 0){
        spike_net.wholeNetworkStimulation(whole_stimulation_val);
    }
    //----------
    //update processes
    //----------
    
    //UPDATE SNN
    spike_net.update();
    
    //UPDATE SNN OUTPUT
    
    //get firing rates
    for(i=0;i<ConstParams::Output_Group_Size;i++){
        spiked_output[i]= spike_net.getSpikedOutput(i)/(n*output_time_window);
        output_group_value[i] = ofClamp(spiked_output[i]*spiked_scalar, 0, 254);
    }
    if(ofGetFrameNum()%output_time_window == 0){
        spike_net.clearSpikedNeuronId();
    }
    
    osc_spiked_output.clear();
    osc_spiked_output.setAddress("/SNN_output/spiked_output");
    for(i=0;i<ConstParams::Output_Group_Size;i++){  //only one output group here
        osc_spiked_output.addFloatArg(spiked_output[i]);
    }
    osc_sender_msx.sendMessage(osc_spiked_output);
}



//--------------------------------------------------------------
void ofApp::draw(){
    int nbColums = ConstParams::Grid_network_width;
    ofClear(0);
    ofSetColor(255*display_intensity);
    ofDrawBitmapString("OSC IN : "+ofToString(PORT_IN)+" - OUT :"+ofToString(PORT_OUT), nbColums*display_size + 10,15);
    ofDrawBitmapString("Neuron number : "+ofToString(ConstParams::Number_Of_Neurons), nbColums*display_size + 10,30);
    ofDrawBitmapString("frame rate : "+ofToString(ofGetFrameRate(), 2), nbColums*display_size + 10,45);
    ofDrawBitmapString("display offset : "+ofToString(neuron_getV_offset, 2), nbColums*display_size + 10,60);
    ofDrawBitmapString("output time window : "+ofToString(output_time_window), nbColums*display_size + 10,75);
    ofDrawBitmapString("stimulation input : "+ofToString(stimulation_val[0]), nbColums*display_size + 10,90);
    ofDrawBitmapString("spiked output : "+ofToString(spiked_output[0]), nbColums*display_size + 10,105);
    
    for(int i=0; i<ConstParams::Number_Of_Neurons; i++){
        ofSetColor((spike_net.neurons[i].getV()+neuron_getV_offset)*display_intensity);
        ofDrawRectangle(1+display_size*(i%nbColums),1+display_size*int(i/nbColums),display_size-1,display_size-1);
    }
}

//--------------------------------------------------------------
void ofApp::initSNN(){
    ConstParams::Number_Of_Neurons = set_number_of_neurons;
    ConstParams::Inhibitory_Portion = set_inhibitory_portion;
    ConstParams::Number_Of_Inhibitory = ConstParams::Number_Of_Neurons/ConstParams::Inhibitory_Portion;
    ConstParams::Number_Of_Connection = set_number_of_connections;
    
    ConstParams::Network_Type = set_network_type;
    ConstParams::Excitatory_Neuron_Type = set_excitatory_neuron_type;
    ConstParams::Inhibitory_Neuron_Type = set_inhibitory_neuron_type;
    
    ConstParams::Grid_network_width = set_grid_network_width;
    
    ConstParams::Input_Portion  = set_input_portion;
    ConstParams::Output_Portion = set_output_portion;
    ConstParams::Input_Group_Size  = set_input_group_size;
    ConstParams::Output_Group_Size = set_output_group_size;
    ConstParams::Input_Neuron_Size  = ConstParams::Number_Of_Neurons/ConstParams::Input_Portion;
    ConstParams::Output_Neuron_Size = ConstParams::Number_Of_Neurons/ConstParams::Output_Portion;
    ConstParams::Input_Neuron_Per_Group  = ConstParams::Input_Neuron_Size/ConstParams::Input_Group_Size;
    ConstParams::Output_Neuron_Per_Group = ConstParams::Output_Neuron_Size/ConstParams::Output_Group_Size;
    
    spike_net.init();
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
