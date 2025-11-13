//
// Created by aryel on 11/13/25.
//

#include "../header/DataManagement.h"
#include <cmath>
#include <algorithm>

// Constructor
DataManagement::DataManagement() {
    stations = new BinarySearchTree();
}

// Destructor
DataManagement::~DataManagement() {
    delete stations;
    grafo.clear();
    coords.clear();
    closedSegments.clear();
}

// ==================== GESTIÓN DE ESTACIONES ====================

void DataManagement::addStation(int id, const string& name, float x, float y) {
    // Verificar que la estación no existe
    if (stationExists(id)) {
        cerr << "Error: La estación con ID " << id << " ya existe." << endl;
        return;
    }

    // Crear la estación SIN coordenadas (separación de responsabilidades)
    Station newStation(id, name);
    
    // Agregar al árbol binario (maneja búsqueda/ordenamiento)
    stations->insert(newStation);
    
    // Agregar al grafo (inicialmente sin aristas)
    if (grafo.find(id) == grafo.end()) {
        grafo[id] = vector<pair<int, float>>();
    }
    
    // Agregar coordenadas al mapa separado (maneja posiciones geográficas)
    coords[id] = make_pair(x, y);
    
    cout << "Estación agregada: ID=" << id << ", Nombre=" << name 
         << ", Coordenadas=(" << x << ", " << y << ")" << endl;
}

void DataManagement::removeStation(int id) {
    if (!stationExists(id)) {
        cerr << "Error: La estación con ID " << id << " no existe." << endl;
        return;
    }

    // Eliminar del árbol
    stations->remove(id);
    
    // Eliminar todas sus aristas del grafo
    if (grafo.find(id) != grafo.end()) {
        grafo.erase(id);
    }
    
    // Eliminar sus aristas en sentido inverso
    for (auto& par : grafo) {
        auto& adyacentes = par.second;
        adyacentes.erase(
            remove_if(adyacentes.begin(), adyacentes.end(),
                      [id](const pair<int, float>& p) { return p.first == id; }),
            adyacentes.end()
        );
    }
    
    // Eliminar coordenadas
    if (coords.find(id) != coords.end()) {
        coords.erase(id);
    }
    
    // Eliminar de segmentos cerrados
    auto it = closedSegments.begin();
    while (it != closedSegments.end()) {
        if (it->first == id || it->second == id) {
            it = closedSegments.erase(it);
        } else {
            ++it;
        }
    }
    
    cout << "Estación eliminada: ID=" << id << endl;
}

Station* DataManagement::getStation(int id) const {
    return stations->find(id);
}

bool DataManagement::stationExists(int id) const {
    return stations->search(id);
}

vector<Station> DataManagement::getAllStations() const {
    return stations->getStationsAlphabetically();
}

// ==================== GESTIÓN DE RUTAS ====================

void DataManagement::addRoute(int stationA, int stationB, float distance) {
    // Verificar que ambas estaciones existen
    if (!stationExists(stationA)) {
        cerr << "Error: La estación A con ID " << stationA << " no existe." << endl;
        return;
    }
    if (!stationExists(stationB)) {
        cerr << "Error: La estación B con ID " << stationB << " no existe." << endl;
        return;
    }

    // Evitar agregar rutas a sí mismo
    if (stationA == stationB) {
        cerr << "Error: No se pueden crear rutas de una estación a sí misma." << endl;
        return;
    }

    // Verificar que la distancia es positiva
    if (distance <= 0) {
        cerr << "Error: La distancia debe ser positiva." << endl;
        return;
    }

    // Agregar ruta bidireccional
    // Ruta de A a B
    auto& adyacentesA = grafo[stationA];
    auto it = find_if(adyacentesA.begin(), adyacentesA.end(),
                      [stationB](const pair<int, float>& p) { return p.first == stationB; });
    
    if (it != adyacentesA.end()) {
        it->second = distance; // Actualizar distancia si ya existe
    } else {
        adyacentesA.push_back(make_pair(stationB, distance));
    }

    // Ruta de B a A
    auto& adyacentesB = grafo[stationB];
    it = find_if(adyacentesB.begin(), adyacentesB.end(),
                 [stationA](const pair<int, float>& p) { return p.first == stationA; });
    
    if (it != adyacentesB.end()) {
        it->second = distance;
    } else {
        adyacentesB.push_back(make_pair(stationA, distance));
    }

    cout << "Ruta agregada: " << stationA << " <-> " << stationB 
         << " (distancia: " << distance << ")" << endl;
}

void DataManagement::removeRoute(int stationA, int stationB) {
    // Verificar que ambas estaciones existen en el grafo
    if (grafo.find(stationA) == grafo.end() || grafo.find(stationB) == grafo.end()) {
        cerr << "Error: Una o ambas estaciones no existen en el grafo." << endl;
        return;
    }

    // Eliminar ruta de A a B
    auto& adyacentesA = grafo[stationA];
    adyacentesA.erase(
        remove_if(adyacentesA.begin(), adyacentesA.end(),
                  [stationB](const pair<int, float>& p) { return p.first == stationB; }),
        adyacentesA.end()
    );

    // Eliminar ruta de B a A
    auto& adyacentesB = grafo[stationB];
    adyacentesB.erase(
        remove_if(adyacentesB.begin(), adyacentesB.end(),
                  [stationA](const pair<int, float>& p) { return p.first == stationA; }),
        adyacentesB.end()
    );

    // Eliminar de segmentos cerrados si estaba
    closedSegments.erase(make_pair(stationA, stationB));
    closedSegments.erase(make_pair(stationB, stationA));

    cout << "Ruta eliminada: " << stationA << " <-> " << stationB << endl;
}

vector<pair<int, float>> DataManagement::getAdjacentStations(int stationId) const {
    if (grafo.find(stationId) == grafo.end()) {
        return vector<pair<int, float>>();
    }
    return grafo.at(stationId);
}

float DataManagement::getDistance(int stationA, int stationB) const {
    if (grafo.find(stationA) == grafo.end()) {
        return -1.0f; // No existe la estación
    }

    const auto& adyacentes = grafo.at(stationA);
    auto it = find_if(adyacentes.begin(), adyacentes.end(),
                      [stationB](const pair<int, float>& p) { return p.first == stationB; });

    if (it != adyacentes.end()) {
        return it->second;
    }

    return -1.0f; // No hay ruta directa
}

// ==================== GESTIÓN DE SEGMENTOS CERRADOS ====================

void DataManagement::closeRoute(int stationA, int stationB) {
    // Verificar que la ruta existe
    if (getDistance(stationA, stationB) < 0) {
        cerr << "Error: No existe una ruta entre las estaciones " << stationA 
             << " y " << stationB << endl;
        return;
    }

    // Normalizar el par (menor id primero) para evitar duplicados
    int min_id = min(stationA, stationB);
    int max_id = max(stationA, stationB);

    closedSegments.insert(make_pair(min_id, max_id));
    cout << "Ruta cerrada: " << stationA << " <-> " << stationB << endl;
}

void DataManagement::openRoute(int stationA, int stationB) {
    // Normalizar el par
    int min_id = min(stationA, stationB);
    int max_id = max(stationA, stationB);

    if (closedSegments.erase(make_pair(min_id, max_id)) > 0) {
        cout << "Ruta abierta: " << stationA << " <-> " << stationB << endl;
    } else {
        cerr << "Error: La ruta entre " << stationA << " y " << stationB 
             << " no estaba cerrada." << endl;
    }
}

bool DataManagement::isRouteClosed(int stationA, int stationB) const {
    int min_id = min(stationA, stationB);
    int max_id = max(stationA, stationB);
    return closedSegments.find(make_pair(min_id, max_id)) != closedSegments.end();
}

// ==================== GESTIÓN DE COORDENADAS ====================

pair<float, float> DataManagement::getCoordinates(int stationId) const {
    if (coords.find(stationId) == coords.end()) {
        return make_pair(-1.0f, -1.0f); // Indicar que no existe
    }
    return coords.at(stationId);
}

void DataManagement::updateCoordinates(int stationId, float x, float y) {
    if (!stationExists(stationId)) {
        cerr << "Error: La estación con ID " << stationId << " no existe." << endl;
        return;
    }

    coords[stationId] = make_pair(x, y);
    cout << "Coordenadas actualizadas para estación " << stationId 
         << ": (" << x << ", " << y << ")" << endl;
}

// ==================== CONSULTAS GENERALES ====================

bool DataManagement::isEmpty() const {
    return stations->isEmpty();
}

int DataManagement::getStationCount() const {
    return grafo.size();
}

const Grafo& DataManagement::getGrafo() const {
    return grafo;
}

const Coords& DataManagement::getCoords() const {
    return coords;
}
