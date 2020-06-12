//
//  SpikingNet.cpp
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

#include "SpikingNet.h"
#include <iomanip>

SpikingNet::SpikingNet(){

    stdp_tau = 20;

    //set random seed
    if(SpikingNetParameters::Random_Device_Flag)
    {
        std::random_device rnd;
        rand_gen.seed(rnd());
    }
    else
    {
        rand_gen.seed(SpikingNetParameters::Random_Seed);
    }


    neurons.resize(SpikingNetParameters::Neuron_Size);

    output_spiking.resize(SpikingNetParameters::Output_Group_Size, 0);
    
    stdp_counts.resize(SpikingNetParameters::Neuron_Size, 0);

    //STP (according to Science paper)
    stp_u = new double[Neuron_Size];
    stp_x = new double[Neuron_Size];
    stp_wf = new double[Neuron_Size];

    weights = new double*[Neuron_Size];
    for(int i = 0; i < Neuron_Size; ++i){
        weights[i] = new double[Neuron_Size];
    }

}

SpikingNet::~SpikingNet(){

    // delete STP variables
    delete[] stp_u;
    delete[] stp_x;
    delete[] stp_wf;
    stp_u = 0;
    stp_x = 0;
    stp_wf = 0;

    for(int i = 0; i < SpikingNetParameters::Neuron_Size; ++i){
        delete[] weights[i];
        weights[i] = 0;
    }
    delete[] weights;
    weights = 0;
}


void SpikingNet::init(){

    for(int i = 0; i < SpikingNetParameters::Neuron_Size; ++i){
        for(int j = 0; j < SpikingNetParameters::Neuron_Size; ++j){
            weights[i][j] = 0.;
        }
    }

    neurons.clear();
    stdp_counts.clear();

    neurons.resize(SpikingNetParameters::Neuron_Size);
    
    output_spiking.resize(SpikingNetParameters::Output_Group_Size, 0);

    stdp_counts.resize(SpikingNetParameters::Neuron_Size, 0);

    //initialization of some variables
    frameCount = 0;

    for(int i = 0; i < Neuron_Size; ++i){

        stdp_counts[i] = 0;

        // initialize STP variables
        stp_wf[i] = 1.0;
        stp_x[i] = 1.0;
        stp_u[i] = 0.0;
    }

    for(int i = 0; i < Neuron_Size; ++i){
        if(i < Inhibitory_Size){
            neurons[i].setNeuronType(SpikingNetParameters::Inhibitory_Neuron_Type);
        }else{
            neurons[i].setNeuronType(SpikingNetParameters::Excitatory_Neuron_Type);
//            if(rand()%100 == 0) neurons[i].setNeuronType(5);
        }
    }
    
    // set network
    switch(Network_Type){

        case Sparse_Network:
            setSparseNetwork(SpikingNetParameters::Direct_Connection_Flag);
            break;

        case Random_Network:
            setRandomNetwork();
            break;
        case Uniform_Network:// it is for debugging.
            setUniformNetwork();
            break;
        case Grid_network:
            setGridNetwork();
            break;
        case 6:// it is for debugging.
            setGridNetwork();
            break;
    }
}

// inhibitory neurons are first, the rest is excitatory neurons
// input and output neurons are always in the excitatory neurons

// ------ inhibitory ------ | --------------------------- excitatory ------------------------------ |
// ------ unassigned ------ | ------ input ------ | ------ output ------ | ------ unassigned ------ |

int SpikingNet::indexInhibitoryNeuron(int i) {
    // expects an index [0, SpikingNetParameters::Inhibitory_Size - 1]
    return i;
}

int SpikingNet::indexExcitatoryNeuron(int i) {
    // expects an index [0, SpikingNetParameters::Neuron_Size - SpikingNetParameters::Inhibitory_Size - 1]
    return i + SpikingNetParameters::Inhibitory_Size;
}

int SpikingNet::indexInputNeuron(int i) {
    // expects an index [0, SpikingNetParameters::Input_Size - 1]
    return i + SpikingNetParameters::Inhibitory_Size;
}

int SpikingNet::indexOutputNeuron(int i) {
    // expects an index [0, SpikingNetParameters::Output_Size - 1]
    return i + SpikingNetParameters::Inhibitory_Size + SpikingNetParameters::Input_Size;
}

void SpikingNet::setSparseNetwork(bool is_directly_connected){

    std::uniform_int_distribution<> rand_uni_neurons(0, Neuron_Size-1);
    std::uniform_real_distribution<> rand_uni(0.0, 1.0);

    int sum_connection = 0;
    int* dest_array;

    // in case of non fully connected
    if(SpikingNetParameters::Connections_Per_Neuron != SpikingNetParameters::Neuron_Size)
    {
        for(int src_id = 0; src_id < Neuron_Size; ++src_id){

            dest_array = new int[SpikingNetParameters::Connections_Per_Neuron];
            for(int n = 0; n < SpikingNetParameters::Connections_Per_Neuron; n++){
                // initialize to something that won't be matched when we check if the candidate is already used
                dest_array[n] = -1;
            }

            int count = 0;
            while(count < SpikingNetParameters::Connections_Per_Neuron){

                int candidate_id = rand_uni_neurons(rand_gen);
                // check wether the candidate_id is already included in dest_array.
                bool is_included = false;
                for(int i = 0; i < SpikingNetParameters::Connections_Per_Neuron; i++) {
                    if(dest_array[i] == candidate_id) {
                        is_included = true;
                        break;
                    }
                }

                if(!is_included){
                    dest_array[count] = candidate_id;
                    count++;
                    sum_connection++;
                }
            }

            for(int j = 0; j < SpikingNetParameters::Connections_Per_Neuron; j++){

                int dest_id = dest_array[j];
                if(dest_id != src_id){

                    if(dest_id >= 0 && dest_id < SpikingNetParameters::Neuron_Size){
                        if(src_id < SpikingNetParameters::Inhibitory_Size) {
                            weights[src_id][dest_id] = Init_Weight_Ratio_Inh*rand_uni(rand_gen);
                        }else{
                            weights[src_id][dest_id] = Init_Weight_Ratio_Exc*rand_uni(rand_gen);
                        }
                    }
                }
            }

            delete[] dest_array;
            dest_array = 0;
        }
    }
    // in case of fully connected
    else
    {
        for(int i = 0; i < Neuron_Size; ++i){
            for(int j = 0; j < Neuron_Size; j++){

                if(i != j){

                    if(i < SpikingNetParameters::Inhibitory_Size) {
                        weights[i][j] = Init_Weight_Ratio_Inh * rand_uni(rand_gen);

                    } else {
                        weights[i][j] = Init_Weight_Ratio_Exc * rand_uni(rand_gen);
                    }
                    sum_connection++;
                }
            }
        }

    }

    // TODO: this kind of sucks, make it so that this doesn't happen in the first place?
    // delete direct connection between input and output
    if(is_directly_connected == false){
        for(int src_id = 0; src_id < SpikingNetParameters::Input_Size; src_id++){
            for(int dest_id = 0; dest_id < SpikingNetParameters::Output_Size; dest_id++){
                weights[indexInputNeuron(src_id)][indexOutputNeuron(dest_id)] = 0.0;
            }
        }
    }

    printf("sum of connection: %d\n", sum_connection);
}

// fully connected with same weight : This is for debug.
void SpikingNet::setUniformNetwork(){

    int sum_connection = 0;

    for(int i = 0; i < Neuron_Size; ++i){
        for(int j = 0; j < Neuron_Size; j++){

            if(i != j){

                if(i < Inhibitory_Size) {
                    weights[i][j] = Init_Weight_Ratio_Inh;

                } else {
                    weights[i][j] = Init_Weight_Ratio_Exc;
                    if(j % 3 == 0) weights[i][j] = Init_Weight_Ratio_Exc * 1.50;

                }
                sum_connection++;
            }
        }
    }
    printf("sum of connection: %d\n", sum_connection);
}

// fully connected network with random weight
void SpikingNet::setRandomNetwork(){

    std::uniform_int_distribution<> rand_uni_neurons(0, Neuron_Size-1);
    std::uniform_real_distribution<> rand_uni(0.0, 1.0);

    int sum_connection = 0;

    for(int i = 0; i < Neuron_Size; ++i){
        for(int j = 0; j < SpikingNetParameters::Neuron_Size; j++){

            if(i != j){
                if(i < Inhibitory_Size) {
                    weights[i][j] = Init_Weight_Ratio_Inh * rand_uni(rand_gen);

                } else {
                    weights[i][j] = Init_Weight_Ratio_Exc * rand_uni(rand_gen);
                }
                sum_connection++;
            }
        }
    }
    printf("sum of connection: %d\n", sum_connection);

}

// fully connected with same weight : This is for debug.
void SpikingNet::setGridNetwork(){

    float connection_rate_ = 0.01;

    std::uniform_real_distribution<> rand_uni(0.0, 1.0);

    for(int i = 0; i < Neuron_Size; ++i) {
        int row = i / SpikingNetParameters::Grid_Network_Width;
        int col = i % SpikingNetParameters::Grid_Network_Width;
        for(int j = 0; j < Neuron_Size; j++){
            int row_target = j / SpikingNetParameters::Grid_Network_Width;
            int col_target = j % SpikingNetParameters::Grid_Network_Width;
            if(i < Inhibitory_Size) {
                if(i != j && rand_uni(rand_gen) < connection_rate_) {
                    weights[i][j] = Init_Weight_Ratio_Inh * rand_uni(rand_gen); //HERE!!!
                } else {
                    weights[i][j] = 0;
                }
            } else if (j < Inhibitory_Size) {
                if(i != j && rand_uni(rand_gen) < connection_rate_) {
                    weights[i][j] = Init_Weight_Ratio_Exc * rand_uni(rand_gen); //HERE!!!
                } else {
                    weights[i][j] = 0;
                }
            } else if (i != j && (abs(row - row_target) <= 1 && (abs(col - col_target) <= 1  || abs(col - col_target) == 8))) {
                weights[i][j] = Init_Weight_Ratio_Exc * rand_uni(rand_gen);  // HERE!!!
            } else {
                weights[i][j] = 0;
            }
        }
    }

//    std::uniform_real_distribution<> rand_uni(0.0, 1.0);
//    float connection_rate_ = 0.01;
//
//    for(int i=0; i<Number_Of_Neurons; ++i) {
//        int row = i / 20;
//        int col = i % 20;
//
//        for(int j=0; j<Number_Of_Neurons; j++){
//            int row_target = j / 20;
//            int col_target = j % 20;
//            if(i < Number_Of_Inhibitory) {
//                if(i != j && rand_uni(rand_gen) < connection_rate_) {
//                    weights[i][j] = Init_Weight_Ratio_Inh * rand_uni(rand_gen); //HERE!!!
//                }
//            } else if (j < Number_Of_Inhibitory) {
//                if(i != j && rand_uni(rand_gen) < connection_rate_) {
//                    weights[i][j] = Init_Weight_Ratio_Exc * rand_uni(rand_gen); //HERE!!!
//                }
//            } else if ( i != j && (abs(row - row_target) <= 1 || abs(col - col_target) <= 1  || abs(row - row_target) == 19 || abs(col - col_target) == 19) ) {
//                weights[i][j] = Init_Weight_Ratio_Exc*rand_uni(rand_gen);  // HERE!!!
//            } else {
//                weights[i][j] = 0;
//            }
//        }
//    }

//    std::uniform_real_distribution<> rand_uni(0.0, 1.0);
//
//    for(int i=0; i<Number_Of_Neurons; ++i) {
//        int row = i / 20;
//        int col = i % 20;
//
//        for(int j=0; j<Number_Of_Neurons; j++){
//            if (i==j) {
//                weights[i][j] = 0;
//            }
//
//            int row_target = j / 20;
//            int col_target = j % 20;
//            if(i < Number_Of_Inhibitory && j >= 300 && j < 800) {
//                if(rand()%50==0){
//                    weights[i][j] = Init_Weight_Ratio_Inh*rand_uni(rand_gen);
//                }
//            } else if (j < Number_Of_Inhibitory && i >= 300 && i < 800) {
//                if(rand()%50==0){
//                    weights[i][j] = Init_Weight_Ratio_Exc*rand_uni(rand_gen);
//                }
//            } else if ( i != j && (abs(row - row_target) <= 1 || abs(col - col_target) <= 1) ) {
//                weights[i][j] = Init_Weight_Ratio_Exc*rand_uni(rand_gen);  // HERE!!!
//            } else {
//                weights[i][j] = 0;
//            }
//        }
//    }

//    for(int i=0; i<Number_Of_Neurons; ++i) {
//        int row = i / 20;
//        int col = i % 20;
//
//        for(int j=0; j<Number_Of_Neurons; j++){
//            int row_target = j / 20;
//            int col_target = j % 20;
//            if(i != j && ( (row + 1 == row_target) || (row - 1 == row_target) || (col + 1 == col_target) || (col - 1 == col_target) || (col == 0 && col_target == 19) || (col_target == 19 && col == 0) ) ) {
//                //weights[i][j] = Init_Weight_Ratio_Inh*rand_uni(rand_gen);
//                if(i < Number_Of_Inhibitory) {
//                    weights[i][j] = Init_Weight_Ratio_Inh*drand48();;
//                } else {
//                    weights[i][j] = Init_Weight_Ratio_Exc*drand48();
//                }
//            } else {
//                weights[i][j] = 0.0;
//            }
//        }
//    }
//    int sum_connection = 0;
//
//    for(int i=0; i<Number_Of_Neurons; ++i){
//        for(int j=0; j<Number_Of_Neurons; j++){
//
//            if(i != j){
//
//                if(i < Number_Of_Inhibitory) {
//                    weights[i][j] = Init_Weight_Ratio_Inh;
//
//                } else {
//                    weights[i][j] = Init_Weight_Ratio_Exc;
//                    if(j%3==0) weights[i][j] = Init_Weight_Ratio_Exc*1.50;
//
//                }
//                sum_connection++;
//            }
//        }
//    }
//    printf("sum of connection: %d\n", sum_connection);
}

void SpikingNet::checkFiring(){
    // reset spiking output
    for(int i = 0; i < SpikingNetParameters::Output_Group_Size; i++) {
        output_spiking[i] = 0;
    }
    int sizePerGroup = (SpikingNetParameters::Output_Size / SpikingNetParameters::Output_Group_Size);
    for(int i = 0; i < SpikingNetParameters::Output_Size; i++) {
        // get the index of the output neuron
        int index = indexOutputNeuron(i);
        // check to see if the neuron is firing
        if(neurons[index].checkFiring()) {
            // get the index of the group this neuron belongs to. min is there to prevent an edge case where the number of group doesn't perfectly divide the number of output neurons.
            int indexGroup = std::min<int>(i / sizePerGroup, SpikingNetParameters::Output_Group_Size - 1);
            // apply increment with averaging
            output_spiking[indexGroup] += 1.0 / sizePerGroup;
        }
    }
}


// the thing that produces the actual output
int SpikingNet::getSpikedOutput(int index){
    return output_spiking[index];
}


void SpikingNet::update_input(){

    //setup normal distribution random function
    std::normal_distribution<> normalRand(0.0, 1.0);

    //pseudo thalamus noise-input
    for(int i = 0; i < Neuron_Size; ++i){

        if(i < Inhibitory_Size ){
            neurons[i].addToI(Noise_Ratio_Inh * normalRand(rand_gen));
        }else{
            neurons[i].addToI(Noise_Ratio_Exc * normalRand(rand_gen));
        }
    }

    //input from connected neurons with STP
    for(int i = 0; i < Neuron_Size; ++i){
        if(neurons[i].isFiring()){
            for(int j = 0; j < Neuron_Size; ++j){

                if(i != j){
                    if(i>Inhibitory_Size)
                    neurons[j].addToI((float)weights[i][j] * (float)stp_wf[i]);
                    else
                    neurons[j].addToI((float)weights[i][j]);
                }
            }
        }
    }

}

void SpikingNet::update_neuron(){

    // update differential equation
    for(int i = 0; i < Neuron_Size; ++i){
        neurons[i].update();
        neurons[i].setI(0.0);
    }

}


void SpikingNet::stdp(){

    // read http://www.scholarpedia.org/article/Spike-timing_dependent_plasticity for more info on the maths behind this
    
    // essentially, this is a technique for dynamically changing the strength of connections according to the the firing patterns of connected neurons.
    // spike timing dependent plasticity works by strengthening connections that "predict" spiking patterns, and in a way "speed up" the propagation of spikes, in the sense that if a neuron A has a tendency to fire just before another neuron B, the connection from A to B will become stronger. inversely, the connection from B to A will become weaker.
    
    // the way the connection strength changes is decided by a function made up of two exponentials. it looks a bit like this.
    
    //                            . +
    //                           .  |
    //                        ...   |
    //  connection       .....      |
    //  strength     - <------------|------------> +
    //  change                      |      .....
    //  A -> B                      |   ...
    //                              |  .
    //                              - .
    //
    //                       timing difference
    //
    //                A before B        B before A
    //
    
    // in this code, stdp_counts[i] stores a number that allows calculating how many frames ago the neuron i fired.
    // when stdp_counts[i] = stdp_tau (in this case it is hard-coded to 20), the neuron fired in the current frame.
    // at every subsequent frame it is decremented.
    // the way we compute how many frames ago the neuron fired is by evaluating stdp_tau - stdp_counts[i]
    
    // TODO: this is not framerate invariant, as it measures timings by counting frames. this needs to be fixed.
    
    for(int i = Inhibitory_Size; i < Neuron_Size; ++i){
        // decrease the stdp_count for all neurons. this keeps track of how far away in time the neuron has fired.
        // when it reaches 0, it won't be considered when changing weights according to spike time changes.
        if(stdp_counts[i] > 0) stdp_counts[i] = stdp_counts[i] - 1;
        
        // if the neuron is currently firing, set its stdp_count to stdp_tau.
        if(neurons[i].isFiring()) stdp_counts[i] = stdp_tau;
    }

    double d;
    for(int i = Inhibitory_Size; i < Neuron_Size; i++){

        if(neurons[i].isFiring()){

            for(int j = Inhibitory_Size; j < Neuron_Size; j++){

                if(stdp_counts[j] > 0 && stdp_counts[j] != stdp_tau && i != j){
                    // another (uniquely different) neuron has fired in the last stdp_tau frames (excluding the current frame)
                    //
                    
                    // this is part of the exponential function described above
                    d = (0.1 * pow(0.95, (stdp_tau-stdp_counts[j])));

                    // check that neuron linked to i fired less than tau_ms before (but is not firing right now)
                    // if j fired before i, then weight j->i ++
                    if(weights[j][i] != 0.0){
                        weights[j][i] += d;
                        if (weights[j][i] > Weight_Max) weights[j][i] = Weight_Max;

                    }

                    // now weight from i to j, should be lowered if i fired before j. careful! only the indexing changes.
                    // if j fired after i, then weight j->i --
                    if(weights[i][j] != 0.0){
                        weights[i][j] -= d;
                        if(weights[i][j] < Weight_Min) weights[i][j] = Weight_Min;

                    }
                }
            }
        }
    }
}

void SpikingNet::stp(){

    for(int i = Inhibitory_Size; i < Neuron_Size; ++i){

        if(neurons[i].isFiring()){
            stp_wf[i] = getStpValue(i,1);
        }else{
            stp_wf[i] = getStpValue(i,0);
        }
    }
}

double SpikingNet::getStpValue(int index, int is_firing){
    // see http://www.rmki.kfki.hu/~banmi/elte/synaptic.pdf for paper describing the mechanics of this
    
    double U = 0.2; // mV. baseline level for u

    double s = double(is_firing); // s indicates whether the neuron is currently firing or not (0 or 1)
    double u = stp_u[index];    // u represents the amount of resources used by the creation of a spike.
                                // normalized (0, U), baseline is U
    double x = stp_x[index];    // x represents the amount of resources avaliable
                                // normalized (0, 1), baseline is 1
    
    // when a spike is created, an amount u * x of resources is used. this decreases x and increases u
    // both u and v recover to their baseline levels exponentially.

    double tau_d = 200.; // ms. defines the time constant for the recovery of x (resources)
    double tau_f = 600.; // ms. defines the time constant for the recovery of u (usage)
    
    // when tau_d > tau_f, the neural activity is depressed
    // when tau_d < tau_f, the neural activity is facilitated
    
    double dx = (1.0 - x) / tau_d - u * x * s;       // change for x
    double du = (U - u) / tau_f + U * (1.0 - u) * s; // change for u

    double nu = u + du; // new value for u
    double nx = x + dx; // new value for x
    
    double wf = nu * nx; // final value returned. modulates synaptic efficacy
    
    // update stored values
    stp_u[index] = nu;
    stp_x[index] = nx;

    return wf;
}

void SpikingNet::decay(){

    for(int i = Inhibitory_Size; i < Neuron_Size; i++){
        for(int j = Inhibitory_Size; j < Neuron_Size; j++){
            weights[i][j] = weights[i][j] * SpikingNetParameters::Decay_Rate;
        }
    }

}

void SpikingNet::update(){

    checkFiring();
    if(SpikingNetParameters::Decay_Flag) decay();
    if(SpikingNetParameters::Stdp_Flag)  stdp();
    if(SpikingNetParameters::Stp_Flag)   stp();

    update_input();
    update_neuron();

    frameCount++;
}

void SpikingNet::stimulation(){
    // external input to input neurons.
    for(int i = 0; i < SpikingNetParameters::Input_Size; ++i){
        neurons[indexInputNeuron(i)].addToI(SpikingNetParameters::Stim_Strength);
    }
}

void SpikingNet::stimulation(double stim_strength_){
    // external input to input neurons.
    for(int i = 0; i < SpikingNetParameters::Input_Size; ++i){
        neurons[indexInputNeuron(i)].addToI(stim_strength_);
    }
}

void SpikingNet::stimulation(int group_id_, double stim_strength_){

    int group_size = floor((float)SpikingNetParameters::Input_Size / (float)SpikingNetParameters::Input_Group_Size);

    // external input to input neurons.
    for(int i = 0; i < SpikingNetParameters::Input_Group_Size; ++i){
        for(int j = (i * group_size); j<((i + 1) * group_size); ++j){
            if(i == group_id_){
                neurons[indexInputNeuron(j)].addToI(stim_strength_);
            }else if(i > group_id_){
                break;
            }
        }
    }
}

void SpikingNet::wholeStimulation(){
    //    external input
    for(int i = 0; i < SpikingNetParameters::Input_Size; ++i){
        neurons[indexInputNeuron(i)].addToI(SpikingNetParameters::Stim_Strength);
    }
}

void SpikingNet::wholeStimulation(double stim_strengh_){
    //    external input
    for(int i = 0; i < SpikingNetParameters::Input_Size; i++){
        neurons[indexInputNeuron(i)].addToI(stim_strengh_);
    }
}

void SpikingNet::wholeNetworkStimulation(){
    //    external input
    for(int i = 0; i < SpikingNetParameters::Neuron_Size; ++i){
        neurons[i].addToI(SpikingNetParameters::Stim_Strength);
    }
}

void SpikingNet::wholeNetworkStimulation(double stim_strengh_){
    //    external input
    for(int i = 0; i < SpikingNetParameters::Neuron_Size; ++i){
            neurons[i].addToI(stim_strengh_);
    }
}
