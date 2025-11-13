#include "../header/FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

FileManager::FileManager() {
    // Usar ruta relativa desde build/ hacia la raíz del proyecto
    savesDirectory = "../saves";
    stationsFile = savesDirectory + "/estaciones.txt";
    routesFile = savesDirectory + "/rutas.txt";
    closuresFile = savesDirectory + "/cierres.txt";
    reportFile = savesDirectory + "/reportes.txt";
    
    createDirectoryIfNotExists();
}

FileManager::~FileManager() {
}

bool FileManager::createDirectoryIfNotExists() {
    struct stat info;
    if (stat(savesDirectory.c_str(), &info) != 0) {
        // El directorio no existe, crearlo
        #ifdef _WIN32
            return mkdir(savesDirectory.c_str()) == 0;
        #else
            return mkdir(savesDirectory.c_str(), 0755) == 0;
        #endif
    }
    return true;
}

bool FileManager::saveStations(const BinarySearchTree& bst, const map<int, pair<float, float>>& coords) {
    createDirectoryIfNotExists();
    
    ofstream file(stationsFile);
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << stationsFile << endl;
        return false;
    }
    
    // Obtener todas las estaciones del árbol usando recorrido in-orden
    vector<Station> stations = bst.getAllStations();
    
    file << stations.size() << endl; // Número de estaciones
    
    for (const auto& station : stations) {
        // Obtener coordenadas
        auto it = coords.find(station.id);
        if (it != coords.end()) {
            // Usar | como delimitador para permitir espacios en nombres
            file << station.id << "|" 
                 << station.name << "|"
                 << it->second.first << "|"
                 << it->second.second << endl;
        }
    }
    
    file.close();
    cout << "Estaciones guardadas exitosamente en " << stationsFile << endl;
    return true;
}

bool FileManager::saveRoutes(const Grafo& grafo) {
    createDirectoryIfNotExists();
    
    ofstream file(routesFile);
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << routesFile << endl;
        return false;
    }
    
    // Contar rutas únicas (evitar duplicados bidireccionales)
    set<pair<int, int>> uniqueRoutes;
    for (const auto& pair : grafo) {
        int from = pair.first;
        for (const auto& adj : pair.second) {
            int to = adj.first;
            float weight = adj.second;
            
            // Asegurar que solo guardamos una vez cada ruta (from < to)
            if (from < to) {
                uniqueRoutes.insert({from, to});
            }
        }
    }
    
    file << uniqueRoutes.size() << endl; // Número de rutas
    
    // Guardar cada ruta con su peso
    for (const auto& route : uniqueRoutes) {
        int from = route.first;
        int to = route.second;
        
        // Buscar el peso
        float weight = 0.0f;
        auto it = grafo.find(from);
        if (it != grafo.end()) {
            for (const auto& adj : it->second) {
                if (adj.first == to) {
                    weight = adj.second;
                    break;
                }
            }
        }
        
        file << from << " " << to << " " << weight << endl;
    }
    
    file.close();
    cout << "Rutas guardadas exitosamente en " << routesFile << endl;
    return true;
}

bool FileManager::saveClosures(const unordered_set<pair<int, int>, PairHash>& closedSegments) {
    createDirectoryIfNotExists();
    
    ofstream file(closuresFile);
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << closuresFile << endl;
        return false;
    }
    
    // Evitar duplicados (si está (A,B) no guardar (B,A))
    set<pair<int, int>> uniqueClosures;
    for (const auto& closure : closedSegments) {
        int a = closure.first;
        int b = closure.second;
        if (a < b) {
            uniqueClosures.insert({a, b});
        } else {
            uniqueClosures.insert({b, a});
        }
    }
    
    file << uniqueClosures.size() << endl; // Número de cierres
    
    for (const auto& closure : uniqueClosures) {
        file << closure.first << " " << closure.second << endl;
    }
    
    file.close();
    cout << "Cierres guardados exitosamente en " << closuresFile << endl;
    return true;
}

bool FileManager::loadStations(BinarySearchTree& bst, map<int, pair<float, float>>& coords, Grafo& grafo) {
    ifstream file(stationsFile);
    if (!file.is_open()) {
        cout << "No se encontró archivo de estaciones previo" << endl;
        return false;
    }
    
    int numStations;
    file >> numStations;
    file.ignore(); // Ignorar el newline después del número
    
    for (int i = 0; i < numStations; i++) {
        string line;
        if (!getline(file, line)) {
            cerr << "Error leyendo línea de estación" << endl;
            continue;
        }
        
        // Parsear usando | como delimitador
        stringstream ss(line);
        string idStr, name, xStr, yStr;
        
        if (!getline(ss, idStr, '|') || 
            !getline(ss, name, '|') || 
            !getline(ss, xStr, '|') || 
            !getline(ss, yStr, '|')) {
            cerr << "Error parseando línea: " << line << endl;
            continue;
        }
        
        try {
            int id = stoi(idStr);
            float x = stof(xStr);
            float y = stof(yStr);
            
            Station station(id, name);
            
            bst.insert(station);
            coords[id] = {x, y};
            
            // Asegurar que el grafo tenga entrada para esta estación
            if (grafo.find(id) == grafo.end()) {
                grafo[id] = vector<pair<int, float>>();
            }
        } catch (const exception& e) {
            cerr << "Error convirtiendo valores: " << e.what() << endl;
            continue;
        }
    }
    
    file.close();
    cout << "Estaciones cargadas exitosamente desde " << stationsFile << endl;
    return true;
}

bool FileManager::loadRoutes(Grafo& grafo) {
    ifstream file(routesFile);
    if (!file.is_open()) {
        cout << "No se encontró archivo de rutas previo" << endl;
        return false;
    }
    
    int numRoutes;
    file >> numRoutes;
    
    for (int i = 0; i < numRoutes; i++) {
        int from, to;
        float weight;
        
        file >> from >> to >> weight;
        
        // Agregar bidireccional
        grafo[from].push_back({to, weight});
        grafo[to].push_back({from, weight});
    }
    
    file.close();
    cout << "Rutas cargadas exitosamente desde " << routesFile << endl;
    return true;
}

bool FileManager::loadClosures(unordered_set<pair<int, int>, PairHash>& closedSegments) {
    ifstream file(closuresFile);
    if (!file.is_open()) {
        cout << "No se encontró archivo de cierres previo" << endl;
        return false;
    }
    
    int numClosures;
    file >> numClosures;
    
    for (int i = 0; i < numClosures; i++) {
        int a, b;
        file >> a >> b;
        
        // Agregar bidireccional
        closedSegments.insert({a, b});
        closedSegments.insert({b, a});
    }
    
    file.close();
    cout << "Cierres cargados exitosamente desde " << closuresFile << endl;
    return true;
}

bool FileManager::generateReport(
    const BinarySearchTree& bst,
    const Grafo& grafo,
    const map<int, pair<float, float>>& coords,
    int totalQueries,
    const string& preOrder,
    const string& inOrder,
    const string& postOrder
) {
    createDirectoryIfNotExists();
    
    ofstream file(reportFile);
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << reportFile << endl;
        return false;
    }
    
    file << "============================================" << endl;
    file << "       REPORTE DEL SISTEMA DONROUTE        " << endl;
    file << "============================================" << endl << endl;
    
    // 1. Información General del Sistema
    file << "--- INFORMACION GENERAL DEL SISTEMA ---" << endl;
    vector<Station> allStations = bst.getAllStations();
    file << "Numero total de estaciones: " << allStations.size() << endl;
    file << "Numero de consultas realizadas: " << totalQueries << endl;
    
    // Contar rutas
    int totalRoutes = 0;
    set<pair<int, int>> uniqueRoutes;
    for (const auto& pair : grafo) {
        for (const auto& adj : pair.second) {
            if (pair.first < adj.first) {
                uniqueRoutes.insert({pair.first, adj.first});
            }
        }
    }
    file << "Numero total de rutas: " << uniqueRoutes.size() << endl;
    file << endl;
    
    // 2. Listado de Estaciones (ordenadas alfabéticamente)
    file << "--- LISTADO DE ESTACIONES (ORDEN ALFABETICO) ---" << endl;
    vector<Station> sortedStations = allStations;
    sort(sortedStations.begin(), sortedStations.end(), [](const Station& a, const Station& b) {
        return a.name < b.name;
    });
    
    for (const auto& station : sortedStations) {
        auto it = coords.find(station.id);
        if (it != coords.end()) {
            file << "ID: " << station.id 
                 << " | Nombre: " << station.name
                 << " | Coordenadas: (" << it->second.first << ", " << it->second.second << ")"
                 << endl;
        }
    }
    file << endl;
    
    // 3. Visualizar Rutas y Conexiones
    file << "--- RUTAS Y CONEXIONES EXISTENTES ---" << endl;
    for (const auto& route : uniqueRoutes) {
        int from = route.first;
        int to = route.second;
        
        // Buscar nombres de estaciones
        Station* fromStation = bst.search(from);
        Station* toStation = bst.search(to);
        
        // Buscar peso
        float weight = 0.0f;
        auto it = grafo.find(from);
        if (it != grafo.end()) {
            for (const auto& adj : it->second) {
                if (adj.first == to) {
                    weight = adj.second;
                    break;
                }
            }
        }
        
        if (fromStation && toStation) {
            file << fromStation->name << " <-> " << toStation->name 
                 << " | Tiempo: " << weight << " minutos" << endl;
        }
    }
    file << endl;
    
    // 4. Recorridos del Árbol Binario
    file << "--- RECORRIDOS DEL ARBOL BINARIO DE ESTACIONES ---" << endl;
    file << endl;
    
    file << "Recorrido PRE-ORDEN:" << endl;
    file << preOrder << endl << endl;
    
    file << "Recorrido IN-ORDEN:" << endl;
    file << inOrder << endl << endl;
    
    file << "Recorrido POST-ORDEN:" << endl;
    file << postOrder << endl << endl;
    
    file << "============================================" << endl;
    file << "          FIN DEL REPORTE                  " << endl;
    file << "============================================" << endl;
    
    file.close();
    cout << "Reporte generado exitosamente en " << reportFile << endl;
    return true;
}
