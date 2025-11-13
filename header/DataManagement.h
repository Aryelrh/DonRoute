//
// Created by aryel on 11/13/25.
//

#ifndef DONROUTE_DATAMANAGEMENT_H
#define DONROUTE_DATAMANAGEMENT_H
#include <iostream>
using namespace std;
#include <map>
#include <utility>
#include <unordered_set>

#include "BinarySearchTree.h"

using Grafo = map<int, vector<pair<int, float>>>;
using Coords = map<int, pair<float, float>>;

struct PairHash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        // Un hash simple combinando los dos IDs
        auto h1 = hash<int>{}(p.first);
        auto h2 = hash<int>{}(p.second);
        // Se combina el hash de la primera y segunda component
        return h1 ^ (h2 << 1);
    }
};

using ClosedSegments = unordered_set<pair<int, int>, PairHash>;

class DataManagement {
private:
    BinarySearchTree* stations;
    Grafo grafo;
    Coords coords;
    ClosedSegments closedSegments;

public:
    DataManagement();
    ~DataManagement();

    // Gestión de estaciones
    void addStation(int id, const string& name, float x, float y);
    void removeStation(int id);
    Station* getStation(int id) const;
    bool stationExists(int id) const;
    vector<Station> getAllStations() const;

    // Gestión de rutas (aristas del grafo)
    void addRoute(int stationA, int stationB, float distance);
    void removeRoute(int stationA, int stationB);
    vector<pair<int, float>> getAdjacentStations(int stationId) const;
    float getDistance(int stationA, int stationB) const;

    // Gestión de segmentos cerrados
    void closeRoute(int stationA, int stationB);
    void openRoute(int stationA, int stationB);
    bool isRouteClosed(int stationA, int stationB) const;

    // Gestión de coordenadas
    pair<float, float> getCoordinates(int stationId) const;
    void updateCoordinates(int stationId, float x, float y);

    // Consultas generales
    bool isEmpty() const;
    int getStationCount() const;
    const Grafo& getGrafo() const;
    const Coords& getCoords() const;
};


#endif //DONROUTE_DATAMANAGEMENT_H