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
    if(ConstParams::Random_Device_Flag)
    {
        std::random_device rnd;
        rand_gen.seed(rnd());
    }
    else
    {
        rand_gen.seed(ConstParams::Random_Seed);
    }


    neurons.resize(ConstParams::Number_Of_Neurons);
    input_neurons.resize(ConstParams::Input_Neuron_Size, 0);
    output_neurons.resize(ConstParams::Output_Neuron_Size, 0);
    spiked_num_of_output_group.resize(ConstParams::Output_Group_Size, 0);

    stdp_spiked_time.resize(ConstParams::Number_Of_Neurons, 0);
    stdp_counts.resize(ConstParams::Number_Of_Neurons, 0);

    //STP (according to Science paper)
    stp_u = new double[Number_Of_Neurons];
    stp_x = new double[Number_Of_Neurons];
    stp_wf = new double[Number_Of_Neurons];

    weights = new double*[Number_Of_Neurons];
    for(int i=0; i<Number_Of_Neurons; ++i){
        weights[i] = new double[Number_Of_Neurons];
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

    for(int i=0; i<ConstParams::Number_Of_Neurons; ++i){
        delete[] weights[i];
        weights[i] = 0;
    }
    delete[] weights;
    weights = 0;
}


void SpikingNet::init(){

    for(int i=0; i<ConstParams::Number_Of_Neurons; ++i){
        for(int j=0; j<ConstParams::Number_Of_Neurons; ++j){
            weights[i][j] = 0.;
        }
    }

    neurons.clear();
    input_neurons.clear();
    output_neurons.clear();
    spiked_num_of_output_group.clear();
    stdp_spiked_time.clear();
    stdp_counts.clear();

    neurons.resize(ConstParams::Number_Of_Neurons);
    input_neurons.resize(ConstParams::Input_Neuron_Size, 0);
    output_neurons.resize(ConstParams::Output_Neuron_Size, 0);
    spiked_num_of_output_group.resize(ConstParams::Output_Group_Size, 0);

    stdp_spiked_time.resize(ConstParams::Number_Of_Neurons, 0);
    stdp_counts.resize(ConstParams::Number_Of_Neurons, 0);

    //initialization of some variables
    frameCount = 0;

    for(int i=0; i<Number_Of_Neurons; ++i){

        stdp_spiked_time[i] = 0;
        stdp_counts[i] = 0;

        // initialize STP variables
        stp_wf[i] = 1.0;
        stp_x[i] = 1.0;
        stp_u[i] = 0.0;
    }

    for(int i=0; i<Number_Of_Neurons; ++i){
        if(i < Number_Of_Inhibitory){
            neurons[i].setNeuronType(ConstParams::Inhibitory_Neuron_Type);
        }else{
            neurons[i].setNeuronType(ConstParams::Excitatory_Neuron_Type);
//            if(rand()%100 == 0) neurons[i].setNeuronType(5);
        }
    }

    setInputNeurons();
    setOutputNeurons();

    // set network
    switch(Network_Type){

        case Sparse_Network:
            setSparseNetwork(ConstParams::Direct_Connection_Flag);
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

void SpikingNet::setInputNeurons(){
    std::cout << "Input neurons : ";

    /*
    for(int i=0; i<ConstParams::Input_Group_Size; i++){
        std::cout << " new input group : " << std::endl;
        for(int j=0; j<ConstParams::Input_Neuron_Per_Group; ++j){
            input_neurons[i* ConstParams::Input_Neuron_Per_Group + j] = Number_Of_Inhibitory + i*36*9 + (36*9/2 - ConstParams::Input_Neuron_Per_Group/2) + j; //each box (36*9neurons) starts with an input group

            std::cout << input_neurons[i* ConstParams::Input_Neuron_Per_Group + j] << ",";
        }
        std::cout << std::endl;
    }
     */

    
    for(int i=0; i<ConstParams::Input_Neuron_Size; ++i){
        input_neurons[i] = Number_Of_Inhibitory + i;
        std::cout << input_neurons[i] << ",";
    }
    

    std::cout << std::endl;

}

void SpikingNet::setOutputNeurons(){

    std::cout << "Output neurons : ";

    /*
    for(int i=0; i<ConstParams::Output_Group_Size; i++){
        std::cout << " new output group : " << std::endl;
        for(int j=0; j<ConstParams::Output_Neuron_Per_Group; ++j){
            output_neurons[i* ConstParams::Output_Neuron_Per_Group + j] = i*36*9 + j; //each box (36*9neurons) starts with an input group & and the portion of inhibitory at the top of the network has to be taken into account to have a propoer positioning//each box (36*9neurons) has an output group in the middle
            std::cout << output_neurons[i*ConstParams::Output_Neuron_Per_Group+j] << ",";
        }
        std::cout << std::endl;
    }
     */

    
    for(int i=0; i<Output_Neuron_Size; ++i){
        output_neurons[i] = Number_Of_Inhibitory + Input_Neuron_Size + i;
        std::cout << output_neurons[i] << ",";
    }
    

    std::cout << std::endl;
}


void SpikingNet::setSparseNetwork(bool is_directly_connected){

    std::uniform_int_distribution<> rand_uni_neurons(0, Number_Of_Neurons-1);
    std::uniform_real_distribution<> rand_uni(0.0, 1.0);

    int sum_connection = 0;
    int* dest_array;

    // in case of non fully connected
    if(ConstParams::Number_Of_Connection != ConstParams::Number_Of_Neurons)
    {
        for(int src_id=0; src_id<Number_Of_Neurons; ++src_id){

            dest_array = new int[ConstParams::Number_Of_Connection];
            for(int n=0; n<ConstParams::Number_Of_Connection; n++){
                dest_array[n] = 0;
            }

            int count = 0;
            while(count < ConstParams::Number_Of_Connection){

                int candidate_id = rand_uni_neurons(rand_gen);
                // check wether the candidate_id is already included in dest_array.
                bool is_included = any(candidate_id, dest_array, count);

                if(!is_included){
                    dest_array[count] = candidate_id;
                    count++;
                    sum_connection++;
                }
            }

            for(int j=0; j<ConstParams::Number_Of_Connection; j++){

                int dest_id = dest_array[j];
                if(dest_id != src_id){

                    if(dest_id >=0 && dest_id < ConstParams::Number_Of_Neurons){
                        if(src_id < ConstParams::Number_Of_Inhibitory) {
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
        for(int i=0; i<Number_Of_Neurons; ++i){
            for(int j=0; j<Number_Of_Neurons; j++){

                if(i != j){

                    if(i < ConstParams::Number_Of_Inhibitory) {
                        weights[i][j] = Init_Weight_Ratio_Inh*rand_uni(rand_gen);

                    } else {
                        weights[i][j] = Init_Weight_Ratio_Exc*rand_uni(rand_gen);
                    }
                    sum_connection++;
                }
            }
        }

    }

    // delete direct connection between input and output
    if(is_directly_connected == false){
        for(int src_id=0; src_id<ConstParams::Input_Neuron_Size; src_id++){
            for(int dest_id=0; dest_id<ConstParams::Output_Neuron_Size; dest_id++){
                weights[input_neurons[src_id]][output_neurons[dest_id]] = 0.0;
            }
        }
    }

    printf("sum of connection: %d\n", sum_connection);
}

// fully connected with same weight : This is for debug.
void SpikingNet::setUniformNetwork(){

    int sum_connection = 0;

    for(int i=0; i<Number_Of_Neurons; ++i){
        for(int j=0; j<Number_Of_Neurons; j++){

            if(i != j){

                if(i < Number_Of_Inhibitory) {
                    weights[i][j] = Init_Weight_Ratio_Inh;

                } else {
                    weights[i][j] = Init_Weight_Ratio_Exc;
                    if(j%3==0) weights[i][j] = Init_Weight_Ratio_Exc*1.50;

                }
                sum_connection++;
            }
        }
    }
    printf("sum of connection: %d\n", sum_connection);
}

// fully connected network with random weight
void SpikingNet::setRandomNetwork(){

    std::uniform_int_distribution<> rand_uni_neurons(0, Number_Of_Neurons-1);
    std::uniform_real_distribution<> rand_uni(0.0, 1.0);

    int sum_connection = 0;

    for(int i=0; i<Number_Of_Neurons; ++i){
        for(int j=0; j<ConstParams::Number_Of_Neurons; j++){

            if(i != j){
                if(i < Number_Of_Inhibitory) {
                    weights[i][j] = Init_Weight_Ratio_Inh*rand_uni(rand_gen);

                } else {
                    weights[i][j] = Init_Weight_Ratio_Exc*rand_uni(rand_gen);
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

    for(int i=0; i<Number_Of_Neurons; ++i) {
        int row = i / ConstParams::Grid_network_width;
        int col = i % ConstParams::Grid_network_width;
        for(int j=0; j<Number_Of_Neurons; j++){
            int row_target = j / ConstParams::Grid_network_width;
            int col_target = j % ConstParams::Grid_network_width;
            if(i < Number_Of_Inhibitory) {
                if(i != j && rand_uni(rand_gen) < connection_rate_) {
                    weights[i][j] = Init_Weight_Ratio_Inh * rand_uni(rand_gen); //HERE!!!
                } else {
                    weights[i][j] = 0;
                }
            } else if (j < Number_Of_Inhibitory) {
                if(i != j && rand_uni(rand_gen) < connection_rate_) {
                    weights[i][j] = Init_Weight_Ratio_Exc * rand_uni(rand_gen); //HERE!!!
                } else {
                    weights[i][j] = 0;
                }
            } else if ( i != j && (abs(row - row_target) <= 1 && (abs(col - col_target) <= 1  || abs(col - col_target) == 8)) ) {
                weights[i][j] = Init_Weight_Ratio_Exc*rand_uni(rand_gen);  // HERE!!!
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



int* SpikingNet::getPartOutputNeuron(int start_index, int size){

    int* part_output = new int[size];
    for(int i=0; i<size; ++i){
        part_output[i] = output_neurons[i+start_index];
    }

    return part_output;
}

void SpikingNet::checkFiring(){

    spiked_neuron_id.clear();

    for(int i=0; i<Number_Of_Neurons; ++i){

        if(neurons[i].checkFiring()) {
            spiked_neuron_id.push_back(i);
            spiked_neuron_id_cum.push_back(i);
        }
    }
}


void SpikingNet::clearSpikedNeuronId(){
    spiked_num_of_output_group.clear();
    spiked_num_of_output_group.resize(ConstParams::Output_Group_Size, 0);
    spiked_neuron_id_cum.clear();
}

void SpikingNet::checkTask(){
    // TODO: it seems like this code (along with anything referencing spiked_neuron_id and variants) is intended to record spike timings and save them to a file. these operations are needlessly expensive and unnecessary for our use case. remove this from the code and implement a more efficient, simple way of computing the output of groups

    // Remove overlaps
    std::sort(spiked_neuron_id_cum.begin(), spiked_neuron_id_cum.end());
    spiked_neuron_id_cum.erase(std::unique(spiked_neuron_id_cum.begin(), spiked_neuron_id_cum.end()), spiked_neuron_id_cum.end() );


    int group_size = floor((float)ConstParams::Output_Neuron_Size / (float)ConstParams::Output_Group_Size);

    for(unsigned int i=0; i<spiked_neuron_id_cum.size(); i++){

        for(int j=0; j<ConstParams::Output_Group_Size; j++){

            int* output_group = getPartOutputNeuron(j*group_size, group_size); // TODO: refactor this for fast computing.

            if(any(spiked_neuron_id_cum[i], output_group, group_size)){
                //                std::cout << spiked_neuron_id_cum[i] << " - spiked" << " - frame - " << frameCount << std::endl;
                spiked_num_of_output_group[j] += 1;
            }
            // Dispose of output group array.
            delete[] output_group;
        }
    }
}

// the thing that produces the actual output
int SpikingNet::getSpikedOutput(int index){
    return spiked_num_of_output_group[index];
}


void SpikingNet::update_input(){

    //setup normal distribution random function
    std::normal_distribution<> normalRand(0.0, 1.0);

    //pseudo thalamus noise-input
    for(int i=0; i<Number_Of_Neurons; ++i){

        if(i < Number_Of_Inhibitory ){
            neurons[i].addToI(Noise_Ratio_Inh*normalRand(rand_gen));
        }else{
            neurons[i].addToI(Noise_Ratio_Exc*normalRand(rand_gen));
        }
    }

    //input from connected neurons with STP
    for(int i=0; i<Number_Of_Neurons; ++i){
        if(neurons[i].isFiring()){
            for(int j=0; j<Number_Of_Neurons; ++j){

                if(i != j){
                    if(i>Number_Of_Inhibitory)
                    neurons[j].addToI((float)weights[i][j]*(float)stp_wf[i]);
                    else
                    neurons[j].addToI((float)weights[i][j]);
                }
            }
        }
    }

}

void SpikingNet::update_neuron(){

    // update differential equation
    for(int i=0; i<Number_Of_Neurons; ++i){
        neurons[i].update();
        neurons[i].setI(0.0);
    }

}


void SpikingNet::stdp(){

    // read http://www.scholarpedia.org/article/Spike-timing_dependent_plasticity for more info on the maths behind this
    
    // decrease the stdp_count for all neurons. this keeps track of how far away in time the neuron has fired.
    // if the neuron is currently firing, set its stdp_count to stdp_tau.
    // when it reaches 0, it won't be considered when changing weights according to spike time changes.
    
    // TODO: this is not framerate invariant, as it measures timings by counting frames. this needs to be fixed.
    
    for(int i=Number_Of_Inhibitory; i<Number_Of_Neurons; ++i){
        if(stdp_counts[i]>0) stdp_counts[i] = stdp_counts[i] - 1;
        if(neurons[i].isFiring()) stdp_counts[i] = stdp_tau;
    }

    double d;
    for(int i=Number_Of_Inhibitory; i<Number_Of_Neurons; i++){

        if(neurons[i].isFiring()){

            for(int j=Number_Of_Inhibitory; j<Number_Of_Neurons; j++){

                if(stdp_counts[j]>0 && stdp_counts[j] != stdp_tau && i != j){
                    // another (uniquely different) neuron has fired in the last stdp_tau frames (excluding the current frame)
                    //

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

    for(int i=Number_Of_Inhibitory; i<Number_Of_Neurons; ++i){

        if(neurons[i].isFiring()){
            stp_wf[i] = getStpValue(i,1);
        }else{
            stp_wf[i] = getStpValue(i,0);
        }
    }
}

double SpikingNet::getStpValue(int index, int is_firing){
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
    
    double dx = (1.0-x)/tau_d - u*x*s;       // change for x
    double du = (U - u)/tau_f + U*(1.0-u)*s; // change for u

    double nu = u+du; // new value for u
    double nx = x+dx; // new value for x
    
    double wf = nu*nx; // final value returned. modulates synaptic efficacy
    
    // update stored values
    stp_u[index] = nu;
    stp_x[index] = nx;

    return wf;
}

void SpikingNet::decay(){

    for(int i=Number_Of_Inhibitory; i<Number_Of_Neurons; i++){
        for(int j=Number_Of_Inhibitory; j<Number_Of_Neurons; j++){
            weights[i][j] = weights[i][j] * ConstParams::Decay_Rate;
        }
    }

}

void SpikingNet::update(){

    checkFiring();
    checkTask();
    if(ConstParams::Decay_Flag) decay();
    if(ConstParams::Stdp_Flag)  stdp();
    if(ConstParams::Stp_Flag)   stp();

    update_input();
    update_neuron();

    frameCount++;
}

void SpikingNet::stimulation(){
    // external input to input neurons.
    for(int i=0; i<ConstParams::Input_Neuron_Size; ++i){
        neurons[input_neurons[i]].addToI(ConstParams::Stim_Strength);
    }
}

void SpikingNet::stimulation(double stim_strength_){
    // external input to input neurons.
    for(int i=0; i<ConstParams::Input_Neuron_Size; ++i){
        neurons[input_neurons[i]].addToI(stim_strength_);
    }
}

void SpikingNet::stimulation(int group_id_, double stim_strength_){

    int group_size = floor((float)ConstParams::Input_Neuron_Size / (float)ConstParams::Input_Group_Size);

    // external input to input neurons.
    for(int i=0; i<ConstParams::Input_Group_Size; ++i){
        for(int j=(i*group_size); j<((i+1)*group_size); ++j){
            if(i == group_id_){
                neurons[input_neurons[j]].addToI(stim_strength_);
            }else if(i > group_id_){
                break;
            }
        }
    }
}

void SpikingNet::wholeStimulation(){
    //    external input
    for(int i=0; i<ConstParams::Input_Neuron_Size; ++i){
        neurons[input_neurons[i]].addToI(ConstParams::Stim_Strength);
    }
}

void SpikingNet::wholeStimulation(double stim_strengh_){
    //    external input

    for(int i=ConstParams::Number_Of_Inhibitory; i<ConstParams::Number_Of_Neurons; ++i){
        if(any(i, input_neurons)==true){
            neurons[i].addToI(stim_strengh_);
        }
    }
}

void SpikingNet::wholeNetworkStimulation(){
    //    external input
    for(int i=0; i<ConstParams::Number_Of_Neurons; ++i){
        neurons[i].addToI(ConstParams::Stim_Strength);
    }
}

void SpikingNet::wholeNetworkStimulation(double stim_strengh_){
    //    external input
    for(int i=0; i<ConstParams::Number_Of_Neurons; ++i){
            neurons[i].addToI(stim_strengh_);
    }
}

bool SpikingNet::any(int target, const int *reference, int reference_size){
    bool answer = false;
    for(int i=0; i<reference_size; ++i){
        if(target == reference[i]){
            answer = true;
            break;
        }
    }
    return answer;
}

bool SpikingNet::any(int target, std::vector<int> reference){
    bool answer = false;
    for(unsigned int i=0; i<reference.size(); ++i){
        if(target == reference[i]){
            answer = true;
            break;
        }
    }
    return answer;
}
