//
//  neuronCtrl.h
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
#include "Parameters.h"

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


class SpikingNet : private ConstParams{
private:
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
    void setInputNeurons();
    void setOutputNeurons();
    
    inline int* getPartOutputNeuron(int start_index, int size);
    inline bool any(int target, const int *reference, int sizeOfReference);
    inline bool any(int target, std::vector<int> reference);
    
    
    std::vector<int> input_neurons;
    std::vector<int> output_neurons;
    std::vector<int> spiked_neuron_id;
    std::vector<int> spiked_neuron_id_cum;
    std::vector<int> spiked_num_of_output_group;
    
    //STDP
    std::vector<int> stdp_counts;
    std::vector<int> stdp_spiked_time;
    int stdp_tau;
    
    //STP (Science paper)
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
    void checkTask();
    void clearSpikedNeuronId();
    int  getSpikedOutput(int index);
    int  getFrameCount(){return frameCount;};
    std::vector<int> getSpikedIds(){return spiked_neuron_id;};
    double** getWeights(){ return weights; };
    
    std::vector<Izhikevich> neurons;
    double** weights;
    
};
