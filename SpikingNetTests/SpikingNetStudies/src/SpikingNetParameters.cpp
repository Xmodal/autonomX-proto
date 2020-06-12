//
//  File.cpp
//  SpikingNeuronsSimulator_Robot
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

#include "SpikingNetParameters.h"

// network settings
int SpikingNetParameters::Neuron_Size = 1000; //1 side = 9 * 36 * 10 = 3240
int SpikingNetParameters::Inhibitory_Portion = 5;
int SpikingNetParameters::Inhibitory_Size = SpikingNetParameters::Neuron_Size / SpikingNetParameters::Inhibitory_Portion;
int SpikingNetParameters::Connections_Per_Neuron = 20; //each neuron

int SpikingNetParameters::Network_Type = Grid_network;//Sparse_Network Random_Network Uniform_Network Grid_network 6
int SpikingNetParameters::Excitatory_Neuron_Type = SpikingNetParameters::Chattering; //original was const
int SpikingNetParameters::Inhibitory_Neuron_Type = SpikingNetParameters::Chattering; //original was const

int SpikingNetParameters::Input_Portion = 5;
int SpikingNetParameters::Input_Group_Size  = 1;
int SpikingNetParameters::Input_Size  = SpikingNetParameters::Neuron_Size / SpikingNetParameters::Input_Portion;

int SpikingNetParameters::Output_Portion = 5;
int SpikingNetParameters::Output_Group_Size = 1;
int SpikingNetParameters::Output_Size = SpikingNetParameters::Neuron_Size / SpikingNetParameters::Output_Portion;

int SpikingNetParameters::Grid_Network_Width = 20;

// synapse settings
double SpikingNetParameters::Init_Weight_Ratio_Exc = 15.0;//5.0, 6.0
double SpikingNetParameters::Init_Weight_Ratio_Inh = -5.0;//-5.0, -5.0
double SpikingNetParameters::Weight_Max = 20.0;
double SpikingNetParameters::Weight_Min = 0.0;
double SpikingNetParameters::Decay_Rate = 0.9999995; //original was const   //standard takashi : 0.9999995

// input-current settings
double SpikingNetParameters::Noise_Ratio_Exc = 5.0;
double SpikingNetParameters::Noise_Ratio_Inh = 3.0;
int    SpikingNetParameters::Stim_Rate = 1;
double SpikingNetParameters::Stim_Strength = 6.0;

// flags
bool SpikingNetParameters::Stp_Flag               = false ;
bool SpikingNetParameters::Stdp_Flag              = true;
bool SpikingNetParameters::Decay_Flag             = true;
bool SpikingNetParameters::Direct_Connection_Flag = true;

// utilities settings
bool SpikingNetParameters::Debug_Flag       = false;
bool SpikingNetParameters::Show_Msg_Flag    = false;
bool SpikingNetParameters::Random_Device_Flag = true;
int  SpikingNetParameters::Random_Seed = (unsigned) time(NULL);
const bool SpikingNetParameters::SaveWeights  = false;


