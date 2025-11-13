//
// Created by aryel on 11/13/25.
//

#ifndef DONROUTE_BINARYSEARCHTREE_H
#define DONROUTE_BINARYSEARCHTREE_H
#include <iostream>
#include <vector>
#include <string>
#include "Node.h"

using namespace std;



class BinarySearchTree {
private:
    Node* root;

    // Métodos privados recursivos auxiliares
    Node* insertHelper(Node* node, const Station& station);
    Node* deleteHelper(Node* node, int id);
    Node* findMin(Node* node);
    void inOrderHelper(Node* node, vector<Station>& result) const; // Para recorrido
    void preOrderHelper(Node* node, vector<Station>& result) const; // Para recorrido
    void postOrderHelper(Node* node, vector<Station>& result) const; // Para recorrido
    void destroyTree(Node* node); // Para limpiar memoria en el destructor

public:
    BinarySearchTree(); // Constructor
    ~BinarySearchTree(); // Destructor
    BinarySearchTree(const BinarySearchTree&) = delete; // Deshabilitar copia si no la implementas
    BinarySearchTree& operator=(const BinarySearchTree&) = delete; // Deshabilitar asignación si no la implementas

    void insert(const Station& station);
    void remove(int id);
    void clear(); // Limpiar todo el árbol
    Station* find(int id) const; // Devuelve puntero a la estación o nullptr si no la encuentra

    // Métodos para recorridos
    vector<Station> inOrder() const;
    vector<Station> preOrder() const;
    vector<Station> postOrder() const;

    // Métodos para obtener recorridos como strings
    string getPreOrderString() const;
    string getInOrderString() const;
    string getPostOrderString() const;

    // Método para obtener todas las estaciones (in-order)
    vector<Station> getAllStations() const;

    // Método para obtener todas las estaciones ordenadas alfabéticamente (usando in-order)
    vector<Station> getStationsAlphabetically() const;

    // Método para verificar si el árbol está vacío (útil para la GUI)
    bool isEmpty() const;
    
    // Método para buscar y retornar puntero a Station
    Station* search(int id) const;
};


#endif //DONROUTE_BINARYSEARCHTREE_H