//
//  SpikingNet.h
//  SpikingNeuronSimulator
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

#pragma once

#include "Izhikevich.h"
#include "SpikingNetParameters.h"

#include <fstream>
#include <time.h>
#include <sys/stat.h>
#include <cmath>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include <random>
#include <algorithm>
#include <memory>
#include <vector>

class SpikingNet : private SpikingNetParameters {
private:
    inline int indexInhibitoryNeuron(int i);
    inline int indexExcitatoryNeuron(int i);
    inline int indexInputNeuron(int i);
    inline int indexOutputNeuron(int i);
    
    inline void update_neuron();
    inline void update_input();
    inline void update_input_debug();
    inline void checkFiring();
    inline void stdp();
    inline void stp();
    inline double getStpValue(int index, int is_fired);
    inline void decay();

    void setRandomNetwork();
    void setSparseGraph();
    void setSparseNetwork(bool is_directly_connected);
    void setUniformNetwork();
    void setChainNetwork();
    void setGridNetwork();
    
    std::vector<float> output_spiking;
    
    // STDP
    std::vector<int> stdp_counts;
    int stdp_tau;
    
    // STP
    double* stp_u;
    double* stp_x;
    double* stp_wf;
    
    int frameCount;
    
    std::mt19937 rand_gen;
    
    
public:
    SpikingNet();
    ~SpikingNet();
    
    void init();
    void update();
    void stimulation();
    void stimulation(double stim_strength_);
    void stimulation(int group_id_, double stim_strength_);
    void wholeStimulation();
    void wholeStimulation(double stim_strength_);
    void wholeNetworkStimulation();
    void wholeNetworkStimulation(double stim_strength_);
    int  getSpikedOutput(int index);
    int  getFrameCount(){return frameCount;};
    double** getWeights(){ return weights; };
    
    std::vector<Izhikevich> neurons;
    double** weights;
    
};
