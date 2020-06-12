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

#pragma once

#define USE_OSC

#define SNN_ID 1


#include <iomanip>
#include <fstream>
#include <time.h>
#include <sys/stat.h>
#include <math.h>
#include <string>
#include <cmath>

#include "ofMain.h"
#include "ofMath.h"

#include "ofxGui.h"
#include "ofxOsc.h"
#ifdef USE_OSC
#include "ofxPubSubOsc.h"
#endif

#include "SpikingNetParameters.h"
#include "SpikingNet.h"
#include "SpikingNetWriter.h"

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

    ofxOscSender osc_sender_msx;
    ofxOscMessage osc_spiked_output;

    void displayOutputGroup(int group);

    // Return neuron in position (x, y) on the display.
    Izhikevich& getNeuron(int x, int y);
    double getNeuronV(int x, int y);
private:

    void initSNN();

    int i, j, k;

    SpikingNet spike_net;
    SpikingNetWriter writer_weight, writer_spike;

    //raw snn variables
    int set_number_of_neurons = SpikingNetParameters::Neuron_Size;
    int set_inhibitory_portion = SpikingNetParameters::Inhibitory_Portion;
    int set_number_of_connections = SpikingNetParameters::Connections_Per_Neuron;

    int set_network_type = SpikingNetParameters::Network_Type;
    int set_excitatory_neuron_type = SpikingNetParameters::Excitatory_Neuron_Type;
    int set_inhibitory_neuron_type = SpikingNetParameters::Inhibitory_Neuron_Type;
    
    int set_grid_network_width = SpikingNetParameters::Grid_Network_Width;

    int set_input_portion = SpikingNetParameters::Input_Portion;
    int set_input_group_size = SpikingNetParameters::Input_Group_Size;

    int set_output_portion = SpikingNetParameters::Output_Portion;
    int set_output_group_size = SpikingNetParameters::Output_Group_Size;

    bool set_stp_flag = SpikingNetParameters::Stp_Flag;
    bool set_stdp_flag = SpikingNetParameters::Stdp_Flag;
    double set_decay_rate = SpikingNetParameters::Decay_Rate;

    int stimulation_val[10]; //dimension is SpikingNetParameters::Input_Group_Size
    int neuron_getV_offset = 0;
    int whole_stimulation_val = 0;

    //output snn variables
    int output_time_window = 10;
    float output_group_value[10];
    float spiked_output[10];
    float spiked_scalar = 400;
    float n = SpikingNetParameters::Output_Size / SpikingNetParameters::Output_Group_Size;

    //display variables
    int display_size = 10;
    int fps = 30;
    int display_width = 420;
    int display_height = 110;
    int display_x = 0;
    int display_y = 0;
    float display_intensity = 1;
    

};
