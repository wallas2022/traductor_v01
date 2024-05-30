#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <sys/stat.h>  // Para crear directorios en Unix/Linux
#include <direct.h>    // Para crear directorios en Windows
#include <map>
#include <curl/curl.h>
#include <cstring> 



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
std::string encryptWord(const std::string& word);
void saveUserInformation(const std::string& username, const std::string& word, const std::string& translation);
std::string decryptWord(const std::string& word);
void showTopWords(const std::string& username, int topN);
void restrictAccess(const std::string& username);
void saveKey(const std::string& keyFilename);
void createProfile(const std::string& username);
void processUserFiles(const std::string& username, const std::string& key);
bool verifyUserPassword(const std::string& username, const std::string& password);
void saveUserPassword(const std::string& username, const std::string& password);
void xorEncryptDecrypt(const std::string& key, std::string& data);
bool textToSpeech(const std::string& text, const std::string& translation, const std::string& username);
static size_t writeCallback(void* data, size_t size, size_t nmemb, FILE* fp);
void printHeader();
// Función para encriptar una palabra según las reglas dadas

std::string encryptWord(const std::string& word) {
     map<char, string> encriptacion = {
        {'a', "U1"}, {'e', "U2"}, {'i', "U3"}, {'o', "U4"}, {'u', "U5"},
        {'b', "g1"}, {'c', "g2"}, {'d', "g3"}, {'f', "g4"}, {'g', "g5"},
        {'h', "g6"}, {'j', "g7"}, {'k', "g8"}, {'l', "g9"}, {'m', "g10"},
        {'n', "g11"}, {'ñ', "g12"}, {'p', "g13"}, {'q', "g14"}, {'r', "g15"},
        {'s', "g16"}, {'t', "g17"}, {'v', "g18"}, {'w', "g19"}, {'x', "g20"},
        {'y', "g21"}, {'z', "g22"}, {'A', "g1"}, {'B', "g2"}, {'C', "g3"},
        {'D', "g4"}, {'E', "g5"}, {'F', "g6"}, {'G', "g7"}, {'H', "g8"},
        {'I', "g9"}, {'J', "g10"}, {'K', "g11"}, {'L', "g12"}, {'M', "g13"},
        {'N', "g14"}, {'Ñ', "g15"}, {'O', "g16"}, {'P', "g17"}, {'Q', "g18"},
        {'R', "g19"}, {'S', "g20"}, {'T', "g21"}, {'U', "g22"}, {'V', "g1"},
        {'W', "g2"}, {'X', "g3"}, {'Y', "g4"}, {'Z', "g5"},
        {'1', "m1"}, {'2', "m2"}, {'3', "m3"}, {'4', "m4"}, {'5', "m5"},
        {'6', "m6"}, {'7', "m7"}, {'8', "m8"}, {'9', "m9"}, {'0', "m10"}
    };

    string palabraEncriptada = "";
    for (char letra : word) {
        if (encriptacion.count(letra) > 0) {
            palabraEncriptada += encriptacion[letra];
        } else {
            palabraEncriptada += letra;
        }
    }
    return palabraEncriptada;

}



// Función para guardar una palabra en un archivo (modo de añadir)
void appendWordToFile(const std::string& word, const std::string& filename) {
    std::ofstream file(filename, std::ios::app); // Abre el archivo en modo de añadir
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
    node->height = 1 + max(height(node->left), height(node->right));
    newRoot->height = 1 + max(height(newRoot->left), height(newRoot->right));

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
    std::string line;
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
       // cout << "Palabras: " << spanishWord << ", " << italianWord << ", " << frenchWord << ", " << germanWord << ", " << englishWord << endl;
    }

    file.close();
    return root;
}




    // Revertir la encriptación de las vocales
  std::string decryptWord(const std::string& word) {
    std::map<std::string, char> desencriptacion = {
        {"U1", 'a'}, {"U2", 'e'}, {"U3", 'i'}, {"U4", 'o'}, {"U5", 'u'},
        {"g1", 'b'}, {"g2", 'c'}, {"g3", 'd'}, {"g4", 'f'}, {"g5", 'g'},
        {"g6", 'h'}, {"g7", 'j'}, {"g8", 'k'}, {"g9", 'l'}, {"g10", 'm'},
        {"g11", 'n'}, {"g12", 'ñ'}, {"g13", 'p'}, {"g14", 'q'}, {"g15", 'r'},
        {"g16", 's'}, {"g17", 't'}, {"g18", 'v'}, {"g19", 'w'}, {"g20", 'x'},
        {"g21", 'y'}, {"g22", 'z'}, {"m1", '1'}, {"m2", '2'}, {"m3", '3'},
        {"m4", '4'}, {"m5", '5'}, {"m6", '6'}, {"m7", '7'}, {"m8", '8'},
        {"m9", '9'}, {"m10", '0'}
    };

    std::string palabraDesencriptada = "";
    std::string buffer = "";
    for (char letra : word) {
        if (letra == 'U' || letra == 'g' || letra == 'm') {
            buffer = letra;
        } else {
            buffer += letra;
            if (desencriptacion.count(buffer) > 0) {
                palabraDesencriptada += desencriptacion[buffer];
                buffer = "";
            }
        }
    }
    return palabraDesencriptada;
}



void showTopWords(const string& username, int topN) {
    map<string, int> topWords;

    // Cargar las palabras buscadas del archivo encriptado.ptra
    ifstream file(username+"/encriptado.ptra");
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo: encriptado.ptra" << endl;
        return;
    }

    string word;
    while (file >> word) {
        // Desencriptar la palabra
        string decryptedWord = decryptWord(word);
        // Incrementar el contador de la palabra desencriptada
        topWords[decryptedWord]++;
    }

    file.close();

    // Ordenar el mapa por frecuencia de búsqueda
    vector<pair<string, int>> sortedWords(topWords.begin(), topWords.end());
    sort(sortedWords.begin(), sortedWords.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second;
    });

    // Mostrar el top de palabras buscadas
    cout << "Top " << topN << " palabras buscadas por " << username << ":" << endl;
    int count = 0;
    for (const auto& pair : sortedWords) {
        cout << pair.first << " - " << pair.second << " veces" << endl;
        if (++count >= topN) {
            break;
        }
    }
}




// Función para guardar la información del usuario en una carpeta con su nombre
void saveUserInformation(const std::string& username, const std::string& word, const std::string& translation) {
    // Crear una carpeta con el nombre del usuario
    std::string userDir = "./" + username;
    std::string sonidosDir = username + "/sonidos" ;
    #if defined(_WIN32)
        _mkdir(userDir.c_str());
        _mkdir(sonidosDir.c_str());
    #else 
        mkdir(userDir.c_str(), 0777);
        mkdir(sonidosDir.c_str(), 0777);
    #endif

    // Encriptar la palabra
    std::string encryptedWord = encryptWord(word);

    // Guardar la palabra original en un archivo
    appendWordToFile(word, userDir + "/original.ptra");

    // Guardar la palabra encriptada en otro archivo
    appendWordToFile(encryptedWord, userDir + "/encriptado.ptra");

    // Guardar la llave en otro archivo
    appendWordToFile("Umg", userDir + "/llave.ptra");

    // Guardar el historial de búsqueda en un archivo
    SearchEntry entry;
    entry.username = username;
    entry.searchedWord = word;
    entry.translation = translation;
    saveSearchEntry(entry, userDir + "/historial.ptra");
}


void printHeader() {
    std::cout << "+---------------------------------------+" << std::endl;
    std::cout << "|             Bienvenido a             |" << std::endl;
    std::cout << "|  Nuestra Aplicación de Traduccion    |" << std::endl;
    std::cout << "+---------------------------------------+" << std::endl;
}

// Función para mostrar el menú y procesar la selección del usuario
void showMenu(AVLNode* root, string& username) {
    std::string word, traducir;
    std::string translation;
    
    while (true) {
        printHeader();
    std::cout << "\n Hola, " << username << "" << std::endl;
    std::cout << "\n--- Menu Principal ---" << std::endl;
    std::cout << "1. Buscar traduccion de una palabra." << std::endl;
    std::cout << "2. Mostrar las palabras más buscadas." << std::endl;
    std::cout << "3. Salir." << std::endl;
    std::cout << "\nSeleccione una opcion: ";
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
            	system("clear");
                std::cout << "Ingrese la palabra a buscar: ";
                std::cin >> word;
                std::cout << "Traducir a qué idioma: (espanol, italiano, frances, aleman o ingles)\n";
                std::cin >> traducir;
              
                translation = searchTranslation(root, word, traducir);
                
                if (!translation.empty()) {
                    std::cout << "Traducción de '" << word << "': " << translation << std::endl;
                    
                    // Guardar la información del usuario en una carpeta
                    saveUserInformation(username, word, translation);

                    std::cout << "Información del usuario guardada correctamente." << std::endl;
                } else {
                    std::cout << "No se encontró traducción para '" << word << "'." << std::endl;
                }
                 if (!translation.empty()) {
                 	   // para reproducir sonido de traducción
					      if (textToSpeech(word, translation,username)) {
					        //std::cout << "Text translated to speech successfully. Output file: " << translation << ".mp3" << std::endl;
					
					        // Preguntar al usuario si desea reproducir el audio
					       // std::cout << "¿Desea escuchar sonido de traducción (si/no): ";
					       // std::string play;
					       // std::cin >> play;
					       // std::getline(std::cin, play);
					       // if (std::strcmp(play.c_str(), "si") == 0) {
					          // Reproducir el audio
					          std::string command = "start "+ username + "/sonidos/" + translation + ".mp3"; // Abrir archivo de audio en el sistema (Windows)
					         
							  system(command.c_str());
							  system("pause");
					        }
					     // } else {
					     //   std::cerr << "Error translating text to speech." << std::endl;
					     // }
					    } else {
					      std::cerr << "The translation is empty." << std::endl;
					    }                
                break;
            case 2:
            	system("clear");
                int topN;
                std::cout << "Ingrese el número de palabras más buscadas a mostrar: ";
                std::cin >> topN;
                showTopWords(username,topN);
                break;
            case 3:
            	system("clear");
                std::cout << "Saliendo del programa...\n";
                return;
            default:
                std::cout << "Opción inválida. Por favor, seleccione una opción válida.\n";
                break;
        }
    }
}

void processUserFiles(const std::string& username, const std::string& key) {
    std::vector<std::string> filenames = {"/original.ptra", "/encriptado.ptra", "/historial.ptra"};
    for (const auto& filename : filenames) {
        std::string filePath = username + filename;
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            continue;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();

        xorEncryptDecrypt(key, content);

        std::ofstream outFile(filePath, std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << "Error al abrir el archivo para escribir: " << filePath << std::endl;
            continue;
        }

        outFile << content;
        outFile.close();
    }
}
void saveUserPassword(const std::string& username, const std::string& password) {
    std::string encryptedPassword = password;
    xorEncryptDecrypt("simple_key", encryptedPassword);
    
    std::string passwordFile = username + "/llave_" + username + ".ptra";
    std::ofstream file(passwordFile);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << passwordFile << std::endl;
        return;
    }

    file << encryptedPassword << std::endl;
    file.close();
}

bool verifyUserPassword(const std::string& username, const std::string& password) {
    std::string passwordFile = username + "/llave_" + username + ".ptra";
    std::ifstream file(passwordFile);
    if (!file.is_open()) {
        return false; // No existe el archivo de contraseña
    }

    std::string storedPassword;
    std::getline(file, storedPassword);
    file.close();

    xorEncryptDecrypt("simple_key", storedPassword);

    return storedPassword == password;
}


void xorEncryptDecrypt(const std::string& key, std::string& data) {
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] ^= key[i % key.size()];
    }
}

// Function to convert text to speech using Google TTS API
bool textToSpeech(const std::string& text, const std::string& translation, const std::string& username) {
  std::string encodedText = curl_easy_escape(NULL, translation.c_str(), translation.length());
  std::string url = "https://translate.google.com/translate_tts?ie=UTF-8&q=" + encodedText + "&tl=en&client=tw-ob";

  CURL* curl = curl_easy_init();
  if (curl) {
    std::string outputFile = username+"/sonidos/"+translation + ".mp3"; // Use la traducción como nombre del archivo de audio
    FILE* fp = fopen(outputFile.c_str(), "wb");
    if (fp) {
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Ignore SSL certificate verification

      CURLcode res = curl_easy_perform(curl);
      fclose(fp);

      if (res != CURLE_OK) {
        cerr << "Error performing HTTP request: " << curl_easy_strerror(res) << endl;
        return false;
      }
    } else {
      cerr << "Error opening output file: " << outputFile << endl;
      return false;
    }

    curl_easy_cleanup(curl);
  } else {
    cerr << "Error initializing Curl." << endl;
    return false;
  }

  return true;
}

// Function to write data received from HTTP response to a file
static size_t writeCallback(void* data, size_t size, size_t nmemb, FILE* fp) {
  size_t written = fwrite(data, size, nmemb, fp);
  return written;
}


int main() {
    // Cargar el árbol AVL desde un archivo
    AVLNode* root = loadAVLFromFile("arbol_avl.txt");

     // Obtener el nombre de usuario
    std::string username;
    std::cout << "+---------------------------------------+" << std::endl;
    std::cout << "|                                       |" << std::endl;
    std::cout << "|        Bienvenido al traductor        |" << std::endl;
    std::cout << "|                                       |" << std::endl;
    std::cout << "|                                       |" << std::endl;
    std::cout << "|              Integrantes              |" << std::endl;
    std::cout << "|                                       |" << std::endl;
    std::cout << "|                                       |" << std::endl;
    std::cout << "+---------------------------------------+" << std::endl;
    std::cout << "Ingrese su nombre de usuario: ";
    std::cin >> username;

    std::string userDir = "./" + username;
    std::string password;
    
    // Verificar si el usuario ya existe
    struct stat info;
    if (stat(userDir.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
        // El usuario no existe, crear carpeta y pedir contraseña nueva
        #if defined(_WIN32)
            _mkdir(userDir.c_str());
        #else 
            mkdir(userDir.c_str(), 0777);
        #endif

        std::cout << "Ingrese una nueva clave: ";
        std::cin >> password;
        saveUserPassword(username, password);
        std::cout << "Usuario y clave creados exitosamente." << std::endl;
    } else {
        // El usuario existe, pedir contraseña y verificar
        std::cout << "Ingrese su clave: ";
        std::cin >> password;
        
        if (!verifyUserPassword(username, password)) {
            std::cerr << "Clave incorrecta. Saliendo del programa..." << std::endl;
            return 1;
        } else {
            std::cout << "Clave verificada. Bienvenido, " << username << "!" << std::endl;
        }
    }

    // Desencriptar archivos del usuario antes de trabajar con ellos
    processUserFiles(username, password);
     
      // Limpiar la consola
    system("cls || clear");
    // Mostrar el menú principal
    showMenu(root, username);

    // Encriptar archivos del usuario después de trabajar con ellos
    processUserFiles(username, password);

    return 0;
}

