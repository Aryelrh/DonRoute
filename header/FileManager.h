#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <utility>
#include "DataManagement.h"
#include "BinarySearchTree.h"

using namespace std;

class FileManager {
public:
    FileManager();
    ~FileManager();
    
    // Guardar datos
    bool saveStations(const BinarySearchTree& bst, const map<int, pair<float, float>>& coords);
    bool saveRoutes(const Grafo& grafo);
    bool saveClosures(const unordered_set<pair<int, int>, PairHash>& closedSegments);
    
    // Cargar datos
    bool loadStations(BinarySearchTree& bst, map<int, pair<float, float>>& coords, Grafo& grafo);
    bool loadRoutes(Grafo& grafo);
    bool loadClosures(unordered_set<pair<int, int>, PairHash>& closedSegments);
    
    // Generar reporte
    bool generateReport(
        const BinarySearchTree& bst,
        const Grafo& grafo,
        const map<int, pair<float, float>>& coords,
        int totalQueries,
        const string& preOrder,
        const string& inOrder,
        const string& postOrder
    );
    
private:
    string savesDirectory;
    string stationsFile;
    string routesFile;
    string closuresFile;
    string reportFile;
    
    bool createDirectoryIfNotExists();
};

#endif // FILEMANAGER_H
