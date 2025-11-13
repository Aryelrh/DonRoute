//
// Created by aryel on 11/13/25.
//

#ifndef DONROUTE_BINARYSEARCHTREE_H
#define DONROUTE_BINARYSEARCHTREE_H
#include <iostream>
#include <vector>
#include "Node.h"



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
    bool search(int id) const; // Devuelve true si encuentra la estación
    Station* find(int id) const; // Devuelve puntero a la estación o nullptr si no la encuentra

    // Métodos para recorridos
    vector<Station> inOrder() const;
    vector<Station> preOrder() const;
    vector<Station> postOrder() const;

    // Método para obtener todas las estaciones ordenadas alfabéticamente (usando in-order)
    vector<Station> getStationsAlphabetically() const;

    // Método para verificar si el árbol está vacío (útil para la GUI)
    bool isEmpty() const;
};


#endif //DONROUTE_BINARYSEARCHTREE_H