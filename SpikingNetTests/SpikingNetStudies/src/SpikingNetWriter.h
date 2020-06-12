//
//  SpikeNetWriter.h
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

#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <time.h>

#include "SpikingNetParameters.h"

class SpikingNetWriter{
private:
    
public:
    SpikingNetWriter();
    ~SpikingNetWriter();
    void open(std::string filepath);
    void open(char* filepath);
    void write (int frame_current, double value);
    void writeWeights(int frame_current, double **weights);
    void writeSome(std::vector<double> some_values);
    void writeSettings();
    void close();
    
    std::ofstream sn_stream;
};
