//
// Created by aryel on 11/13/25.
//

#include "../header/BinarySearchTree.h"

// Constructor
BinarySearchTree::BinarySearchTree() : root(nullptr) {}

// Destructor
BinarySearchTree::~BinarySearchTree() {
    destroyTree(root);
}

// Insertar una estación en el árbol
void BinarySearchTree::insert(const Station& station) {
    root = insertHelper(root, station);
}

// Método auxiliar recursivo para insertar
Node* BinarySearchTree::insertHelper(Node* node, const Station& station) {
    if (node == nullptr) {
        return new Node(station);
    }

    // Comparar por ID
    if (station.id < node->data.id) {
        node->left = insertHelper(node->left, station);
    } else if (station.id > node->data.id) {
        node->right = insertHelper(node->right, station);
    }
    // Si son iguales, no insertamos duplicados

    return node;
}

// Eliminar una estación del árbol por ID
void BinarySearchTree::remove(int id) {
    root = deleteHelper(root, id);
}

// Método auxiliar recursivo para eliminar
Node* BinarySearchTree::deleteHelper(Node* node, int id) {
    if (node == nullptr) {
        return nullptr;
    }

    // Comparar IDs
    if (id < node->data.id) {
        node->left = deleteHelper(node->left, id);
    } else if (id > node->data.id) {
        node->right = deleteHelper(node->right, id);
    } else {
        // Nodo encontrado
        // Caso 1: Sin hijos (hoja)
        if (node->left == nullptr && node->right == nullptr) {
            delete node;
            return nullptr;
        }
        // Caso 2: Un hijo
        if (node->left == nullptr) {
            Node* temp = node->right;
            delete node;
            return temp;
        }
        if (node->right == nullptr) {
            Node* temp = node->left;
            delete node;
            return temp;
        }
        // Caso 3: Dos hijos
        Node* minRight = findMin(node->right);
        node->data = minRight->data;
        node->right = deleteHelper(node->right, minRight->data.id);
    }

    return node;
}

// Encontrar el nodo con valor mínimo
Node* BinarySearchTree::findMin(Node* node) {
    if (node == nullptr) {
        return nullptr;
    }
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

// Buscar si existe una estación con el ID dado
bool BinarySearchTree::search(int id) const {
    return find(id) != nullptr;
}

// Encontrar y retornar una estación por ID
Station* BinarySearchTree::find(int id) const {
    Node* current = root;

    while (current != nullptr) {
        if (id == current->data.id) {
            return &(current->data);
        } else if (id < current->data.id) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return nullptr;
}

// Recorrido in-order (izquierda, raíz, derecha)
vector<Station> BinarySearchTree::inOrder() const {
    vector<Station> result;
    inOrderHelper(root, result);
    return result;
}

// Método auxiliar recursivo para recorrido in-order
void BinarySearchTree::inOrderHelper(Node* node, vector<Station>& result) const {
    if (node == nullptr) {
        return;
    }
    inOrderHelper(node->left, result);
    result.push_back(node->data);
    inOrderHelper(node->right, result);
}

// Recorrido pre-order (raíz, izquierda, derecha)
vector<Station> BinarySearchTree::preOrder() const {
    vector<Station> result;
    preOrderHelper(root, result);
    return result;
}

// Método auxiliar recursivo para recorrido pre-order
void BinarySearchTree::preOrderHelper(Node* node, vector<Station>& result) const {
    if (node == nullptr) {
        return;
    }
    result.push_back(node->data);
    preOrderHelper(node->left, result);
    preOrderHelper(node->right, result);
}

// Recorrido post-order (izquierda, derecha, raíz)
vector<Station> BinarySearchTree::postOrder() const {
    vector<Station> result;
    postOrderHelper(root, result);
    return result;
}

// Método auxiliar recursivo para recorrido post-order
void BinarySearchTree::postOrderHelper(Node* node, vector<Station>& result) const {
    if (node == nullptr) {
        return;
    }
    postOrderHelper(node->left, result);
    postOrderHelper(node->right, result);
    result.push_back(node->data);
}

// Obtener todas las estaciones ordenadas alfabéticamente (usando in-order)
vector<Station> BinarySearchTree::getStationsAlphabetically() const {
    return inOrder();
}

// Verificar si el árbol está vacío
bool BinarySearchTree::isEmpty() const {
    return root == nullptr;
}

// Destruir el árbol (limpiar memoria)
void BinarySearchTree::destroyTree(Node* node) {
    if (node == nullptr) {
        return;
    }
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}
