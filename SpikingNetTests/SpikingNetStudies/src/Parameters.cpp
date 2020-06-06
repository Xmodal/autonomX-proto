//
//  File.cpp
//  SpikingNeuronsSimulator_Robot
//
//  Created by atsmsmr on 2015/06/21.
//
//

#include "Parameters.h"

// network settings
int ConstParams::Number_Of_Neurons = 1000; //1 side = 9 * 36 * 10 = 3240
int ConstParams::Inhibitory_Portion = 5;
int ConstParams::Number_Of_Inhibitory = ConstParams::Number_Of_Neurons/ConstParams::Inhibitory_Portion;
int ConstParams::Number_Of_Connection = 20; //each neuron

int ConstParams::Network_Type = Grid_network;//Sparse_Network Random_Network Uniform_Network Grid_network 6
int ConstParams::Excitatory_Neuron_Type = ConstParams::Chattering; //original was const
int ConstParams::Inhibitory_Neuron_Type = ConstParams::Chattering; //original was const

int ConstParams::Input_Portion = 5;
int ConstParams::Input_Group_Size  = 1;
int ConstParams::Input_Neuron_Size  = ConstParams::Number_Of_Neurons/ConstParams::Input_Portion;
int ConstParams::Input_Neuron_Per_Group  = ConstParams::Input_Neuron_Size/ConstParams::Input_Group_Size;

int ConstParams::Output_Portion = 5;
int ConstParams::Output_Group_Size = 1;
int ConstParams::Output_Neuron_Size = ConstParams::Number_Of_Neurons/ConstParams::Output_Portion;
int ConstParams::Output_Neuron_Per_Group = ConstParams::Output_Neuron_Size/ConstParams::Output_Group_Size;

int ConstParams::Grid_network_width = 20;

// synapse settings
double ConstParams::Init_Weight_Ratio_Exc = 15.0;//5.0, 6.0
double ConstParams::Init_Weight_Ratio_Inh = -5.0;//-5.0, -5.0
double ConstParams::Weight_Max = 20.0;
double ConstParams::Weight_Min = 0.0;
double ConstParams::Decay_Rate = 0.9999995; //original was const   //standard takashi : 0.9999995

// input-current settings
double ConstParams::Noise_Ratio_Exc = 5.0;
double ConstParams::Noise_Ratio_Inh = 3.0;
int    ConstParams::Stim_Rate = 1;
double ConstParams::Stim_Strength = 6.0;

// flags
bool ConstParams::Stp_Flag               = false ;
bool ConstParams::Stdp_Flag              = true;
bool ConstParams::Decay_Flag             = true;
bool ConstParams::Direct_Connection_Flag = true;

// utilities settings
bool ConstParams::Debug_Flag       = false;
bool ConstParams::Show_Msg_Flag    = false;
bool ConstParams::Random_Device_Flag = true;
int  ConstParams::Random_Seed = (unsigned) time(NULL);
const bool ConstParams::SaveSpikes   = false;
const bool ConstParams::SaveWeights  = false;


