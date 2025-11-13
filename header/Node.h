//
// Created by aryel on 11/13/25.
//

#ifndef DONROUTE_NODE_H
#define DONROUTE_NODE_H
#include <iostream>

#include "Station.h"
using namespace std;


struct Node {
    Station data;
    Node* left;
    Node* right;

    Node(const Station& stationData) : data(stationData), left(nullptr), right(nullptr) {}
};


#endif //DONROUTE_NODE_H