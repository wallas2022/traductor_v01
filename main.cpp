#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;


// Estructura del nodo del árbol AVL
struct AVLNode {
    std::string spanishWord;
    std::string italianWord;
    std::string frenchWord;
    std::string germanWord;
    std::string englishWord;
    AVLNode* left;
    AVLNode* right;
    int height;
};


// Estructura para el historial de búsqueda
struct SearchEntry {
    std::string username;
    std::string searchedWord;
    std::string translation;
};

int height(AVLNode* node);
int getHeight(AVLNode* node);
int getBalance(AVLNode* node);
AVLNode* rotateLeft(AVLNode* y);
AVLNode* rotateRight(AVLNode* y);
void saveSearchEntry(const SearchEntry& entry, const std::string& filename);
string encryptWord(const std::string& word);

// Función para encriptar una palabra según las reglas dadas
std::string encryptWord(const std::string& word) {
    std::string encryptedWord = word;

    // Reemplazar las vocales por su equivalente U
    for (char& c : encryptedWord) {
        char lowercase = std::tolower(c);
        if (lowercase == 'a') {
            c = 'U1';
        } else if (lowercase == 'e') {
            c = 'U2';
        } else if (lowercase == 'i') {
            c = 'U3';
        } else if (lowercase == 'o') {
            c = 'U4';
        } else if (lowercase == 'u') {
            c = 'U5';
        }
    }

    // Reemplazar las letras por su equivalente m o g
    for (char& c : encryptedWord) {
        char lowercase = std::tolower(c);
        if (lowercase >= 'a' && lowercase <= 'z') {
            c = 'm' + (lowercase - 'a' + 1);
        } else if (c >= 'A' && c <= 'Z') {
            c = 'g' + (c - 'A' + 1);
        }
    }

    return encryptedWord;
}



// Función para guardar una palabra en un archivo
void saveWordToFile(const std::string& word, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        return;
    }

    file << word << std::endl;

    file.close();
}


int height(AVLNode* node) {
    if (node == nullptr) {
        return 0;
    } else {
        int leftHeight = height(node->left);
        int rightHeight = height(node->right);
        return 1 + max(leftHeight, rightHeight);
    }
}

int getBalance(AVLNode* node) {
    if (node == nullptr) {
        return 0;
    }
    return height(node->left) - height(node->right);
}

// Función para buscar la traducción de una palabra en el árbol AVL
std::string searchTranslation(AVLNode* root, const std::string& word, const std::string& targetLanguage) {
    if (root == nullptr) {
        return ""; // Si el árbol está vacío, retorna una cadena vacía
    }

    // Buscar la palabra solicitada en el nodo actual
    if (word == root->spanishWord || word == root->italianWord || word == root->frenchWord || word == root->germanWord || word == root->englishWord) {
        // Se ha encontrado la palabra solicitada, ahora buscar la traducción en el idioma deseado
        if (targetLanguage == "espanol") {
            return root->spanishWord;
        } else if (targetLanguage == "italiano") {
            return root->italianWord;
        } else if (targetLanguage == "frances") {
            return root->frenchWord;
        } else if (targetLanguage == "aleman") {
            return root->germanWord;
        } else if (targetLanguage == "ingles") {
            return root->englishWord;
        } else {
            return "Idioma no válido";
        }
    } else if (word < root->spanishWord) {
        // Si la palabra buscada es menor que la palabra en el nodo actual, buscar en el subárbol izquierdo
        return searchTranslation(root->left, word, targetLanguage);
    } else {
        // Si la palabra buscada es mayor que la palabra en el nodo actual, buscar en el subárbol derecho
        return searchTranslation(root->right, word, targetLanguage);
    }
    // Si la palabra no se encuentra en el árbol, devolver una cadena vacía
    return "";
}



// Función para insertar un nuevo nodo en el árbol AVL
AVLNode* insertNode(AVLNode* root, const std::string& spanishWord, const std::string& italianWord, const std::string& frenchWord, const std::string& germanWord, const std::string& englishWord) {
    // Paso 1: Realizar una inserción normal de un árbol de búsqueda binaria
    if (root == nullptr) {
        AVLNode* newNode = new AVLNode;
        newNode->spanishWord = spanishWord;
        newNode->italianWord = italianWord;
        newNode->frenchWord = frenchWord;
        newNode->germanWord = germanWord;
        newNode->englishWord = englishWord;
        newNode->left = nullptr;
        newNode->right = nullptr;
        newNode->height = 1;
        return newNode;
    }

    // Comparar la palabra con la palabra en el nodo actual
    int compareResult = spanishWord.compare(root->spanishWord);

    // Insertar en el subárbol izquierdo si la palabra es menor
    if (compareResult < 0) {
        root->left = insertNode(root->left, spanishWord, italianWord, frenchWord, germanWord, englishWord);
    }
    // Insertar en el subárbol derecho si la palabra es mayor
    else if (compareResult > 0) {
        root->right = insertNode(root->right, spanishWord, italianWord, frenchWord, germanWord, englishWord);
    }
    // No permitir duplicados, simplemente devolver el nodo actual si la palabra es igual
    // Alternativamente, puedes manejar duplicados de otra manera si es necesario

    // Actualizar la altura del nodo actual
    root->height = 1 + std::max(height(root->left), height(root->right));

    // Calcular el factor de equilibrio del nodo actual
    int balance = getBalance(root);

    // Realizar rotaciones para reequilibrar el árbol si es necesario
    // Rotación a la izquierda
    if (balance > 1 && spanishWord.compare(root->left->spanishWord) < 0) {
        return rotateRight(root);
    }
    // Rotación a la derecha
    if (balance < -1 && spanishWord.compare(root->right->spanishWord) > 0) {
        return rotateLeft(root);
    }
    // Rotación doble a la izquierda
    if (balance > 1 && spanishWord.compare(root->left->spanishWord) > 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }
    // Rotación doble a la derecha
    if (balance < -1 && spanishWord.compare(root->right->spanishWord) < 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root; // Devolver el nodo actualizado
}


// Función para guardar una entrada de historial de búsqueda en un archivo
void saveSearchEntry(const SearchEntry& entry, const std::string& filename) {
    std::ofstream file(filename, std::ios::app); // Abre el archivo en modo de añadir para agregar entradas sin borrar el contenido existente
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        return;
    }

    file << entry.username << "," << entry.searchedWord << "," << entry.translation << std::endl;

    file.close();
}



// Definición de la función getHeight
int getHeight(AVLNode* node) {
    if (node == nullptr) {
        return 0;
    } else {
        return node->height;
    }
}
// Función para realizar una rotación hacia la izquierda en un árbol AVL
AVLNode* rotateRight(AVLNode* node) {
    AVLNode* newRoot = node->left;
    node->left = newRoot->right;
    newRoot->right = node;

    // Actualizar alturas
    node->height = 1 + std::max(height(node->left), height(node->right));
    newRoot->height = 1 + std::max(height(newRoot->left), height(newRoot->right));

    return newRoot;
}
// Función para realizar una rotación simple a la izquierda
AVLNode* rotateLeft(AVLNode* node) {
    AVLNode* newRoot = node->right;
    node->right = newRoot->left;
    newRoot->left = node;

    // Actualizar alturas
    node->height = 1 + std::max(height(node->left), height(node->right));
    newRoot->height = 1 + std::max(height(newRoot->left), height(newRoot->right));

    return newRoot;
}



// Función para cargar el árbol AVL desde un archivo
AVLNode* loadAVLFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo: " << filename << endl;
        return nullptr; // Devuelve nullptr si no se puede abrir el archivo
    }

    AVLNode* root = nullptr;
    std:string line;
    while (getline(file, line)) {
        stringstream ss(line);
        std::string spanishWord, italianWord, frenchWord, germanWord, englishWord;
        getline(ss, spanishWord, ',');
        getline(ss, italianWord, ',');
        getline(ss, frenchWord, ',');
        getline(ss, germanWord, ',');
        getline(ss, englishWord, ',');

        root = insertNode(root, spanishWord, italianWord, frenchWord, germanWord, englishWord);

        // Esta instrucción es solo para verificar qué se está leyendo de cada línea
        cout << "Palabras: " << spanishWord << ", " << italianWord << ", " << frenchWord << ", " << germanWord << ", " << englishWord << endl;
    }

    file.close();
    return root;
}

// Función para mostrar el menú y procesar la selección del usuario
void showMenu(AVLNode* root, string& username) {
    std::string word,traducir;
    std::string translation;
     // Historial de búsqueda del usuario

  
    
    while (true) {
        std::cout << "\nMenú:\n";
        std::cout << "1. Buscar traducción de una palabra\n";
        std::cout << "2. Salir\n";
        std::cout << "Seleccione una opción: ";
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Ingrese la palabra a buscar: ";
                std::cin >> word;
                std::cout << "Traducior a que idioma: (espanol,italiano,frances,aleman o ingles) \n";
                std::cin >> traducir;
              
                translation = searchTranslation(root, word,traducir);
               
           
                
                if (!translation.empty()) {
                    std::cout << "Traducción de '" << word << "': " << translation << std::endl;
                    SearchEntry entry;
				    entry.username = username;
				    entry.searchedWord = word;
				    entry.translation = translation;
                    // Guardar el historial de búsqueda en un archivo
				    saveSearchEntry(entry, username+"_historial.txt");
				    
				    // Encriptar la palabra
    				std::string encryptedWord = encryptWord(word);
    				
    				// Guardar la palabra original en un archivo
    				saveWordToFile(word, "original.txt");
    				
    				// Guardar la palabra encriptada en otro archivo
   					 saveWordToFile(encryptedWord, "encriptado.txt");
   					 
   					// Guardar la llave en otro archivo
    				saveWordToFile("Umg", "llave.txt"); 
				
				    std::cout << "Historial de búsqueda guardado correctamente." << std::endl;
				    std::cout << "Palabra original: " << word << std::endl;
   					std::cout << "Palabra encriptada: " << encryptedWord << std::endl;

                system("pause");
                } else {
                    std::cout << "No se encontró traducción para '" << word << "'." << std::endl;
                }
                  
                break;
            case 2:
                std::cout << "Saliendo del programa...\n";
                return;
            default:
                std::cout << "Opción inválida. Por favor, seleccione una opción válida.\n";
                break;
        }
    }
}

int main() {
    // Cargar el árbol AVL desde un archivo
    AVLNode* root = loadAVLFromFile("arbol_avl.txt");
    
    // Obtener el nombre de usuario
    std::string username;
    std::cout << "Ingrese su nombre de usuario: ";
    std::cin >> username;
    
    // Mostrar el menú principal
    showMenu(root,username);

    return 0;
}
