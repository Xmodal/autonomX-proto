//
//  SpikeNetWriter.cpp
//  spiking_neurons_simulator
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

#include "SpikingNetWriter.h"

SpikingNetWriter::SpikingNetWriter(){
    
}

SpikingNetWriter::~SpikingNetWriter(){
    
}

void SpikingNetWriter::open(std::string filepath){
    sn_stream.open(filepath);
};

void SpikingNetWriter::open(char* filepath){
    sn_stream.open(filepath);
};

void SpikingNetWriter::write(int frame_current, double value){
    sn_stream << frame_current << "," << value << std::endl;
};

void SpikingNetWriter::writeWeights(int frame_current, double** weights){
    
    for(int i = 0; i < SpikingNetParameters::Neuron_Size; i++){
        for(int j = 0; j < SpikingNetParameters::Neuron_Size; j++){
            
            sn_stream << frame_current << "," << i << "," << j << "," << weights[i][j] << "\n";
        }
    }
    
};

void SpikingNetWriter::writeSome(std::vector<double> some_values){
    
    for(int i = 0; i < some_values.size(); i++){
        sn_stream << some_values[i];
        if(i != some_values.size() - 1) sn_stream << ",";
        else sn_stream << "\n";
    }
};

void SpikingNetWriter::writeSettings(){
    
    struct tm *date;
    time_t now;
    int year, month, day;
    int hour, minute, second;
    
    time(&now);
    date = localtime(&now);
    
    year = date->tm_year + 1900;
    month = date->tm_mon + 1;
    day = date->tm_mday;
    hour = date->tm_hour;
    minute = date->tm_min;
    second = date->tm_sec;
    
    sn_stream << "----------Information------------------------------\n";
    sn_stream << "Start Date :                   " << year << "/" << month << "/" << day << "/ " << hour << ":" << minute << ":" << second << "\n";
    sn_stream << "Weight Data :                  " << SpikingNetParameters::SaveWeights            << "\n";
    
    sn_stream << "----------Network settings-------------------------\n";
    sn_stream << "Neurons :                      " << SpikingNetParameters::Neuron_Size      << "\n";
    sn_stream << "Connections :                  " << SpikingNetParameters::Connections_Per_Neuron   << "\n";
    sn_stream << "Inhibitory Neurons:            " << SpikingNetParameters::Inhibitory_Size   << "\n";
    
    sn_stream << "Exc Neuron Type :              " << SpikingNetParameters::Excitatory_Neuron_Type << "\n";
    sn_stream << "Inh Neuron Type :              " << SpikingNetParameters::Inhibitory_Neuron_Type << "\n";
    sn_stream << "Network Type :                 " << SpikingNetParameters::Network_Type           << "\n";
    
    sn_stream << "Input Neurons :                " << SpikingNetParameters::Input_Size      << "\n";
    sn_stream << "Input Groups :                 " << SpikingNetParameters::Input_Group_Size       << "\n";
    sn_stream << "Output Neurons :               " << SpikingNetParameters::Output_Size     << "\n";
    sn_stream << "Output Groups :                " << SpikingNetParameters::Output_Group_Size      << "\n";
    
    
    sn_stream << "----------Experimental settings--------------------\n";
    sn_stream << "Direct Connection I/O :        " << SpikingNetParameters::Direct_Connection_Flag << "\n";
    sn_stream << "STP :                          " << SpikingNetParameters::Stp_Flag               << "\n";
    sn_stream << "STDP :                         " << SpikingNetParameters::Stdp_Flag              << "\n";
    
    sn_stream << "----------Model settings---------------------------\n";
    sn_stream << "Initial Weight Ratio - Exc :   " << SpikingNetParameters::Init_Weight_Ratio_Exc  << "\n";
    sn_stream << "Initial Weight Ratio - Inh :   " << SpikingNetParameters::Init_Weight_Ratio_Inh  << "\n";
    sn_stream << "Noise Ratio - Exc :            " << SpikingNetParameters::Noise_Ratio_Exc        << "\n";
    sn_stream << "Noise Ratio - Inh :            " << SpikingNetParameters::Noise_Ratio_Inh        << "\n";
    sn_stream << "Weight Max :                   " << SpikingNetParameters::Weight_Max             << "\n";
    sn_stream << "Weight Min :                   " << SpikingNetParameters::Weight_Min             << "\n";
    sn_stream << "Stim Rate :                    " << SpikingNetParameters::Stim_Rate              << "\n";
    sn_stream << "Stim Strength :                " << SpikingNetParameters::Stim_Strength          << "\n";
    
    sn_stream << std::endl;
    
}

void SpikingNetWriter::close(){
    sn_stream.close();
}
