//
// Created by aryel on 11/13/25.
//
#ifndef DONROUTE_STATION_H
#define DONROUTE_STATION_H
#include <iostream>
using namespace std;

struct Station {
    int id;
    string name;
    
public:
    // Constructor simple - las coordenadas se manejan en DataManagement::coords
    Station (int id, const string &name) 
        : id(id), name(name) {}
};


#endif //DONROUTE_STATION_H