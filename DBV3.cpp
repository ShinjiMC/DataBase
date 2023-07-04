#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>
#include <chrono>
#include <vector>
#include <deque>
#include <filesystem>
#include <string>
#include <functional>
#include <numeric>
#include <limits>
namespace fs = std::filesystem;

class structuras {
private:
    char Path[256];
    char name[256];
    int tamTotal;
    int cantidadAtributos;
    std::unique_ptr<int[]> tams;
    std::unique_ptr<std::string[]> names;
    std::unique_ptr<std::string[]> types;
public:
    structuras() : tamTotal(0) {}

    structuras(const char* path, const char* nombre) {
        strncpy(Path, path, sizeof(Path) - 1);
        strncpy(name, nombre, sizeof(name) - 1);
        //Contarlineas
        std::ifstream inputFile0(Path);
        if (!inputFile0) {
            std::cerr << "Error al abrir el archivo: " << Path << std::endl;
            return;
        }

        int lineCount = 0;
        char d;
        while (inputFile0.get(d)) {
            if (d == '\n') {
                lineCount++;
            }
        }
        inputFile0.clear();  // Restaurar el estado del flujo
        inputFile0.seekg(0, std::ios::beg);  // Volver al principio del archivo
        tams = std::make_unique<int[]>(lineCount);
        names = std::make_unique<std::string[]>(lineCount);
        types = std::make_unique<std::string[]>(lineCount);
        inputFile0.close();
        cantidadAtributos = lineCount;
        //Extraer valores
        std::ifstream inputFile(Path);

        if (!inputFile) {
            std::cerr << "Error al abrir el archivo: " << Path << std::endl;
            return;
        }
        int espacioTotal = 0;
        char word[256];
        char c;
        int index = 0;
        int line = 0;
        int nW = 0;
        while (inputFile.get(c)) {
            if (c == '#' || c == '\n') {
                word[index] = '\0';  // Agrega el carácter nulo al final de la palabra
                //std::cout << "Palabra: " << word << std::endl; //nombre de Variable
                if (nW == 0) {
                    if (strcmp(word, "INT") == 0) {
                        espacioTotal += 4;
                        tams[line] = 4;
                        types[line] = word;
                        nW = 2;
                    }
                    else { types[line] = word; nW++; }
                }
                else if (nW == 1) { tams[line] = std::atoi(word); espacioTotal += std::atoi(word); nW++; }
                else if (nW == 2) { names[line] = word; }
                index = 0;  // Reinicia el índice para la siguiente palabra
                if (c == '\n') { line++; nW = 0; }
            }
            else {
                word[index] = c;
                index++;
            }
        }
        tamTotal = espacioTotal;
        inputFile.close();

    }

    const char* getPath() const {
        return Path;
    }

    const char* getName() const {
        return name;
    }

    int getTamTotal() const {
        return tamTotal;
    }

    int getTamAtIndex(int index) const {
        return tams[index];
    }

    void setPath(const char* path) {
        std::strcpy(Path, path);
    }

    void setTamTotal(int totalSize) {
        tamTotal = totalSize;
        tams = std::make_unique<int[]>(tamTotal);
    }

    std::vector<std::string> getTypesVect() {
        std::vector<std::string> typesVec;

        for (int i = 0; i < cantidadAtributos; i++) {
            typesVec.push_back(types[i]);
        }

        return typesVec;
    }

    void setTamAtIndex(int index, int size) {
        tams[index] = size;
    }

    void showData() {
        for (int i = 0; i < cantidadAtributos; i++) {
            std::cout << "Tipo " << types[i] << " Tamano: " << tams[i] << " Nombre: " << names[i] << std::endl;
        }
        std::cout << "Total: " << tamTotal << std::endl;
    }

    std::unique_ptr<std::string[]>& getNames() {
        return names;
    }

    std::unique_ptr<std::string[]>& getTypes() {
        return types;
    }

    std::unique_ptr<int[]>& getValues() {
        return tams;
    }
};

class Cache {
private:
    char Path[8] = "./Cache";
    std::unique_ptr<structuras[]> estructuras;
    int cantidadEstruct;
public:
    Cache() {
        int count = 0;
        if (!fs::exists(Path)) {
            if (fs::create_directory(Path)) {
                std::cout << "Directorio creado: " << Path << std::endl;
            }
            else {
                std::cerr << "Error al crear el directorio: " << Path << std::endl;
            }
        }
        else {
            for (const auto& entry : fs::directory_iterator(Path)) {
                if (fs::is_regular_file(entry)) {
                    count++;
                }
            }
        }
        cantidadEstruct = count;
    }

    structuras& getEstructura(const char* val) const {
        for (int n = 0; cantidadEstruct > n; n++) {
            if (std::strcmp(estructuras[n].getName(), val) == 0) {
                return estructuras[n];
            }
        }
        throw std::runtime_error("No se encontró la estructura con el nombre especificado");
    }

    int getCantidadEstruct() { return cantidadEstruct; }

    void InsertDataDisk(int numPlatos, int numPistas, int numSectores, int bytesSector, int numBloques, int numSectorXBloque, int typeBuffer) {
        char DataDisk[256];
        snprintf(DataDisk, sizeof(DataDisk), "%d#%d#%d#%d#%d#%d#%d", numPlatos, numPistas, numSectores, bytesSector, numBloques, numSectorXBloque, typeBuffer);
        char PATH[13] = "./Cache/DISK";
        std::ofstream file(PATH, std::ios::trunc);
        if (file.is_open()) {
            file << DataDisk;
            file.close();
        }
        else {
            std::cerr << "Error al crear o reiniciar el archivo: " << PATH << std::endl;
        }

    }

    void Reload() {
        if (cantidadEstruct > 0) {
            estructuras = std::make_unique<structuras[]>(cantidadEstruct);
            for (int n = 0; cantidadEstruct > n; n++) {
                int currentIndex = 0;
                for (const auto& entry : fs::directory_iterator(Path)) {
                    if (entry.is_regular_file()) {
                        if (currentIndex == n) {
                            const std::string& filePath = entry.path().string();
                            char* charArray = new char[filePath.size() + 1];
                            std::strcpy(charArray, filePath.c_str());
                            const std::string& fileName = entry.path().filename().string();
                            char* charName = new char[fileName.size() + 1];
                            std::strcpy(charName, fileName.c_str());
                            if (std::strcmp(charName, "DISK") != 0) {
                                estructuras[n] = structuras(charArray, charName);
                            }
                        }
                        currentIndex++;
                    }
                }

            }
        }
    }

    void AddEstruct(const char* cadena, const char* fileName) {
        std::ofstream outputFile;
        char filePath[256];
        snprintf(filePath, sizeof(filePath), "%s/%s", Path, fileName);
        outputFile.open(filePath);

        if (outputFile.is_open()) {
            int currentIndex = 0;
            int charIndex = 0;
            char currentChar;

            while ((currentChar = cadena[charIndex]) != '\0') {
                if (currentChar == '|') {
                    outputFile << std::endl;
                    currentIndex++;
                }
                else {
                    outputFile << currentChar;
                }

                charIndex++;
            }

            outputFile.close();
            cantidadEstruct++;
            std::cout << "Archivo \"" << fileName << "\" guardado exitosamente en la carpeta \"" << Path << "\"" << std::endl;
        }
        else {
            std::cout << "Error al abrir el archivo de salida." << std::endl;
        }
        Reload();
    }

    void showDataIndex(int n) {
        if (n < cantidadEstruct) {
            estructuras[n].showData();
        }
    }
};

class Sector {
public:
    // Constructor por defecto
    Sector() {}
    // Constructor con parámetros
    Sector(int idPlato, int idSuperficie, int idPista, int idSector) {
        char Path[256];
        snprintf(Path, sizeof(Path), "./Disk/Plato_%d/Superficie_%d/Pista_%d/Sector_%d.txt", idPlato, idSuperficie, idPista, idSector);
        Path[sizeof(Path) - 1] = '\0';
        if (!fs::exists(Path)) {
            std::ofstream file(Path);
            if (file.is_open()) {
                file.close();
            }
            else {
                std::cerr << "Error al crear el archivo del Sector " << idSector << ": " << Path << std::endl;
            }
        }
    }

    ~Sector() {}
};

class Pista {
public:
    // Constructor por defecto
    Pista() {}
    // Constructor con parámetros
    Pista(int idPlato, int idSuperficie, int idPista, int numSectores) {
        char Path[256];
        snprintf(Path, sizeof(Path), "./Disk/Plato_%d/Superficie_%d/Pista_%d", idPlato, idSuperficie, idPista);
        Path[sizeof(Path) - 1] = '\0';
        if (!fs::exists(Path)) {
            if (fs::create_directory(Path)) {
                for (int i = 0; i < numSectores; i++) {
                    Sector::Sector(idPlato, idSuperficie, idPista, i);
                }
            }
            else {
                std::cerr << "Error al crear el directorio de Pista " << idPista << ": " << Path << std::endl;
            }
        }
    }

    ~Pista() {}
};

class Superficie {
public:
    // Constructor por defecto
    Superficie() {}
    // Constructor con parámetros
    Superficie(int idPlato, int idSuperficie, int numPistas, int numSectores) {
        char Path[256];
        snprintf(Path, sizeof(Path), "./Disk/Plato_%d/Superficie_%d", idPlato, idSuperficie);
        Path[sizeof(Path) - 1] = '\0';
        if (!fs::exists(Path)) {
            if (fs::create_directory(Path)) {
                for (int i = 0; i < numPistas; i++) {
                    Pista::Pista(idPlato, idSuperficie, i, numSectores);
                }
            }
            else {
                std::cerr << "Error al crear el directorio de Superficie " << idSuperficie << ": " << Path << std::endl;
            }
        }
    }

    ~Superficie() {}
};

class Plato {
public:
    // Constructor por defecto
    Plato() {}
    // Constructor con parámetros
    Plato(int idPlato, int numPistas, int numSectores) {
        char Path[256];
        snprintf(Path, sizeof(Path), "./Disk/Plato_%d", idPlato);
        Path[sizeof(Path) - 1] = '\0';
        if (!fs::exists(Path)) {
            if (fs::create_directory(Path)) {
                for (int i = 0; i < 2; i++) {
                    Superficie::Superficie(idPlato, i, numPistas, numSectores);
                }
            }
            else {
                std::cerr << "Error al crear el directorio de Plato " << idPlato << ": " << Path << std::endl;
            }
        }
    }

    ~Plato() {}
};

class StackPlatos {
public:
    // Constructor por defecto
    StackPlatos() {}
    // Constructor con parámetros
    StackPlatos(int numPlatos, int numPistas, int numSectores) {
        if (!fs::exists("./Disk")) {
            if (fs::create_directory("./Disk")) {
                for (int i = 0; i < numPlatos; i++) {
                    Plato::Plato(i, numPistas, numSectores);
                }
            }
            else {
                std::cerr << "Error al crear el directorio principal del Disco: ./Disk" << std::endl;
            }
        }
    }

    ~StackPlatos() {}
};

class Block {
    std::string dataBlock;
    std::vector<std::pair<int, int>> cabeceraBlock;
    std::vector<std::string> pathSectores;
    std::vector<int> numRegistosPart;
    int idBloque, numSectorXBloque, capacidadBytes, BytesOcupados;
public:
    // Constructor por defecto
    Block() : idBloque(0), numSectorXBloque(0), capacidadBytes(0), BytesOcupados(0) {}

    std::string GetSectPATH(int indexBloque, int numPlatos, int numSectores, int numPistas) {
        int i = indexBloque;
        int Pn = 0, Sn = 0, Pin = 0, Sectn = 0;
        while (i != 0) {
            if (Sn == 2) { Pn++; Sn = 0; }
            if (Pn == numPlatos) { Sectn++; Pn = 0; }
            if (Sectn == numSectores) { Pin++; Sectn = 0; }
            if (Pin == numPistas) { std::cout << "No existe en el Disco " << std::endl; }
            i--;
            if (i == 0) { break; }
            else { Sn++; }
        }
        std::string pathSect = "./Disk/Plato_" + std::to_string(Pn) + "/Superficie_" + std::to_string(Sn) + "/Pista_" + std::to_string(Pin) + "/Sector_" + std::to_string(Sectn) + ".txt";
        return pathSect;
    }

    // Constructor con parámetros
    Block(int idBloque, int numSectoresXBloque, int numPlatos, int numSectores, int numPistas, int BytesXSector)
        : capacidadBytes(BytesXSector* numSectoresXBloque), idBloque(idBloque), numSectorXBloque(numSectoresXBloque) {
        int index = idBloque * numSectorXBloque;
        BytesOcupados = 0;
        std::vector<int> numRegPart2(numSectoresXBloque, 0);
        numRegistosPart = numRegPart2;
        int IndicadorFoV = 0;
        for (int i = 1; i <= numSectoresXBloque; i++) {
            //OBTENER PATH DE SECTOR


            int n = 0;
            std::string path = GetSectPATH(index + i, numPlatos, numSectores, numPistas);
            pathSectores.push_back(path);
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (file.is_open()) {
                std::string firstLine, restOfFile;
                std::streamsize fileSize = file.tellg(); // TAMANIO DEL FILE
                file.seekg(0, std::ios::beg); //UBICARSE AL INICIO DEL TXT
                if (fileSize != 0) {
                    std::getline(file, firstLine);//LEE LA PRIMERA LINEA DEL TXT (CABECERA) 
                    //cabeceraBlock += firstLine;
                    if (i == 1 && firstLine[0] == '0' && firstLine[1] == ',') {
                        IndicadorFoV = 1;
                        std::string pairStr;
                        //std::cout << "Variable 1" << std::endl;
                        size_t start = 0;
                        size_t end = firstLine.find('#');
                        n -= 1;
                        while (end != std::string::npos) {
                            pairStr = firstLine.substr(start, end - start);
                            size_t commaPos = pairStr.find(',');

                            if (commaPos != std::string::npos) {
                                int secondNum = std::stoi(pairStr.substr(commaPos + 1));
                                int firstNum = 0;
                                if (pairStr.substr(0, commaPos) == "00-1") {
                                    firstNum = -1;
                                }
                                else {
                                    firstNum = std::stoi(pairStr.substr(0, commaPos));
                                }

                                cabeceraBlock.push_back(std::make_pair(firstNum, secondNum));
                                n++;
                            }

                            start = end + 1;
                            end = firstLine.find('#', start);
                        }

                        restOfFile.resize(fileSize - firstLine.size());
                        file.read(&restOfFile[0], restOfFile.size());// GUARDA EL CONTENIDO EN EL BLOQUE SIN CONSIDERAR LA PRIMERA LINEA
                        BytesOcupados += fileSize; // BYTES OCUPADOS EN EL BLOQUE
                        size_t pos = restOfFile.find('\0');

                        // Eliminar el carácter nulo si se encuentra
                        if (pos != std::string::npos) {
                            restOfFile.erase(pos);
                        }
                        std::string registroV;
                        size_t startPos = 0;
                        size_t pipePos;

                        while ((pipePos = restOfFile.find("|", startPos)) != std::string::npos) {
                            std::string extractedString = restOfFile.substr(startPos, pipePos - startPos + 1);
                            dataBlock = extractedString + dataBlock;
                            startPos = pipePos + 1;
                        }
                        std::string lastString = restOfFile.substr(startPos);
                        dataBlock = lastString + dataBlock;

                    }
                    else if (IndicadorFoV == 1) {
                        std::string pairStr;
                        //std::cout << "Variable 2" << std::endl;
                        size_t start = 0;
                        size_t end = firstLine.find('#');

                        while (end != std::string::npos) {
                            pairStr = firstLine.substr(start, end - start);
                            size_t commaPos = pairStr.find(',');

                            if (commaPos != std::string::npos) {
                                int secondNum = std::stoi(pairStr.substr(commaPos + 1));
                                int firstNum = 0;
                                if (pairStr.substr(0, commaPos) == "00-1") {
                                    firstNum = -1;
                                }
                                else {
                                    firstNum = std::stoi(pairStr.substr(0, commaPos));
                                }

                                cabeceraBlock.push_back(std::make_pair(firstNum, secondNum));
                                n++;
                            }

                            start = end + 1;
                            end = firstLine.find('#', start);
                        }

                        restOfFile.resize(fileSize - firstLine.size());
                        file.read(&restOfFile[0], restOfFile.size());// GUARDA EL CONTENIDO EN EL BLOQUE SIN CONSIDERAR LA PRIMERA LINEA
                        BytesOcupados += fileSize; // BYTES OCUPADOS EN EL BLOQUE
                        size_t pos = restOfFile.find('\0');

                        // Eliminar el carácter nulo si se encuentra
                        if (pos != std::string::npos) {
                            restOfFile.erase(pos);
                        }
                        std::string registroV;
                        size_t startPos = 0;
                        size_t pipePos;

                        while ((pipePos = restOfFile.find("|", startPos)) != std::string::npos) {
                            std::string extractedString = restOfFile.substr(startPos, pipePos - startPos + 1);
                            dataBlock = extractedString + dataBlock;
                            startPos = pipePos + 1;
                        }
                        std::string lastString = restOfFile.substr(startPos);
                        dataBlock = lastString + dataBlock;
                    }
                    if (IndicadorFoV == 0) {
                        //std::cout <<"Linea cabecera : "<< firstLine<<std::endl;
                        std::string pairStr;
                        //std::cout << "Fija " << std::endl;
                        size_t start = 0;
                        size_t end = firstLine.find('#');

                        while (end != std::string::npos) {
                            pairStr = firstLine.substr(start, end - start);
                            size_t commaPos = pairStr.find(',');

                            if (commaPos != std::string::npos) {
                                int secondNum = std::stoi(pairStr.substr(commaPos + 1));
                                int firstNum = 0;
                                if (pairStr.substr(0, commaPos) == "00-1") {
                                    firstNum = -1;
                                }
                                else {
                                    firstNum = std::stoi(pairStr.substr(0, commaPos));
                                }

                                cabeceraBlock.push_back(std::make_pair(firstNum, secondNum));
                                n++;
                            }

                            start = end + 1;
                            end = firstLine.find('#', start);
                        }

                        restOfFile.resize(fileSize - firstLine.size());
                        file.read(&restOfFile[0], restOfFile.size());// GUARDA EL CONTENIDO EN EL BLOQUE SIN CONSIDERAR LA PRIMERA LINEA
                        BytesOcupados += fileSize; // BYTES OCUPADOS EN EL BLOQUE
                        size_t pos = restOfFile.find('\0');

                        // Eliminar el carácter nulo si se encuentra
                        if (pos != std::string::npos) {
                            restOfFile.erase(pos);
                        }
                        dataBlock += restOfFile;
                    }


                }
                file.close();
            }

            else {
                std::cerr << "Error al abrir el archivo: " << path << std::endl;
            }
            numRegistosPart[i - 1] = n;
        }
    }

    void Bloque_Sector() {
        std::vector<std::pair<int, int>> cabecera = cabeceraBlock;
        int bytesSector = capacidadBytes / numSectorXBloque; // OBTENER LA CAPACIDAD DE BYTES POR SECTOR 
        size_t pos = 0;
        int bytes_registro_sector = 2;
        std::string cabeceraSect, dataSect;
        int nSect = 0;
        std::string data = dataBlock;
        if (!cabecera.empty() && cabecera[0].first == 0) {
            int spaceLibre = 0;
            int n = 0;
            //std::cout <<"Nums: "<< numSectorXBloque << std::endl;
            for (int j = 0; j < numSectorXBloque; j++) {
                int cantidadRegs = numRegistosPart[j] + n;
                std::string cabeceraIng = "", dataIng = "";
                if (j == 0) {
                    std::pair<int, int> cabReg = cabecera[0];
                    std::string aux1 = std::to_string(cabReg.first);
                    std::string valor2 = std::to_string(cabReg.second);
                    std::string cabTMP = aux1 + "," + valor2 + "#";
                    cabeceraIng += cabTMP;
                    spaceLibre = (cabReg.second + 1);
                    cabecera.erase(cabecera.begin());
                }
                for (int i = n; i < cantidadRegs; i++) {
                    std::pair<int, int> cabReg = cabecera[i];
                    //NUM REGISTRO
                    std::string aux1 = std::to_string(cabReg.first);
                    std::string valor1;
                    for (int i = 0; i < (4 - aux1.length()); i++) {
                        valor1 += "0";
                    }
                    valor1 += aux1;
                    // POSICION 
                    std::string valor2 = std::to_string(cabReg.second);
                    //FORMAR CABECERA QUE SE INSERTAR[A EN EL TXT
                    std::string cabTMP = valor1 + "," + valor2 + "#";
                    cabeceraIng += cabTMP;

                    int positionK = cabReg.second - spaceLibre;
                    //std::cout << "Posicion de Reg " << valor1 << " : " << positionK << std::endl;
                    std::string substring = "";
                    size_t pipePos = data.find("|", positionK);
                    if (pipePos != std::string::npos) {
                        substring = data.substr(positionK, pipePos - positionK + 1);
                    }
                    dataIng += substring;

                }
                /*std::cout << "Path: " << pathSectores[j] << std::endl;
                std::cout << "Informacion: " << std::endl;
                std::cout << cabeceraIng << std::endl;
                std::cout << dataIng << std::endl;*/
                std::fstream file(pathSectores[j], std::ios::out);
                if (file.is_open()) {
                    file.clear();
                    if (!cabeceraIng.empty()) {
                        file << cabeceraIng;//ESCRIBE LA CABECERA EN EL SECTOR
                        file << '\n';
                        file << dataIng;// ESCRIBE EL CONTENIDO DE REGISTROS EN EL SECTOR SEGUN CORRESPONDA 
                    }
                    file.close();
                }
                else {
                    std::cerr << "Error al abrir el archivo: " << pathSectores[j] << std::endl;
                }
                n = numRegistosPart[j] + n;
            }
        }
        else {
            //--------------------        
            //std::string cabecera = cabeceraBlock;
            //std::cout < cabecera<<std::endl;
            //std::cout << dataBlock;

            int n = 0;
            //std::cout <<"Nums: "<< numSectorXBloque << std::endl;
            for (int j = 0; j < numSectorXBloque; j++) {
                int cantidadRegs = numRegistosPart[j] + n;
                std::string cabeceraIng = "", dataIng = "";
                for (int i = n; i < cantidadRegs; i++) {
                    std::pair<int, int> cabReg = cabecera[i];
                    //NUM REGISTRO
                    std::string aux1 = std::to_string(cabReg.first);
                    std::string valor1;
                    for (int i = 0; i < (4 - aux1.length()); i++) {
                        valor1 += "0";
                    }
                    valor1 += aux1;
                    // POSICION 
                    std::string valor2 = std::to_string(cabReg.second);
                    //FORMAR CABECERA QUE SE INSERTAR[A EN EL TXT
                    std::string cabTMP = valor1 + "," + valor2 + "#";
                    cabeceraIng += cabTMP;


                    std::string substring = "";
                    size_t pipePos = data.find("|", cabReg.second);
                    if (pipePos != std::string::npos) {
                        substring = data.substr(cabReg.second, pipePos - cabReg.second + 1);
                    }
                    dataIng += substring;

                }
                /*std::cout <<"Path: "<< pathSectores[j] << std::endl;
                std::cout << "Informacion: " << std::endl;
                std::cout << cabeceraIng << std::endl;
                std::cout << dataIng << std::endl;*/
                std::fstream file(pathSectores[j], std::ios::out);
                if (file.is_open()) {
                    file.clear();
                    if (!cabeceraIng.empty()) {
                        file << cabeceraIng;//ESCRIBE LA CABECERA EN EL SECTOR
                        file << '\n';
                        file << dataIng;// ESCRIBE EL CONTENIDO DE REGISTROS EN EL SECTOR SEGUN CORRESPONDA 
                    }
                    file.close();
                }
                else {
                    std::cerr << "Error al abrir el archivo: " << pathSectores[j] << std::endl;
                }
                n = numRegistosPart[j] + n;
            }
        }

    }

    void InsertarTodo(std::string data, std::vector<std::pair<int, int>> cabec, std::vector<int> parts) {
        dataBlock = data;
        cabeceraBlock = cabec;
        numRegistosPart = parts;
        int count = 0;
        //obtener tamanio de cabecera
        for (const auto& par : cabec) {
            //std::string strFirst = std::to_string(par.first);
            std::string strSecond = std::to_string(par.second);
            count += strSecond.length();
            count += 6;  // 4 bytes predeterminados y 2 "," y "#"
        }
        BytesOcupados = 2 + data.length() + count;
        Bloque_Sector(); // GUARDAR DATOS DEL BLOQUE A LOS SECTORES 
    }

    void ShowSectores() {
        std::cout << "Sectores: " << std::endl;
        // IMPRIME LAS RUTAS DE LOS SECTORES CORRESPONDIENTES AL BLOQUE
        for (const std::string& path : pathSectores) {
            std::cout << "- " << path << std::endl;
        }
    }

    void showBloque() {
        //IMPRIME CABECERA
        std::cout << "Cantidad de Regitros: " << std::accumulate(numRegistosPart.begin(), numRegistosPart.end(), 0) << std::endl;
        std::cout << "Cabecera: " << std::endl;
        for (const auto& par : cabeceraBlock) {
            std::cout << par.first << "," << par.second << "#";
        }
        std::cout << std::endl;
        //std::cout << cabeceraBlock << std::endl;
        //IMPRIME DATOS DEL BLOQUE
        std::cout << "Registros: " << std::endl;
        std::cout << dataBlock << std::endl;
    }

    int getBytesOcupados() {
        return BytesOcupados;
    }

    std::string getData() {
        return dataBlock;
    }

    std::vector<std::pair<int, int>> getCabecera() {
        return cabeceraBlock;
    }

    std::vector<int> getNumRegvect() {
        return numRegistosPart;
    }

    ~Block() {}
};

class StackBlocks {
private:
    std::vector<Block> bloques;
    int numBloques;
public:
    // Constructor por defecto
    StackBlocks() : numBloques(0) {}
    // Constructor con parámetros
    StackBlocks(int numBloques, int numSectoresXBloque, int numPlatos, int numSectores, int numPistas, int BytesXSector) : numBloques(numBloques) {
        if (fs::exists("./Disk")) {
            for (int i = 0; i < numBloques; i++) {
                Block tmp = Block(i, numSectoresXBloque, numPlatos, numSectores, numPistas, BytesXSector);
                bloques.push_back(tmp);
            }
        }
        else {
            std::cerr << "Error: No existe Disco creado previamente." << std::endl;
        }
    }

    void InsertData(std::string data, std::vector<std::pair<int, int>> cabec, std::vector<int> parts, int n) {
        bloques[n].InsertarTodo(data, cabec, parts);
    }

    std::vector<Block>& getBloques() {
        return bloques;
    }

    int getNumBloques() const {
        return numBloques;
    }

    void ImprimirBloque(int N) {
        std::cout << "Bloque " << N << ":" << std::endl;
        bloques[N].showBloque();
    }

    std::vector<int> getVectNumReg(int n) {
        return bloques[n].getNumRegvect();
    }

    std::string getDataN(int n) {
        return bloques[n].getData();
    }

    std::vector<std::pair<int, int>> getCabeceraN(int n) {
        return bloques[n].getCabecera();
    }

    int getBytesOcupadosN(int n) {
        return bloques[n].getBytesOcupados();
    }

    ~StackBlocks() {}
};

class HardDisk {
private:
    StackBlocks bloques;
    int numBloques, numSectoresXBloque, numPlatos, numPistas, numSectores, numBytes;
public:
    // Constructor por defecto
    HardDisk() : numSectoresXBloque(0), numPlatos(0), numPistas(0), numSectores(0) {}
    // Constructor por parametros
    HardDisk(int numPlatos, int numPistas, int numSectores, int numBytes, int numSectoresXBloque)
        : numSectoresXBloque(numSectoresXBloque), numPlatos(numPlatos), numPistas(numPistas), numSectores(numSectores), numBytes(numBytes) {
        numBloques = ((numPlatos * 2 * numPistas * numSectores) / numSectoresXBloque);
        StackPlatos::StackPlatos(numPlatos, numPistas, numSectores);
        bloques = StackBlocks(numBloques, numSectoresXBloque, numPlatos, numSectores, numPistas, numBytes);
    }

    ~HardDisk() {}

    const char* GetSectPATH(int indexSector) {
        int i = indexSector;
        int Pn = 0, Sn = 0, Pin = 0, Sectn = 0;
        while (i != 0) {
            if (Sn == 2) { Pn++; Sn = 0; }
            if (Pn == numPlatos) { Sectn++; Pn = 0; }
            if (Sectn == numSectores) { Pin++; Sectn = 0; }
            if (Pin == numPistas) { std::cout << "No existe en el Disco " << std::endl; }
            i--;
            if (i == 0) { break; }
            else { Sn++; }
        }
        char Path[7] = "./Disk";
        static char filePath[256];
        snprintf(filePath, sizeof(filePath), "%s/Plato_%d/Superficie_%d/Pista_%d/Sector_%d.txt", Path, Pn, Sn, Pin, Sectn);
        return filePath;
    }

    void ShowSectPATH(int n) {
        int i = n;
        int Pn = 0, Sn = 0, Pin = 0, Sectn = 0;
        while (i != 0) {
            if (Sn == 2) { Pn++; Sn = 0; }
            if (Pn == numPlatos) { Sectn++; Pn = 0; }
            if (Sectn == numSectores) { Pin++; Sectn = 0; }
            if (Pin == numPistas) { std::cout << "No existe en el Disco " << std::endl; }
            i--;
            if (i == 0) { break; }
            else { Sn++; }
        }
        char Path[7] = "./Disk";
        static char filePath[256];
        snprintf(filePath, sizeof(filePath), "%s/Plato_%d/Superficie_%d/Pista_%d/Sector_%d.txt", Path, Pn, Sn, Pin, Sectn);
        std::cout << "\t\tPlato:" << Pn << "\t" << "Superficie: " << Sn << "\t" << "Pista: " << Pin << "\t" << "Sector: " << Sectn << std::endl;
        std::cout << "\t\tPath: " << filePath << std::endl;
    }

    void ShowSectXBloq(int indBloq) {
        bloques.getBloques()[indBloq].ShowSectores();
    }

    void InsertBlock(std::string data, std::vector<std::pair<int, int>> cabecera, std::vector <int> parts, int n) {
        bloques.InsertData(data, cabecera, parts, n);
    }

    int getBytesBloq() {
        return (numBytes * numSectoresXBloque);
    }

    int getBytesBloqOcupados(int n) {
        return bloques.getBytesOcupadosN(n);
    }

    int getBytesSectOcupados(int n) {
        const char* path = GetSectPATH(n);
        std::ifstream inputFile(path, std::ios::binary | std::ios::ate);
        if (!inputFile) {
            std::cerr << "Error al abrir el archivo Bloque: " << path << std::endl;
            return -1;
        }
        std::streampos fileSize = inputFile.tellg();
        inputFile.close();
        return static_cast<int>(fileSize);
    }

    void ShowBloq(int n) {
        bloques.ImprimirBloque(n);
    }

    void ShowSectByBloq(int n) {
        ShowSectXBloq(n);
    }

    // Getter para el atributo 'numSectoresXBloque'
    int getNumSectoresXBloque() const { return numSectoresXBloque; }

    // Getter para el atributo 'numPlatos'
    int getNumPlatos() const {
        return numPlatos;
    }

    // Getter para el atributo 'numPistas'
    int getNumPistas() const {
        return numPistas;
    }

    // Getter para el atributo 'numSectores'
    int getNumSectores() const {
        return numSectores;
    }

    // Getter para el atributo 'numBloques'
    int getNumBloques() const {
        return numBloques;
    }

    // Getter para el atributo 'numBytes'
    int getNumBytes() const {
        return numBytes;
    }



    StackBlocks getStackBlock() {
        return bloques;
    }
};

class Page {
private:
    std::string data;
    std::vector <int> numRegPart;
    std::vector<std::pair<int, int>> cabecera;
    int index;
    int bytes;
    int bytesOcupados;
    bool referenceBit;
public:
    // Constructor por defecto
    Page() : index(0), bytes(0), bytesOcupados(0), referenceBit(false) {}

    void InsertCabecera2(int numReg, int pos) {
        std::pair<int, int> tmp = std::make_pair(numReg, pos);
        cabecera.push_back(tmp);
    }

    void InsertData(char content) {
        data += content;
        bytesOcupados++;
    }
    
    bool getReferenceBit() const {
        return referenceBit;
    }

    void setReferenceBit(bool bit) {
        referenceBit = bit;
    }

    // Constructor con parámetros
    Page(int idx, int numBytes, int BytesOcup, std::string dat, std::vector<std::pair<int, int>> cabec, std::vector <int> tmp)
        : index(idx), bytes(numBytes), bytesOcupados(BytesOcup), referenceBit(false) {
        numRegPart = tmp;
        cabecera = cabec;

        data = dat;
    }

    ~Page() {}

    std::string& getDataR() {
        return data;
    }
    // Getter para el atributo 'data'
    const std::string& getData() const {
        return data;
    }

    //
    std::vector<std::pair<int, int>>& getCabeceraR() {
        return cabecera;
    }

    const std::vector<std::pair<int, int>> getCabecera() const {
        return cabecera;
    }
    // Getter para el atributo 'index'
    int getIndex() const {
        return index;
    }

    // Getter para el atributo 'bytes'
    int getBytes() const {
        return bytes;
    }

    // Getter para el atributo 'bytesOcupados'
    int getBytesOcupados() const {
        return bytesOcupados;
    }

    int getNumregT() const {
        return std::accumulate(numRegPart.begin(), numRegPart.end(), 0);
    }

    void replaceData(std::string n) {
        data = n;
    }

    std::vector <int> getRegPart() {
        return numRegPart;
    }
    std::vector <int>& getRegPartR() {//para sobreescribir datos
        return numRegPart;
    }

    void Insert(int numReg, int pos, std::string registros_conca, int bytes_regis, int idPart) {
        std::pair<int, int> tmp = std::make_pair(numReg, pos);
        cabecera.push_back(tmp);
        data += registros_conca;
        bytesOcupados += bytes_regis;
        numRegPart[idPart] = numRegPart[idPart] + 1;
    }
    void InsertVar(int numReg, int pos, std::string registros_conca, int bytes_regis, int idPart) {
        std::pair<int, int> tmp = std::make_pair(numReg, pos);
        cabecera.push_back(tmp);
        data = registros_conca + data;
        bytesOcupados += bytes_regis;
        numRegPart[idPart] = numRegPart[idPart] + 1;
    }
    void InsertVarFE(int pos) {
        std::pair<int, int> tmp = std::make_pair(0, pos);
        cabecera.insert(cabecera.begin(), tmp);
        std::string cabFreeSpace = ("0," + std::to_string(pos) + "#");
        bytesOcupados += cabFreeSpace.length();
    }

    int getSpacePageVar(int MAXREG) {
        int numFillRegs = 0;
        if (cabecera.size() > 0 && cabecera[0].first == 0) {
            numFillRegs = (cabecera[0].second / numRegPart.size()) / MAXREG;
            //Aca falta aplicar diccionarios, extrayendo desde el nombre de tabla ubicado en esa pagina, 
        }
        return numFillRegs;
    }

    int getSpacePage() {
        int numFillRegs = 0;
        for (const auto& par : cabecera) {
            if (par.first == -1) {
                numFillRegs++;
            }
        }
        return numFillRegs;
    }

    int getFirstRegs() {
        int numFillRegs = -1;
        if (cabecera.size() > 0) {
            numFillRegs = cabecera[0].first;
            //Aca falta aplicar diccionarios, extrayendo desde el nombre de tabla ubicado en esa pagina, 
        }
        return numFillRegs;
    }

    int getFirstReg() {
        if (!cabecera.empty()) {
            return cabecera.front().first;
        }
        return -1;
    }

    int getLastReg() {
        if (!cabecera.empty()) {
            return cabecera.back().first;
        }
        return -1;
    }


    int getPart(int n) {
        int suma = 0;
        int parte = 0;

        for (int i = 0; i < numRegPart.size(); i++) {
            suma += numRegPart[i];
            if (n <= cabecera[suma - 1].first) {
                parte = i;
                break;
            }
        }
        return parte;
    }

    int numBytesforPart(int n) {//Segun el numero de registro te retorna la capacidadOcupada de esta parte de pagina (sector)
        int parte = getPart(n);
        std::vector<std::pair<int, int>> tmp;
        for (int i = numRegPart[parte - 1]; i < numRegPart[parte] + numRegPart[parte - 1]; i++) {
            tmp.push_back(cabecera[i]);
        }
        int bytesIndice = 4, bytesdd = 0;
        for (int i = 0; i < tmp.size(); i++) {
            bytesdd += (2 + std::to_string(tmp[i].second).length() + bytesIndice);
        }
        bytesdd += 2;
        int inicio = tmp[0].second;
        int fin = tmp.back().second + 1;
        size_t siguienteBarra = data.find("|", fin);
        if (siguienteBarra != std::string::npos) {
            fin = static_cast<int>(siguienteBarra);
        }
        int cantidadBytes = fin - inicio;
        return cantidadBytes + bytesdd;
    }
    int getPartSize() { return bytes / numRegPart.size(); }
};

class BufferPool {
private:
    std::vector<Page> frames;
    std::vector<Page> Cache;
    StackBlocks bloquesC;
    int numFrames, numPaginas, numBytes, typeofBuffer;
public:
    // Constructor por defecto
    BufferPool() : numFrames(0), numPaginas(0), numBytes(0), typeofBuffer(0) {}
    // Constructor con parámetros
    int straInt(std::string cadena) {
        int num = 0;
        if (cadena == "00-1") {
            num = -1;
        }
        else {
            num = stoi(cadena);
        }
        return num;
    }

    bool isIndexFrames(int n) {
        for (const Page& page : frames) {
            if (page.getIndex() == n) {
                return false; // El índice n ya existe en frames
            }
        }
        return true; // El índice n no se encontró en frames
    }
    
    bool isIndexCache(int n) {
        for (const Page& page : Cache) {
            if (page.getIndex() == n) {
                return false; // El índice n ya existe en cache
            }
        }
        return true; // El índice n no se encontró en cache
    }

    int getIndexFromFrame(int n) {
        auto it = std::find_if(frames.begin(), frames.end(), [n](const Page& page) {
            return page.getIndex() == n;
            });

        if (it != frames.end()) {
            // El elemento con getIndex() igual a n fue encontrado en Cache
            int index = std::distance(frames.begin(), it);
            return index;
        }

        return -1;  // Si no se encuentra, retornar -1 o un valor que indique que no se encontró
    }

    void LRU(int n) {//LRU
        if (n >= 0 && n < numPaginas) {
            if (isIndexFrames(n)) { // No está en vector
                if (isIndexCache(n)) { // No está en Cache
                    if (frames.size() == numFrames) { // Se llenó en frames
                        Cache.push_back(frames.front());
                        frames.erase(frames.begin());
                    }
                    std::vector<std::pair<int, int>> CabeceraVector = bloquesC.getCabeceraN(n);
                    Page tmp = Page(n, numBytes, bloquesC.getBytesOcupadosN(n), bloquesC.getDataN(n), CabeceraVector, bloquesC.getVectNumReg(n));
                    frames.push_back(tmp);
                }
                else { // Está en cache
                    auto found = false;
                    for (size_t i = 0; i < Cache.size(); ++i) {
                        if (Cache[i].getIndex() == n) {
                            found = true;
                            if (frames.size() == numFrames) { // Se llenó en frames
                                Cache.push_back(frames.front());
                                frames.erase(frames.begin());
                            }
                            Page tmp(Cache[i]);
                            frames.push_back(tmp);
                            Cache.erase(Cache.begin() + i);
                            break;
                        }
                    }
                    if (!found) {
                        std::cout << "ERROR: Página no encontrada en cache" << std::endl;
                    }
                }
            }
            else { // Está en vector
                auto it = std::find_if(frames.begin(), frames.end(), [&](const Page& page) {
                    return page.getIndex() == n;
                    });

                if (it != frames.end()) {
                    std::rotate(it, it + 1, frames.end());
                }
            }
        }
        else {
            std::cout << "Página no encontrada" << std::endl;
            return;
        }
      std::cout << "Contenido del buffer de páginas:" << std::endl;
      for (const auto& page : frames) {
          std::cout << "Página ID: " << page.getIndex() << std::endl;
          //std::cout << "Página ID: " << page.getData() << std::endl;
          // Imprimir otros atributos de la página según sea necesario
      }
      std::cout << "Contenido del buffer de cache:" << std::endl;
      for (const auto& page : Cache) {
          std::cout << "Página ID: " << page.getIndex() << std::endl;
          //std::cout << "Página ID: " << page.getData() << std::endl;
          // Imprimir otros atributos de la página según sea necesario
      }
    }

    void MRU(int n) {
        if (n >= 0 && n < numPaginas) {
            if (isIndexFrames(n)) {
                if (isIndexCache(n)) {
                    if (frames.size() == numFrames) {
                        Cache.push_back(frames.back());
                        frames.pop_back();
                    }
                    std::vector<std::pair<int, int>> CabeceraVector = bloquesC.getCabeceraN(n);
                    Page tmp = Page(n, numBytes, bloquesC.getBytesOcupadosN(n), bloquesC.getDataN(n), CabeceraVector, bloquesC.getVectNumReg(n));
                    frames.push_back(tmp);
                }
                else {
                    size_t it = 0;
                    while (it < Cache.size()) {
                        if (Cache[it].getIndex() == n) {
                            if (frames.size() == numFrames) {
                                Cache.push_back(frames.back());
                                frames.pop_back();
                            }
                            frames.push_back(Cache[it]);
                            Cache.erase(Cache.begin() + it);
                            break;
                        }
                        ++it;
                    }
                }
            }
            else {
                size_t it = 0;
                while (it < frames.size()) {
                    if (frames[it].getIndex() == n) {
                        std::rotate(frames.begin() + it, frames.begin() + it + 1, frames.end());
                        break;
                    }
                    ++it;
                }
            }
        }
        else {
            std::cout << "Página no encontrada" << std::endl;
            return;
        }
        std::cout << "Contenido del buffer de páginas:" << std::endl;
          for (const auto& page : frames) {
              std::cout << "Página ID: " << page.getIndex() << std::endl;
              //std::cout << "Página ID: " << page.getData() << std::endl;
              // Imprimir otros atributos de la página según sea necesario
          }
          std::cout << "Contenido del buffer de cache:" << std::endl;
          for (const auto& page : Cache) {
              std::cout << "Página ID: " << page.getIndex() << std::endl;
              //std::cout << "Página ID: " << page.getData() << std::endl;
              // Imprimir otros atributos de la página según sea necesario
          }
    }

    void CLOCK(int n) {
        int hand = 0;
        if (n >= 0 && n < numPaginas) {
            if (isIndexFrames(n)) { // no está en el frame
                if (isIndexCache(n)) { // no está en la caché 
                    if (frames.size() == numFrames) { // se llenó frames
                        bool allReferencesTrue = true;
                        for (const auto& page : frames) {
                            if (!page.getReferenceBit()) {
                                allReferencesTrue = false;
                                break;
                            }
                        }
                        while (true) {
                            if (!frames[hand].getReferenceBit()) { // buscar el primero que es 0
                                std::vector<std::pair<int, int>> CabeceraVector = bloquesC.getCabeceraN(n);
                                Page tmp(n, numBytes, bloquesC.getBytesOcupadosN(n), bloquesC.getDataN(n), CabeceraVector, bloquesC.getVectNumReg(n));
                                Cache.push_back(frames[hand]);
                                frames[hand] = tmp;
                                frames[hand].setReferenceBit(true);
                                hand = (hand + 1) % frames.size();
                                break;
                            }
                            else {
                                if (!allReferencesTrue) {
                                    hand = (hand + 1) % frames.size();
                                }
                                else {
                                    frames[hand].setReferenceBit(false);
                                    hand = (hand + 1) % frames.size();
                                }
                            }
                        }
                    }
                    else {
                        std::vector<std::pair<int, int>> CabeceraVector = bloquesC.getCabeceraN(n);
                        Page tmp(n, numBytes, bloquesC.getBytesOcupadosN(n), bloquesC.getDataN(n), CabeceraVector, bloquesC.getVectNumReg(n));
                        tmp.setReferenceBit(true);
                        frames.push_back(tmp); // Agregar la nueva página al final de los frames
                    }
                }
                else { // está en la caché
                    bool found = false;
                    size_t indexToRemove = 0;
                    for (size_t i = 0; i < Cache.size(); ++i) {
                        if (Cache[i].getIndex() == n) {
                            found = true;
                            if (frames.size() == numFrames) { // Se llenó frames
                                bool allReferencesTrue = true;
                                for (const auto& page : frames) {
                                    if (!page.getReferenceBit()) {
                                        allReferencesTrue = false;
                                        break;
                                    }
                                }
                                while (true) {
                                    if (!frames[hand].getReferenceBit()) { // buscar el primero que es 0
                                        Page tmp(Cache[i]);
                                        Cache.push_back(frames[hand]);
                                        frames[hand] = tmp;
                                        frames[hand].setReferenceBit(true);
                                        hand = (hand + 1) % frames.size();
                                        break;
                                    }
                                    else {
                                        if (!allReferencesTrue) {
                                            hand = (hand + 1) % frames.size();
                                        }
                                        else {
                                            frames[hand].setReferenceBit(false);
                                            hand = (hand + 1) % frames.size();
                                        }
                                    }
                                }
                            }
                            indexToRemove = i;
                        }
                    }
                    if (found) {
                        Cache.erase(Cache.begin() + indexToRemove);
                    }
                    else {
                        std::cout << "ERROR: Página no encontrada en cache" << std::endl;
                    }
                }
            }
            else { // está en frame
                auto it = std::find_if(frames.begin(), frames.end(), [&](const Page& page) {
                    return page.getIndex() == n;
                    });
                if (it != frames.end()) {
                    it->setReferenceBit(true);
                }
            }
        }
        std::cout << "Contenido del buffer de páginas:" << std::endl;
        for (const auto& page : frames) {
            if (page.getIndex() != -1) {
                std::cout << "Página ID: " << page.getIndex() << " Bit de referencia: " << page.getReferenceBit() << std::endl;
            }
        }
        std::cout << std::endl;
        std::cout << "Contenido del buffer de cache:" << std::endl;
        for (const auto& page : Cache) {
            if (page.getIndex() != -1) {
                std::cout << "Página ID: " << page.getIndex() << " Bit de referencia: " << page.getReferenceBit() << std::endl;
            }
        }
        std::cout << std::endl;
    }

    void InsertPagetoBuffer(int n) {
        if (typeofBuffer == 1) {
            LRU(n);
        }
        else if (typeofBuffer == 2) {
            MRU(n);
        }
        else if (typeofBuffer == 3) {
            CLOCK(n);
        }
    }

    int getIndexFrames(int n) {
        int index = -1;
        for (int i = 0; i < frames.size(); i++) {
            if (frames[i].getIndex() == n) {
                index = i;
            }
        }
        return index;
    }


    BufferPool(int numPaginas, int numFrames, int numBytes, StackBlocks bloques, int typeofBuffers) : numFrames(numFrames), numPaginas(numPaginas), numBytes(numBytes) {
        bloquesC = bloques;
        typeofBuffer = typeofBuffers;
        /*for (int i = 0; i < numPaginas; i++) {
            std::vector<std::pair<int, int>> CabeceraVector = bloques.getCabeceraN(i);
            Page tmp = Page(i, numBytes, bloques.getBytesOcupadosN(i), bloques.getDataN(i), CabeceraVector, bloques.getVectNumReg(i));
            frames.push_back(tmp);
        }*/
    }
    
    ~BufferPool() {}

    void insertDataPage(char content, int nPage) {
        if (nPage >= 0 && nPage < numPaginas) {
            InsertPagetoBuffer(nPage);
            //int id = getIndexFromFrame(nPage);
            frames[getIndexFrames(nPage)].InsertData(content);
        }
        else {
            std::cerr << "Error: Pagina no encontrada." << nPage << std::endl;
        }
    }

    // Getter para el atributo 'pagina'
    Page getPageAtIndex(int n) {
        if (n >= 0 && n < numPaginas) {
            InsertPagetoBuffer(n);
            return frames[getIndexFrames(n)];
        }
        else {
            std::cerr << "Error: Índice de página no válido: " << n << std::endl;
            // Devuelve una página inválida o lanza una excepción según tus necesidades
            return Page();
        }
    }

    int getBytesbyIndex(int n) {
        if (n >= 0 && n < numPaginas) {
            InsertPagetoBuffer(n);
            return frames[getIndexFrames(n)].getBytes();
        }
        else {
            std::cerr << "Error: Índice de página no válido: " << n << std::endl;
            // Devuelve una página inválida o lanza una excepción según tus necesidades
            return -1;
        }
    }

    int getBytesOcupadosbyIndex(int n) {
        if (n >= 0 && n < numPaginas) {
            InsertPagetoBuffer(n);
            return frames[getIndexFrames(n)].getBytesOcupados();
        }
        else {
            std::cerr << "Error: Índice de página no válido: " << n << std::endl;
            // Devuelve una página inválida o lanza una excepción según tus necesidades
            return -1;
        }
    }

    void insertCabeceraReg(int nPage, int numReg, int pos) {
        if (nPage >= 0 && nPage < numPaginas) {
            InsertPagetoBuffer(nPage);
            frames[getIndexFrames(nPage)].InsertCabecera2(numReg, pos);
        }
        else {
            std::cerr << "Error: Pagina no encontrada." << nPage << std::endl;
        }


    }

    // Getter para el atributo 'paginas'
    const std::vector<Page>& getPages() const {
        return frames;
    }

    std::vector<Page>& gettingPages() {
        return frames;
    }

    std::vector<std::pair<int, int>> getCabeceraN(int n) {
        InsertPagetoBuffer(n);
        return frames[getIndexFrames(n)].getCabecera();
    }

    std::vector<int> getRegPartN(int n) {
        InsertPagetoBuffer(n);
        return frames[getIndexFrames(n)].getRegPart();
    }

    std::string getDataN(int n) {
        InsertPagetoBuffer(n);
        return frames[getIndexFrames(n)].getData();
    }

    int getNumRegTotal(int n) {
        InsertPagetoBuffer(n);
        return frames[getIndexFrames(n)].getNumregT();
    }

    // Getter para el atributo 'numPaginas'
    int getNumPages() const {
        return numPaginas;
    }

    int getNumRegTotalY(int n) {//Ya cargado n en frames
        return frames[getIndexFrames(n)].getNumregT();
    }

    std::string getDataNY(int n) {//Ya cargado n en frames
        return frames[getIndexFrames(n)].getData();
    }

    void showCabecera(int n) {
        const std::vector<std::pair<int, int>> data = frames[getIndexFrames(n)].getCabecera();
        for (const auto& par : data) {
            std::cout << par.first << "," << par.second << "#";
        }
        std::cout << std::endl;
    }

    void showPage(int n) {
        InsertPagetoBuffer(n);
        std::cout << "Pagina " << n << ": " << std::endl;
        std::cout << "Bytes Total: " << frames[getIndexFrames(n)].getBytes() << std::endl;
        std::cout << "Bytes Ocupados: " << frames[getIndexFrames(n)].getBytesOcupados() << std::endl;
        std::cout << "Cabecera: " << std::endl;
        showCabecera(n);
        std::cout << "Cantidad de Registros: " << std::endl;
        std::cout << getNumRegTotalY(n) << std::endl;
        std::cout << "Registros: " << std::endl;
        std::cout << getDataNY(n) << std::endl;
    }

    void ADDReg(int numReg, int pos, std::string registros_conca, int bytes_regis, int inde, int idPart) {
        InsertPagetoBuffer(inde);
        frames[getIndexFrames(inde)].Insert(numReg, pos, registros_conca, bytes_regis, idPart);
    }
    
    void ADDRegVar(int numReg, int pos, std::string registros_conca, int bytes_regis, int inde, int idPart) {
        InsertPagetoBuffer(inde);
        frames[getIndexFrames(inde)].InsertVar(numReg, pos, registros_conca, bytes_regis, idPart);
    }
    
    void ADDFREESPACE(int pos, int inde) {
        InsertPagetoBuffer(inde);
        frames[getIndexFrames(inde)].InsertVarFE(pos);
    }

    int ValidateVariable() {
        int numPag = 1;
        InsertPagetoBuffer(0);
        int j = frames[getIndexFrames(0)].getFirstRegs();
        if (j == 0) {
            numPag = 0;
        }
        return numPag;
    }

    int freeSpaceMappingVar(int MAXREG) {
        int numPag = -1;
        int val = 0;
        for (int i = 0; i < numPaginas; i++) {
            InsertPagetoBuffer(i);
            int j = frames[getIndexFrames(i)].getSpacePageVar(MAXREG);
            if (j > val) {
                numPag = i;
            }
        }
        return numPag;
    }

    int freeSpaceMapping() {
        int numPag = -1;
        int val = 0;
        for (int i = 0; i < numPaginas; i++) {
            InsertPagetoBuffer(i);
            int j = frames[getIndexFrames(i)].getSpacePage();
            if (j > val) {
                numPag = i;
            }
        }
        return numPag;
    }

    int Page_complete() {
        int numPag = -1;
        int val = 0;
        for (int i = 0; i < numPaginas; i++) {
            InsertPagetoBuffer(i);
            int j = frames[getIndexFrames(i)].getSpacePage();
            if (j > val) {
                numPag = i;
            }
        }
        return numPag;
    }

    void FillBlankReg(std::string& cadena, int nPosicion) {//eliminar registro de cadena llenandolo por espacio vacio
        size_t inicio = nPosicion;
        size_t fin = cadena.find("|", inicio + 1);
        //std::cout << "reemplazar pos: " << inicio << "-" << fin << std::endl;

        if (inicio != std::string::npos && fin != std::string::npos) {
            cadena.replace(inicio, fin - inicio, fin - inicio, ' ');
        }
    }

    int buscarNumeroEnPrimerPar(const std::vector<std::pair<int, int>>& vectorPares, int numero) {//retorna pos del n registro
        auto it = std::find_if(vectorPares.begin(), vectorPares.end(), [numero](const std::pair<int, int>& par) {
            return par.first == numero;
            });

        if (it != vectorPares.end()) {
            return it->second;
        }
        return -1;
    }

    int ubicar_ultimo_registro(std::vector<std::pair<int, int>>& vectorPares) {
        if (!vectorPares.empty()) {
            int ultimo_registro = vectorPares.back().first;
            return ultimo_registro;
        }
        else {
            std::cout << "El vector está vacío." << std::endl;
        }
    }

    void ChangeCabecera(std::vector<std::pair<int, int>>& vectorPares, int valor, int Nvalor) {//se cambia a nValor
        auto it = std::find_if(vectorPares.begin(), vectorPares.end(), [valor](std::pair<int, int>& par) {
            return par.second == valor;
            });

        if (it != vectorPares.end()) {
            it->first = Nvalor;
        }
    }

    void Add_Cabecera(std::vector<std::pair<int, int>>& vectorPares, int registro, int posicion_inicio) {
        vectorPares.push_back(std::make_pair(registro, posicion_inicio));
    }
    
    void Add_CabeceraVAR(std::vector<std::pair<int, int>>& vectorPares, int registro, int posicion_inicio, int tam) {
        vectorPares.push_back(std::make_pair(registro, posicion_inicio));
        vectorPares[0].second -= tam;
    }

    void Add_CabeceraVAR2(std::vector<std::pair<int, int>>& vectorPares, int registro, int posicion_inicio) {
        vectorPares.push_back(std::make_pair(registro, posicion_inicio));
    }

    void AddFreeSpace(std::vector<std::pair<int, int>>& vectorPares,int pos, int numReg, int pos2) {
        vectorPares.push_back(std::make_pair(0, pos));
        vectorPares.push_back(std::make_pair(numReg, pos2));
    }

    void EliminarReg(int numRegE) {
        for (int i = 0; i < numPaginas; i++) {
            InsertPagetoBuffer(i);

            int val = buscarNumeroEnPrimerPar(frames[getIndexFrames(i)].getCabecera(), numRegE);
            //std::cout << "Elminar: " << val << std::endl;
            if (val != -1) {
                ChangeCabecera(frames[getIndexFrames(i)].getCabeceraR(), val, -1);
                FillBlankReg(frames[getIndexFrames(i)].getDataR(), val);
                //frames[i].UpdateBytesOcupados();
                return;
            }
        }
        std::cout << "El registro " << numRegE << " no existe " << std::endl;
    }

  

    bool FindReginCabecera(const std::vector<std::pair<int, int>>& vectorPares, int n) {
        return std::any_of(vectorPares.begin(), vectorPares.end(), [n](const std::pair<int, int>& par) {
            return par.first == n;
            });
    }

    int MaxNumReg() {
        int maxValue = std::numeric_limits<int>::min();
        for (int i = 0; i < numPaginas; i++) {
            InsertPagetoBuffer(i);
            for (const auto& par : frames[getIndexFrames(i)].getCabecera()) {
                maxValue = std::max(maxValue, par.first);
            }
        }
        return maxValue;
    }

    void InsertRegistro(std::string& data, std::string reg, int pos) {
        data.replace(pos, reg.length(), reg);

    }

    void InsertarRegistroUltimo(std::string& data, std::string reg, int pos) {
        //std::cout << "POSICION: " << pos << std::endl;
        data.replace(pos, reg.length(), reg);
        data += "|";

    }

    void InsertarRegistroUltimoVAR(std::string& data, std::string reg) {
        //std::cout << "POSICION: " << pos << std::endl;
        data = reg + data;

    }

    void InsertarRegistroUltimo2(std::string& data, std::string reg) {
        size_t lastPipePos = data.find_last_of("|");
        if (lastPipePos != std::string::npos) {
            data = data.substr(0, lastPipePos + 1) + reg;
        }
        else {
            data += reg;
        }

    }

    int SelectlastPageFill() {
        int id = -1; //
        for (int i = 0; i < numPaginas; i++) {
            InsertPagetoBuffer(i);
            if (frames[getIndexFrames(i)].getData().empty()) {
                id = i - 1;
                break;
            }
            else if (i == numPaginas - 1) {
                id = i;
            }
        }
        return id;
    }
    
    int obtenerUltimoIndiceNoCero(const std::vector<int>& vec) {
        for (int i = vec.size() - 1; i >= 0; --i) {
            if (vec[i] != 0) {
                return i;
            }
        }
        return 0; // Si todos los elementos son cero, retorna -1
    }
    
    int obtnerBytesParte(int n, const std::string& data, const std::vector <int>& numRegPart, const std::vector<std::pair<int, int>>& cabecera) {
        // Verificar si el índice n es válido
        int longitud = data.length();
        if (data.empty()) {
            return 0;
        }
        else {
            if (n < 0 || n >= numRegPart.size()) {
                std::cout << "El índice n es inválido." << std::endl;
                return -1;
            }

            // Obtener la posición inicial (I) y final (F) de la cabecera
            int posInicial = 0;
            int posFinal = 0;

            for (int i = 0; i <= n; ++i) {
                if (i == n) {
                    posInicial = posFinal;
                }
                posFinal += numRegPart[i];
            }

            // Verificar si las posiciones son válidas
            if (posInicial < 0 || posInicial >= cabecera.size() || posFinal < 0 || posFinal > cabecera.size()) {
                std::cout << "Las posiciones de la cabecera son inválidas." << std::endl;
                return -1;
            }

            // Obtener el rango en data y obtener el tamaño
            int inicio = cabecera[posInicial].second;
            int fin = cabecera[posFinal - 1].second;

            while (fin < longitud && data[fin] != '|') {
                fin++;
            }
            fin++;
            //std::cout << "Las posiciones son: " << inicio << " - " << fin << std::endl;
            std::string parteData = data.substr(inicio, fin - inicio);
            int tamano = parteData.length();
            int tamanoCabecera = 0;
            for (int i = posInicial; i < posFinal; ++i) {
                std::string segundoPar = std::to_string(cabecera[i].second);
                tamanoCabecera += segundoPar.length() + 6;  // Sumar 6 por la coma los primeros 4 bytes del primer par y 2 por # y ,
            }

            tamano += tamanoCabecera + 2;
            return tamano;
        }

    }

    void Changestring(std::string& cadena, int nPosicion, std::string reg) {//Poner un reg dentro de la cadena segun pos
        size_t inicio = nPosicion;
        size_t fin = cadena.find("|", inicio + 1);
        //std::cout << "reemplazar pos: " << inicio << "-" << fin << std::endl;

        if (inicio != std::string::npos && fin != std::string::npos) {
            cadena.replace(inicio, fin - inicio, reg);
        }
    }

    void MoverRegs(int n1, int n2) {
        int idPag1 = -1, idPag2 = -1;
        for (int i = 0; i < numPaginas; i++) {
            InsertPagetoBuffer(i);
            if (FindReginCabecera(frames[getIndexFrames(i)].getCabecera(), n1)) {
                idPag1 = i;
            }
            if (FindReginCabecera(frames[getIndexFrames(i)].getCabecera(), n2)) {
                idPag2 = i;
            }
        }
        if (idPag1 != -1 && idPag2 != -1) {
            std::string extracted1, extracted2;
            InsertPagetoBuffer(idPag1);
            int pos1 = buscarNumeroEnPrimerPar(frames[getIndexFrames(idPag1)].getCabecera(), n1);
            std::string data1 = frames[getIndexFrames(idPag1)].getData();
            std::size_t startPos1 = pos1; // Posición inicial para la búsqueda
            std::size_t endPos1 = data1.find('|', startPos1); // Posición donde se encuentra el carácter '|' después de startPos

            if (endPos1 != std::string::npos) {
                extracted1 = data1.substr(startPos1, endPos1 - startPos1);
            }
            InsertPagetoBuffer(idPag2);
            int pos2 = buscarNumeroEnPrimerPar(frames[getIndexFrames(idPag2)].getCabecera(), n2);
            std::string data2 = frames[getIndexFrames(idPag2)].getData();
            std::size_t startPos2 = pos2; // Posición inicial para la búsqueda
            std::size_t endPos2 = data1.find('|', startPos2); // Posición donde se encuentra el carácter '|' después de startPos
            if (endPos2 != std::string::npos) {
                extracted2 = data2.substr(startPos2, endPos2 - startPos2);
            }
            Changestring(frames[getIndexFrames(idPag2)].getDataR(), pos2, extracted1);
            ChangeCabecera(frames[getIndexFrames(idPag2)].getCabeceraR(), pos2, n1);
            InsertPagetoBuffer(idPag1);
            Changestring(frames[getIndexFrames(idPag1)].getDataR(), pos1, extracted2);
            ChangeCabecera(frames[getIndexFrames(idPag1)].getCabeceraR(), pos1, n2);
        }
        else {
            if (idPag1 == -1) {
                std::cout << "Registro " << n1 << " no se encuentra en la tabla." << std::endl;
            }
            if (idPag2 == -2) {
                std::cout << "Registro " << n2 << " no se encuentra en la tabla." << std::endl;
            }
        }

    }

    void BuscarRegs(int n1) {
        int idPag1 = -1;
        for (int i = 0; i < numPaginas; i++) {
            InsertPagetoBuffer(i);
            if (FindReginCabecera(frames[getIndexFrames(i)].getCabecera(), n1)) {
                idPag1 = i;
            }

         
        }
        //std::cout << idPag1;
        if (idPag1 != -1 ) {
            std::string extracted1;
            InsertPagetoBuffer(idPag1);
            int pos1 = buscarNumeroEnPrimerPar(frames[getIndexFrames(idPag1)].getCabecera(), n1);
            if (frames[getIndexFrames(idPag1)].getCabecera()[0].first == 0) {
                pos1 = pos1 - frames[getIndexFrames(idPag1)].getCabecera()[0].second - 1;
            }
            
            std::string data1 = frames[getIndexFrames(idPag1)].getData();
            std::size_t startPos1 = pos1; // Posición inicial para la búsqueda
            std::size_t endPos1 = data1.find('|', startPos1); // Posición donde se encuentra el carácter '|' después de startPos
            if (endPos1 != std::string::npos) {
                extracted1 = data1.substr(startPos1, endPos1 - startPos1);
            }
            std::cout << "Posicion Inicio: " << startPos1 << std::endl;
            std::cout << "Posicion Final: " << endPos1 << std::endl;
            std::cout << "Registro se encuentra en la pagina: " << idPag1 << std::endl;
            std::cout << extracted1 << std::endl;
            //std::cout << "Registro se encuentra en el sector: " << frames[idPag1]. << std::endl;
        }
        else {
            if (idPag1 == -1) {
                std::cout << "Registro " << n1 << " no se encuentra en la tabla." << std::endl;
            }
		}/* if (frames.back().getCabecera()[0].first==0) {
				pos1 = pos1 - frames.back().getCabecera()[0].second-1;
			}
			*/
    }
    
    void AddNewRegVar(int MAXREG, int numRegN, std::string registro) {
        int idPag = freeSpaceMappingVar(MAXREG); //ENCONTRAR LA PAGINA QUE TIENE ESPACIO PARA INSERTAR
        for (int i = 0; i < numPaginas; i++) {
            InsertPagetoBuffer(i);
            if (FindReginCabecera(frames[getIndexFrames(i)].getCabecera(), numRegN)) {
                numRegN = MaxNumReg() + 1;
                break;
            }
        }
        std::cout << "ID Pagina: " << idPag << std::endl;
        std::cout << "ID Registro: " << numRegN << std::endl;
        std::cout << "Registro: " << registro << std::endl;
        if (idPag == -1) {
            std::cout << "El Disco ya no acepta más registros" << std::endl;
            return;
        }
        else {
            int idSect = 0;
            InsertPagetoBuffer(idPag);
            for (int i = 0; i < frames[getIndexFrames(idPag)].getRegPart().size(); i++) {
                if (frames[getIndexFrames(idPag)].getRegPart()[i] != 0) {
                    idSect = i;
                }
            }
            std::cout << "Parte Sect " << idSect << std::endl;
            if (idSect == 0) {
                int cantRegs = frames[getIndexFrames(idPag)].getRegPart()[idSect];
                if (cantRegs == 0) {//Pagina Vacia

                }
                else {
                    int bytesOcupadosSect = 2;
                    std::string ccabecera1;
                    std::string data1 = frames[getIndexFrames(idPag)].getData();
                    int space = 0;
                    const std::vector<std::pair<int, int>>& cabecera = frames[getIndexFrames(idPag)].getCabecera();
                    ccabecera1 = "0," + std::to_string(cabecera[0].second) + "#";
                    space = cabecera[0].second + 1;
                    bytesOcupadosSect += ccabecera1.length();
                    for (int nu = 1; nu < cantRegs + 1; nu++) {
                        int pos1 = cabecera[nu].second - space;
                        std::size_t startPos1 = pos1; // Posición inicial para la búsqueda
                        std::size_t endPos1 = data1.find('|', startPos1) + 1; // Posición donde se encuentra el carácter '|' después de startPos
                        std::string extracted;
                        if (endPos1 != std::string::npos) {
                            extracted = data1.substr(startPos1, endPos1 - startPos1);
                        }

                        bytesOcupadosSect += (6 + (std::to_string(cabecera[nu].second).length()));
                        bytesOcupadosSect += extracted.length();

                    }
                    std::string posiblePos = std::to_string(cabecera[1].second - registro.length());
                    int bytesNewReg = registro.length() + 6 + posiblePos.length();
                    if (bytesOcupadosSect + bytesNewReg <= frames[getIndexFrames(idPag)].getPartSize()) {
                        Add_CabeceraVAR(frames[getIndexFrames(idPag)].getCabeceraR(), numRegN, cabecera[1].second - registro.length(), registro.length());
                        InsertarRegistroUltimoVAR(frames[getIndexFrames(idPag)].getDataR(), registro);
                        frames[getIndexFrames(idPag)].getRegPartR()[idSect] = frames[getIndexFrames(idPag)].getRegPartR()[idSect] + 1;
                    }
                    else {
                        idSect++;
                        if (idSect == frames[getIndexFrames(idPag)].getRegPart().size()) {
                            idPag++;
                            InsertPagetoBuffer(idPag);
                            idSect = 0;
                            AddFreeSpace(frames[getIndexFrames(idPag)].getCabeceraR(), frames[getIndexFrames(idPag)].getPartSize() - registro.length() - 1,numRegN, frames[getIndexFrames(idPag)].getPartSize() - registro.length());
                            InsertarRegistroUltimoVAR(frames[getIndexFrames(idPag)].getDataR(), registro);
                            frames[getIndexFrames(idPag)].getRegPartR()[idSect] = frames[getIndexFrames(idPag)].getRegPartR()[idSect] + 1;
                        }
                        else {
                            Add_CabeceraVAR2(frames[getIndexFrames(idPag)].getCabeceraR(), numRegN, cabecera.back().second - registro.length());
                            InsertarRegistroUltimoVAR(frames[getIndexFrames(idPag)].getDataR(), registro);
                            frames[getIndexFrames(idPag)].getRegPartR()[idSect] = frames[getIndexFrames(idPag)].getRegPartR()[idSect] + 1;
                        }
                    }

                }
            }
            else {
                int cantRegs = frames[getIndexFrames(idPag)].getRegPart()[idSect];
                int bytesOcupadosSect = 2;
                std::string ccabecera1;
                std::string data1 = frames[getIndexFrames(idPag)].getData();
                int space = 0;
                const std::vector<std::pair<int, int>>& cabecera = frames[getIndexFrames(idPag)].getCabecera();
                space = cabecera[0].second + 1;
                bytesOcupadosSect += ccabecera1.length();
                for (int nu = 1; nu < cantRegs + 1; nu++) {
                    int pos1 = cabecera[nu].second - space;
                    std::size_t startPos1 = pos1; // Posición inicial para la búsqueda
                    std::size_t endPos1 = data1.find('|', startPos1) + 1; // Posición donde se encuentra el carácter '|' después de startPos
                    std::string extracted;
                    if (endPos1 != std::string::npos) {
                        extracted = data1.substr(startPos1, endPos1 - startPos1);
                    }

                    bytesOcupadosSect += (6 + (std::to_string(cabecera[nu].second).length()));
                    bytesOcupadosSect += extracted.length();

                }
                std::string posiblePos = std::to_string(cabecera[1].second - registro.length());
                int bytesNewReg = registro.length() + 6 + posiblePos.length();
            }
        }
    }
    
    void AddNewRegFija(int numRegN, std::string registro) {
        int idPag = freeSpaceMapping(); //ENCONTRAR LA PAGINA QUE TIENE ESPACIO PARA INSERTAR
        for (int i = 0; i < numPaginas; i++) {
            InsertPagetoBuffer(i);
            if (FindReginCabecera(frames[getIndexFrames(i)].getCabecera(), numRegN)) {
                numRegN = MaxNumReg() + 1;
                break;
            }
        }
        if (idPag != -1) { // SI HAY ESPACIO LIBRE DENTRO DE LA TABLA
            InsertPagetoBuffer(idPag);
            int val = buscarNumeroEnPrimerPar(frames[getIndexFrames(idPag)].getCabecera(), -1); // RETORNAR LA POSICION DE  REGISTRO LIBRE
            ChangeCabecera(frames[getIndexFrames(idPag)].getCabeceraR(), val, numRegN); // MODIFICAR CABECERA
            InsertRegistro(frames[getIndexFrames(idPag)].getDataR(), registro, val); //INSERTA EL REGISTRO
            //frames[idPag].UpdateBytesOcupados(); // ACTUALIZA LA CAPACIDAD DE BYTES DE LA PAGINA
        }
        else { // NO HAY ESPACIO LIBRE
            idPag = SelectlastPageFill(); // Ultima p[agina llena o completada  // 16
            if (idPag == -1) {
                std::cout << "El Disco ya no puede aceptar más registros con esa longitud" << std::endl;
            }
            else {
                int BytesOcupadostmp = 0;
                InsertPagetoBuffer(idPag);
                const std::vector<std::pair<int, int>>& cabecera = frames[getIndexFrames(idPag)].getCabecera();



                //int num_new_regis = ubicar_ultimo_registro(frames[idPag].getCabeceraR()) + 1;
                int num_new_regis = numRegN;
                int posNEW = 0;
                size_t lastPipePos = frames[getIndexFrames(idPag)].getData().find_last_of("|");
                if (lastPipePos != std::string::npos) {
                    posNEW = static_cast<int>(lastPipePos) + 1;
                }
                else {
                    std::cerr << "----Error----" << std::endl;
                }
                std::string CabezaTMP = std::to_string(num_new_regis) + "," + std::to_string(posNEW) + "#";
                //std::cout << "CabezaTMP:" << CabezaTMP << std::endl;
                BytesOcupadostmp = CabezaTMP.length() + registro.length();
                //std::cout << "Bytes Ocupados: " << BytesOcupadostmp;
                //std::cout << "PAGINA :" << idPag << std::endl;
                int bytesxPart = frames[getIndexFrames(idPag)].getPartSize();
                //Saber cual es el ultimo valor agregado en el vector int partes, es decir si hay 23,0,0 debe decir que es el parte 1. si hay 23,12,3 debe ser la parte 3
                //despues calcular cuantos bytesocupados tiene esa parte, luego saber que bytesxPart es el limite de un sector y saber si entra en el sector, en caso 
                //contraro pasarlo a otro sector o sino a la siguiente pagina. al agregar a uno debe sumar en el vector con esa parte +1 del registro.

                /*int contador = std::count_if(firstLine.begin(), firstLine.end(), std::bind(std::equal_to<char>(), std::placeholders::_1, '#'));
                numRegistosPart[i - 1] = contador;*/
                int idParte = obtenerUltimoIndiceNoCero(frames[getIndexFrames(idPag)].getRegPart());
                int bytesOcupadosxPart = obtnerBytesParte(idParte, frames[getIndexFrames(idPag)].getData(), frames[getIndexFrames(idPag)].getRegPart(), frames[getIndexFrames(idPag)].getCabecera());
                //std::cout << "Parte Sect de Pagina:" << idParte << std::endl;
                //std::cout << "bytesocupados:" << bytesOcupadosxPart << std::endl;

                if ((bytesOcupadosxPart + BytesOcupadostmp) <= bytesxPart) {
                    Add_Cabecera(frames[getIndexFrames(idPag)].getCabeceraR(), num_new_regis, posNEW);
                    InsertarRegistroUltimo(frames[getIndexFrames(idPag)].getDataR(), registro, posNEW);
                    frames[getIndexFrames(idPag)].getRegPartR()[idParte] = frames[getIndexFrames(idPag)].getRegPartR()[idParte] + 1;
                }
                else {
                    if (idParte + 1 == frames[getIndexFrames(idPag)].getRegPart().size()) {
                        idPag++;
                        if (idPag == numPaginas) {
                            std::cout << "El Disco ya no acepta más registros" << std::endl;
                            return;
                        }
                        InsertPagetoBuffer(idPag);
                        idParte = 0;
                        Add_Cabecera(frames[getIndexFrames(idPag)].getCabeceraR(), num_new_regis, 0);
                        InsertarRegistroUltimo(frames[getIndexFrames(idPag)].getDataR(), registro, 0);
                        frames[getIndexFrames(idPag)].getRegPartR()[idParte] = frames[getIndexFrames(idPag)].getRegPartR()[idParte] + 1;
                    }
                    else {
                        idParte++;
                        Add_Cabecera(frames[getIndexFrames(idPag)].getCabeceraR(), num_new_regis, posNEW);
                        InsertarRegistroUltimo(frames[getIndexFrames(idPag)].getDataR(), registro, posNEW);
                        frames[getIndexFrames(idPag)].getRegPartR()[idParte] = frames[getIndexFrames(idPag)].getRegPartR()[idParte] + 1;
                    }
                }
            }
        }
    }
};

class BufferManager {
private:
    Cache diccionarios;
    BufferPool AllPages;
    HardDisk Disco;
public:
    BufferManager() {
        diccionarios = Cache();
        diccionarios.Reload();
    }
    BufferManager(int numBytes, int numPages) {
        diccionarios = Cache();
        diccionarios.Reload();
    }

    ~BufferManager() {}

    const BufferPool& getAllPages() const {
        return AllPages;
    }

    void MakeDisco() {
        std::vector<std::string> carpetas = { "./Dir","./disk" };
        for (const auto& carpeta : carpetas) {
            if (fs::exists(carpeta)) {
                try {
                    fs::remove_all(carpeta);
                }
                catch (const std::exception& e) {
                    std::cerr << "Error al eliminar la carpeta: " << carpeta << std::endl;
                    std::cerr << e.what() << std::endl;
                }
            }
        }
        int numPlatos = 0, numPistas = 0, numSectores = 0, numBytes = 0, numSectoresXBloque = 0, typeofBuffer = -1;
        std::cout << "Ingrese la cantidad de Platos: ";
        std::cin >> numPlatos;
        std::cout << "\nIngrese la cantidad de Pistas por Superficie: ";
        std::cin >> numPistas;
        std::cout << "\nIngrese la cantidad de Sectores por Pista: ";
        std::cin >> numSectores;
        std::cout << "\nIngrese la cantidad de Bytes por Sector: ";
        std::cin >> numBytes;
        std::cout << "\nIngrese la cantidad de Sectores por Bloque: ";
        std::cin >> numSectoresXBloque;
        std::cout << "\nEscoja la estrategia de reemplazo \n\t1. LRU\n\t2. MRU \n\t3. Clock \nIngrese el numero segun la eleccion:";
        std::cin >> typeofBuffer;

        if (typeofBuffer == 1) {
            std::cout << "Elegiste LRU" << std::endl;
        }
        else if (typeofBuffer == 2) {
            std::cout << "Elegiste MRU" << std::endl;
        }
        else if (typeofBuffer == 3) {
            std::cout << "Elegiste Clock" << std::endl;
        }
        else {
            std::cout << "Opción no válida. Se selecciona LRU por defecto." << std::endl;
            typeofBuffer = 1; // Establecer LRU como opción predeterminada
        }
        diccionarios = Cache();
        diccionarios.Reload();
        diccionarios.InsertDataDisk(numPlatos, numPistas, numSectores, numBytes, (numPlatos * numPistas * numSectores * 2) / numSectoresXBloque, numSectoresXBloque, typeofBuffer);
        Disco = HardDisk(numPlatos, numPistas, numSectores, numBytes, numSectoresXBloque);
        AllPages = BufferPool((numPlatos * numPistas * numSectores * 2) / numSectoresXBloque, 6, numBytes * numSectoresXBloque, Disco.getStackBlock(), typeofBuffer);
    }

    void LoadDisk() {
        int numPlatos = 0, numPistas = 0, numSectores = 0, bytesSector = 0, numBloques = 0, numSectorXBloque = 0, typeofBuffer = -1;
        std::ifstream inputFile("./Cache/DISK");
        if (!inputFile) {
            std::cerr << "Error al abrir el archivo: " << "./Cache/DISK" << std::endl;
            return;
        }
        char valor[256];
        int contador = 0;
        while (inputFile.getline(valor, sizeof(valor), '#')) {
            switch (contador) {
            case 0:
                numPlatos = std::atoi(valor);
                break;
            case 1:
                numPistas = std::atoi(valor);
                break;
            case 2:
                numSectores = std::atoi(valor);
                break;
            case 3:
                bytesSector = std::atoi(valor);
                break;
            case 4:
                numBloques = std::atoi(valor);
                break;
            case 5:
                numSectorXBloque = std::atoi(valor);
                break;
            case 6:
                typeofBuffer = std::atoi(valor);
                break;
            default:
                break;
            }
            contador++;
        }

        inputFile.close();
        diccionarios = Cache();
        diccionarios.Reload();
        Disco = HardDisk(numPlatos, numPistas, numSectores, bytesSector, numSectorXBloque);
        AllPages = BufferPool(numBloques, 6, bytesSector * numSectorXBloque, Disco.getStackBlock(), typeofBuffer);
    }

    void InsertStruct(const char* content, const char* filename) {
        diccionarios.AddEstruct(content, filename);
    }

    void CargarArchivo(const char* filename) {
        diccionarios = Cache();
        diccionarios.Reload();
        const char extensionTXT[] = ".txt";
        std::size_t indicePunto = std::strcspn(filename, ".");
        std::size_t longitudMaxima = 256;
        char nombre2TXT[256];
        if (indicePunto < std::strlen(filename)) {
            std::strncpy(nombre2TXT, filename, indicePunto);
            nombre2TXT[indicePunto] = '\0';
        }
        else {
            std::strncpy(nombre2TXT, filename, longitudMaxima);
            nombre2TXT[longitudMaxima - 1] = '\0';
        }
        char nombreTXT[] = "new.txt";

        std::ifstream entrada(filename);
        if (!entrada) {
            std::cout << "No se pudo abrir el archivo." << std::endl;
            return;
        }
        std::ofstream salida(nombreTXT);
        std::string linea;
        char c;

        // Ignorar la primera línea del archivo CSV
        entrada.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Procesar las líneas restantes
        bool dentroDeComillas = false;
        while (entrada.get(c)) {
            if (c == '\n') {
                // Escribir la línea procesada en el archivo de salida
                salida << linea << '\n';
                linea.clear();
            }
            else {
                size_t pos = linea.size();


                // Reemplazar comas por #
                if (c == ',') {
                    // Verificar si estamos dentro de una cadena entre comillas
                    if (dentroDeComillas) {
                        linea.push_back(c);
                    }
                    else {
                        linea.push_back('#');
                    }
                }
                else if (c == '"') {
                    // Cambiar el estado de dentroDeComillas
                    dentroDeComillas = !dentroDeComillas;
                    linea.push_back(c);
                }
                else {
                    linea.push_back(c);
                }
            }
        }

        // Escribir la última línea procesada en el archivo de salida
        if (!linea.empty()) {
            salida << linea << '\n';
        }

        // Cerrar los archivos
        entrada.close();
        salida.close();

        //std::cout << "Archivo convertido exitosamente. Nombre del archivo TXT: " << nombreTXT << std::endl;

        const char* archivoAnterior2 = "new.txt";
        const char* archivoNuevo2 = strcat(strcpy(new char[strlen(nombre2TXT) + 5], nombre2TXT), ".txt");
        std::ifstream entrada2(archivoAnterior2);
        std::ofstream salida2(archivoNuevo2);
        char c2;
        int i2 = 0;
        int j2 = diccionarios.getEstructura(nombre2TXT).getTamAtIndex(i2);
        while (entrada2.get(c2)) {
            if (c2 == '\n') {
                while (j2 != 0) {
                    salida2 << ' ';
                    j2--;
                }
                salida2 << '\n';
                i2 = 0;
                j2 = diccionarios.getEstructura(nombre2TXT).getTamAtIndex(i2);
            }
            else if (c2 == '#') {
                if (j2 == 0) {
                    i2++;
                    j2 = diccionarios.getEstructura(nombre2TXT).getTamAtIndex(i2);
                }
                else {
                    while (j2 != 0) {
                        salida2 << ' ';
                        j2--;
                    }
                    i2++;
                    j2 = diccionarios.getEstructura(nombre2TXT).getTamAtIndex(i2);
                }
            }
            else {
                salida2 << c2;
                j2--;
            }
        }

        // Cerrar los archivos
        entrada2.close();
        salida2.close();
        int nSectores = Disco.getNumSectoresXBloque(); // N sectores por bloque o pagina 
        int bytes_por_sector = Disco.getNumBytes(); //bytes por sector
        std::ifstream pro(archivoNuevo2, std::ios::binary);
        pro.seekg(0, std::ios::end);
        std::streampos fileSize = pro.tellg();
        pro.seekg(0, std::ios::beg);
        int inde = -1;
        //UBICAR PAGINA PARA INGRESAR NUEVOS DATOS
        for (int jai = 0; jai < AllPages.getNumPages(); jai++) {
            if (AllPages.getBytesOcupadosbyIndex(jai) != AllPages.getBytesbyIndex(jai)) {
                inde = jai; break;
            }
        }

        if (inde != -1) {
            std::string car; //LINEA DE TEXTO
            std::string registros_conca; // Concatenar registros
            std::string cabecera_conca; // Concatenar cabeceras
            int bytes_regis = 2; // Recolectar suma de bytes_registros que pueden estar en un sector
            int tamReg = diccionarios.getEstructura(nombre2TXT).getTamTotal() + 1; // tamanio registro
            int indice = 1;//indice registro
            int posicion_regis = 1;
            int sect = 0;// primer sector por pagina o bloque
            if (fileSize < (AllPages.getBytesbyIndex(inde) * AllPages.getNumPages())) {
                //leer X caracteres y eso sera un registro luego le agregamos "|" al final y lo concatenamos en un string, la cabecera se determinara numReg y (numreg-1)*X =(posicion)
                //para agregarlo a la pagina debemos saber cuantos bytes serian el registro y la cabecera, segun eso consultamos si en nuestro tamaño sector bytes es 
                // optimo, sino pasar al otro sector agregando los datos anteriorres a la pagina
                //en caso de llenar los n sectores de pagina( ficticios) debemos cambiar pagina
                //std::cout << "PAGINA:" << inde << std::endl;
                while (std::getline(pro, car)) {
                    car.erase(std::remove(car.begin(), car.end(), '\r'), car.end());
                    car.erase(std::remove(car.begin(), car.end(), '\r\n'), car.end());
                    car.erase(std::remove(car.begin(), car.end(), '\n'), car.end());
                    car += '|';
                    //std::cout << car << std::endl;
                    std::string aux1 = std::to_string(indice);
                    std::string valor1;
                    for (int i = 0; i < (4 - aux1.length()); i++) {
                        valor1 += "0";
                    }
                    valor1 += aux1;
                    std::string cabeceraTMP = valor1 + "," + std::to_string((posicion_regis - 1) * car.length()) + "#"; // insertar cabecera
                    if ((bytes_regis + (car.length() + cabeceraTMP.length())) <= bytes_por_sector) { // VERIFICAR SI EL REGISTRO NO PASA LA CAPACIDAD DEL SECTOR
                        AllPages.ADDReg(indice, (posicion_regis - 1) * car.length(), car, car.length() + cabeceraTMP.length(), inde, sect);
                        bytes_regis += (car.length() + cabeceraTMP.length()); // aumenta el tamaño ocupado
                    }
                    else {
                        sect++; // PASA AL SIGUIENTE SECTOR
                        //Meter lo anterior registros_conca y cabecera_conca a Pagina
                        bytes_regis = 2 + (car.length() + cabeceraTMP.length());
                        if (sect == nSectores) {
                            inde++;
                            //std::cout << "PAGINA:" << inde << std::endl;
                            sect = 0;
                            posicion_regis = 1;
                        }
                        cabeceraTMP = std::to_string(indice) + "," + std::to_string((posicion_regis - 1) * tamReg) + "#";
                        AllPages.ADDReg(indice, (posicion_regis - 1) * car.length(), car, car.length() + cabeceraTMP.length(), inde, sect);
                    }
                    indice++;
                    posicion_regis++;
                }
            }
            else {
                std::cout << "El documento es muy grande para cargarlo" << std::endl;
            }
        }
        else {
            std::cout << "Las páginas ya están completas." << std::endl;
        }
        pro.close();


        try {
            std::filesystem::remove(nombreTXT);
        }
        catch (const std::filesystem::filesystem_error& error) {
            std::cout << "Error al eliminar el archivo: " << error.what() << std::endl;
        }
        try {
            std::filesystem::remove(archivoNuevo2);
        }
        catch (const std::filesystem::filesystem_error& error) {
            std::cout << "Error al eliminar el archivo: " << error.what() << std::endl;
        }
    }


    std::vector<int> findIndicesOf(const std::vector<std::string>& strings, std::string n) {
        std::vector<int> indices;

        for (int i = 0; i < strings.size(); i++) {
            if (strings[i] == n) {
                indices.push_back(i);
            }
        }

        return indices;
    }

    std::vector<std::string> split(const std::string& linea, char separador) {
        std::vector<std::string> tokens;
        std::size_t inicio = 0;
        std::size_t fin = linea.find(separador);

        while (fin != std::string::npos) {
            std::string token = linea.substr(inicio, fin - inicio);
            tokens.push_back(token);
            inicio = fin + 1;
            fin = linea.find(separador, inicio);
        }

        std::string ultimoToken = linea.substr(inicio);
        tokens.push_back(ultimoToken);

        return tokens;
    }

    std::string convertToBinaryString(const std::vector<std::string>& valores) {
        std::string resultado;

        for (const std::string& valor : valores) {
            if (valor.empty()) {
                resultado += '0';
            }
            else {
                resultado += '1';
            }
        }

        return resultado;
    }

    int countOnes(const std::string& binaryString, const std::vector<int>& indices) {
        int count = 0;
        for (int index : indices) {
            if (index >= 0 && index < binaryString.size() && binaryString[index] == '1') {
                count++;
            }
        }
        return count;
    }
    
    std::string intTOstring(int number, int n) {
        std::string str = std::to_string(number);

        if (str.length() < n) {
            str = std::string(n - str.length(), '0') + str;
        }

        return str;
    }

    void CargarArchivoVariable(const char* filename) {
        diccionarios = Cache(); // carga el esquema del disco 
        diccionarios.Reload();
        const char extensionTXT[] = ".txt";
        std::size_t indicePunto = std::strcspn(filename, ".");
        std::size_t longitudMaxima = 256;
        char nombre2TXT[256];
        if (indicePunto < std::strlen(filename)) {
            std::strncpy(nombre2TXT, filename, indicePunto);
            nombre2TXT[indicePunto] = '\0';
        }
        else {
            std::strncpy(nombre2TXT, filename, longitudMaxima);
            nombre2TXT[longitudMaxima - 1] = '\0';
        }
        char nombreTXT[] = "new.txt";

        std::ifstream entrada(filename);
        if (!entrada) {
            std::cout << "No se pudo abrir el archivo." << std::endl;
            return;
        }
        std::ofstream salida(nombreTXT);
        std::string linea;
        char c;

        // Ignorar la primera línea del archivo CSV
        entrada.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // esta contiene el esquema 

        // Procesar las líneas restantes
        bool dentroDeComillas = false;
        while (entrada.get(c)) {
            if (c == '\n') {
                // Escribir la línea procesada en el archivo de salida
                salida << linea << '\n';
                linea.clear();
            }
            else {
                size_t pos = linea.size();


                // Reemplazar comas por #
                if (c == ',') {
                    // Verificar si estamos dentro de una cadena entre comillas
                    if (dentroDeComillas) {
                        linea.push_back(c);
                    }
                    else {
                        linea.push_back('#');
                    }
                }
                else if (c == '"') {
                    // Cambiar el estado de dentroDeComillas
                    dentroDeComillas = !dentroDeComillas;
                    linea.push_back(c);
                }
                else {
                    linea.push_back(c);
                }
            }
        }

        // Escribir la última línea procesada en el archivo de salida
        if (!linea.empty()) {
            salida << linea << '\n';
        }

        // Cerrar los archivos
        entrada.close();
        salida.close();



        //std::cout << "Archivo convertido exitosamente. Nombre del archivo TXT: " << nombreTXT << std::endl;
        ///posicion,longitud de los string, enteros , nullbitmap, string
        //21,5/26,10/36,10/#65000,#10101#palabra1palabra2
        std::vector<std::string> tipos_estructura = diccionarios.getEstructura(nombre2TXT).getTypesVect();
        /*std::cout << "Contenido del vector tipos_estructura:" << std::endl;
        for (const std::string& tipo : tipos_estructura) {
            std::cout << tipo << std::endl;
        }*/
        std::vector<int> indices_int = findIndicesOf(tipos_estructura, "INT");
        /*for (const int& tipo : indices_int) {
            std::cout << tipo << std::endl;
        }*/
        std::vector<int> indices_str = findIndicesOf(tipos_estructura, "STR");
        /*for (const int& tipo : indices_str) {
            std::cout << tipo << std::endl;
        }*/
        int cantidaddeSTR = tipos_estructura.size() - indices_int.size();
        //Hallar longitud de nullbitmap
        std::string ones(tipos_estructura.size(), '1');
        int decimalbitmap = std::stoi(ones, nullptr, 2);
        int lengthBitMap = std::to_string(decimalbitmap).length();
        ///
        int maxLongReg = diccionarios.getEstructura(nombre2TXT).getTamTotal();//Tamaño maximo de Registro
        int MAXREG = maxLongReg + 3 + indices_int.size() + lengthBitMap + ((std::to_string(maxLongReg).length() * 2 + 2) * indices_str.size());
        if (std::to_string(maxLongReg).length() != std::to_string(MAXREG).length()) {
            MAXREG = maxLongReg + 3 + indices_int.size() + lengthBitMap + (((std::to_string(maxLongReg).length() + 1) * 2 + 2) * indices_str.size());
        }
        int LongBuff = std::to_string(MAXREG).length();

        /*std::cout << "Tamano NULLBITMAP: " << lengthBitMap << std::endl;
        std::cout << "Tamano MAX reg: " << MAXREG << std::endl;
        std::cout << " MAX reg : " << LongBuff << std::endl;*/

        std::vector<std::string> valores;

        std::ifstream entrada2(nombreTXT);

        if (!entrada2) {
            std::cout << "No se pudo abrir el archivo." << std::endl;
            return;
        }

        std::string lineaU;
        while (std::getline(entrada2, lineaU)) {

            std::vector<std::string> valores_linea = split(lineaU, '#');
            /*for (const std::string& valor : valores_linea) {
                std::cout <<"Val: " << valor << std::endl;
            }*/
            std::string nullbitmap = convertToBinaryString(valores_linea);
            int decimal = std::stoi(nullbitmap, nullptr, 2);

            std::string registro;
            int pos_RVariable = (countOnes(nullbitmap, indices_str) * ((LongBuff * 2) + 2)) + 1 + (5 * indices_int.size()) + 1 + std::to_string(decimal).length() + 1;
            //std::cout << nullbitmap << " - " << std::to_string(decimal) << " - " << pos_RVariable << std::endl;
            std::string cabec;
            for (int i = 0; i < indices_str.size(); i++) {
                int j = indices_str[i];
                std::string palabra = valores_linea[j];
                if (palabra != "") {
                    cabec += (intTOstring(pos_RVariable, LongBuff) + "," + intTOstring(valores_linea[j].length(), LongBuff) + "/");
                    pos_RVariable += valores_linea[j].length();
                }
            }

            registro = cabec + "#";
            for (int i = 0; i < indices_int.size(); i++) {
                int j = indices_int[i];
                std::string num = valores_linea[j];
                if (num != "") {
                    registro += (intTOstring(std::stoi(num), 4) + ",");
                }
            }
            registro += ("#" + std::to_string(decimal) + "#");

            for (int i = 0; i < indices_str.size(); i++) {
                int j = indices_str[i];
                std::string palabra = valores_linea[j];
                if (palabra != "") {
                    registro += palabra;
                }
            }


            registro += "|";
            valores.push_back(registro);
            //valores.push_back(valores_linea);
        }

        entrada2.close();
        //VECTOR VALORES -> TODOS LOS REGISTROS

        // Imprimir los valores extraídos
        /*for (const auto& linea_valores : valores) {
            std::cout << linea_valores << std::endl;
        }*/
        int fileSize = std::accumulate(valores.begin(), valores.end(), 0,
            [](int sum, const std::string& str) {
                return sum + str.length();
            }
        );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int nSectores = Disco.getNumSectoresXBloque(); // N sectores por bloque o pagina 
        int bytes_por_sector = Disco.getNumBytes();
        int inde = -1;
        //UBICAR PAGINA PARA INGRESAR NUEVOS DATOS
        for (int jai = 0; jai < AllPages.getNumPages(); jai++) {
            if (AllPages.getBytesOcupadosbyIndex(jai) != AllPages.getBytesbyIndex(jai)) {
                inde = jai; break;
            }
        }


        if (inde != -1) {
            std::string car; //LINEA DE TEXTO
            std::string registros_conca; // Concatenar registros
            std::string cabecera_conca; // Concatenar cabeceras

            int tamReg = 0; // tamanio registro
            int indice = 1;//indice registro
            int posicion_regis = AllPages.getBytesbyIndex(inde);
            int cabFreeSpace = (3 + std::to_string(posicion_regis).length());
            int bytes_regis = 2 + cabFreeSpace; // Recolectar suma de bytes_registros que pueden estar en un sector
            int sect = 0;// primer sector por pagina o bloque

            //ALMACENAR


            if (fileSize < (AllPages.getBytesbyIndex(inde) * AllPages.getNumPages())) {
                for (int i = 0; i < valores.size(); i++) {
                    tamReg = valores[i].length();//POSISCION DEL REGISTRO 
                    posicion_regis -= tamReg;
                    std::string aux1 = std::to_string(i + 1);
                    std::string valor1;
                    for (int i = 0; i < (4 - aux1.length()); i++) {
                        valor1 += "0";
                    }
                    valor1 += aux1;//Parte indice Cabecera
                    std::string cabeceraTMP = valor1 + "," + std::to_string(posicion_regis) + "#"; // insertar cabecera

                    int bytesReg = cabeceraTMP.length() + tamReg;
                    if ((bytes_regis + bytesReg) <= bytes_por_sector) {
                        AllPages.ADDRegVar(i + 1, posicion_regis, valores[i], bytesReg, inde, sect);
                        bytes_regis += bytesReg;
                    }
                    else {
                        sect++;
                        bytes_regis = 2 + bytesReg;
                        if (sect == nSectores) {
                            AllPages.ADDFREESPACE(posicion_regis + tamReg - 1, inde);
                            inde++;
                            bytesReg += cabFreeSpace;
                            sect = 0;
                            posicion_regis = AllPages.getBytesbyIndex(inde) - tamReg;
                        }
                        std::string cabeceraTMP = valor1 + "," + std::to_string(posicion_regis) + "#"; // insertar cabecera
                        int bytesReg = cabeceraTMP.length() + tamReg;
                        AllPages.ADDRegVar(i + 1, posicion_regis, valores[i], bytesReg, inde, sect);
                    }

                }
                AllPages.ADDFREESPACE(posicion_regis - 1, inde);
            }
            else {
                std::cout << "El documento es muy grande para cargarlo" << std::endl;
            }
        }
        else {
            std::cout << "Las páginas ya están completas." << std::endl;
        }


        try {
            std::filesystem::remove(nombreTXT);
        }
        catch (const std::filesystem::filesystem_error& error) {
            std::cout << "Error al eliminar el archivo: " << error.what() << std::endl;
        }
    }

    void showCabecera(int n) {
        const std::vector<std::pair<int, int>> data = AllPages.getPages()[n].getCabecera();
        for (const auto& par : data) {
            std::cout << par.first << "," << par.second << "#";
        }
        std::cout << std::endl;
    }

    void showPage(int n) {
        AllPages.showPage(n);
    }

    void Page_to_Bloq(int n) {
        std::string data = AllPages.getDataN(n);
        Disco.InsertBlock(data, AllPages.getCabeceraN(n), AllPages.getRegPartN(n), n);
    }

    void allPages_to_bloqs() {
        for (int i = 0; i < AllPages.getNumPages(); i++) {
            Page_to_Bloq(i);
        }
    }

    void MetaDataBloq(int n) {
        if (AllPages.getNumPages() <= n && 0 <= n) {
            std::cout << "Bloque " << n << " No existe." << std::endl;
        }
        else {
            std::cout << "Bloque " << n << ": " << std::endl;
            std::cout << "Bytes Total: " << Disco.getBytesBloq() << " bytes" << std::endl;
            std::cout << "Bytes Ocupados: " << Disco.getBytesBloqOcupados(n) << " bytes" << std::endl;
            std::cout << "Sectores: " << std::endl;
            Disco.ShowSectByBloq(n);
            std::cout << "Contenido: " << std::endl;
            Disco.ShowBloq(n);
        }
    }

    void MetaDataSect(int n) {
        if (AllPages.getNumPages() * Disco.getNumSectoresXBloque() <= n && 0 <= n) {
            std::cout << "Sector " << n - 1 << " No existe." << std::endl;
        }
        else {
            std::cout << "Sector " << n - 1 << ": " << std::endl;
            std::cout << "Bytes Total: " << Disco.getNumBytes() << " bytes" << std::endl;
            std::cout << "Bytes Ocupados: " << Disco.getBytesSectOcupados(n) << " bytes" << std::endl;
            std::cout << "Path: " << Disco.GetSectPATH(n) << std::endl;
            std::cout << "Contenido: " << std::endl;
            std::ifstream inputFile(Disco.GetSectPATH(n), std::ios::binary);
            if (!inputFile) {
                std::cerr << "Error al abrir el archivo Bloque: " << Disco.GetSectPATH(n) << std::endl;
                return;
            }
            char character;
            while (inputFile.get(character)) {
                std::cout << character;
            }
            inputFile.close();
        }
    }

    void ShowFILE(const char* filename, int n) {
        int reg = 0;
        std::ifstream inputFile(filename, std::ios::binary);
        if (!inputFile) {
            std::cerr << "Error al abrir el archivo: " << filename << std::endl;
            return;
        }

        std::size_t indicePunto = std::strcspn(filename, ".");
        std::size_t longitudMaxima = 256;
        char nombre2TXT[256];
        if (indicePunto < std::strlen(filename)) {
            std::strncpy(nombre2TXT, filename, indicePunto);
            nombre2TXT[indicePunto] = '\0';
        }
        else {
            std::strncpy(nombre2TXT, filename, longitudMaxima);
            nombre2TXT[longitudMaxima - 1] = '\0';
        }
        std::cout << "Registro " << n << ": " << std::endl;
        char character;
        while (inputFile.get(character)) {
            if (character == '\n') {
                reg++;
            }
            if (reg == n) {
                std::cout << character;
            }
        }
        inputFile.close();
        std::cout << std::endl;
        std::cout << "# de bytes por Registro: " << diccionarios.getEstructura(nombre2TXT).getTamTotal() << " bytes" << std::endl;
        std::cout << "# de bits por Registro: " << diccionarios.getEstructura(nombre2TXT).getTamTotal() * 8 << " bits" << std::endl;
        std::cout << "# de bytes por FILE " << filename << ": " << diccionarios.getEstructura(nombre2TXT).getTamTotal() * reg << " bytes" << std::endl;
        std::cout << "# de bits por FILE " << filename << ": " << diccionarios.getEstructura(nombre2TXT).getTamTotal() * 8 * reg << " bits" << std::endl;
    }


    void MoverRegistro(int n1, int n2) {
        AllPages.MoverRegs(n1, n2);

    }

    void BuscarRegistro(int n){
        AllPages.BuscarRegs(n);
    }

    void EliminarRegistro(int n) {
        AllPages.EliminarReg(n);
    }

    void AddRegistro(const char* tabla, int numReg, std::string registro) {
        int j = AllPages.ValidateVariable();
        if (j == 0) {
            std::string registroCompleto;
            std::vector<std::string> tipos_estructura = diccionarios.getEstructura(tabla).getTypesVect();
            std::vector<int> indices_int = findIndicesOf(tipos_estructura, "INT");
            std::vector<int> indices_str = findIndicesOf(tipos_estructura, "STR");
            int cantidaddeSTR = tipos_estructura.size() - indices_int.size();
            std::string ones(tipos_estructura.size(), '1');
            int decimalbitmap = std::stoi(ones, nullptr, 2);
            int lengthBitMap = std::to_string(decimalbitmap).length();
            int maxLongReg = diccionarios.getEstructura(tabla).getTamTotal();//Tamaño maximo de Registro
            int MAXREG = maxLongReg + 3 + indices_int.size() + lengthBitMap + ((std::to_string(maxLongReg).length() * 2 + 2) * indices_str.size());
            if (std::to_string(maxLongReg).length() != std::to_string(MAXREG).length()) {
                MAXREG = maxLongReg + 3 + indices_int.size() + lengthBitMap + (((std::to_string(maxLongReg).length() + 1) * 2 + 2) * indices_str.size());
            }
            int LongBuff = std::to_string(MAXREG).length();

            std::vector<std::string> valores_linea = split(registro, '#');
            std::string nullbitmap = convertToBinaryString(valores_linea);
            int decimal = std::stoi(nullbitmap, nullptr, 2);

            std::string registro;
            int pos_RVariable = (countOnes(nullbitmap, indices_str) * ((LongBuff * 2) + 2)) + 1 + (5 * indices_int.size()) + 1 + std::to_string(decimal).length() + 1;
            //std::cout << nullbitmap << " - " << std::to_string(decimal) << " - " << pos_RVariable << std::endl;
            std::string cabec;
            for (int i = 0; i < indices_str.size(); i++) {
                int j = indices_str[i];
                std::string palabra = valores_linea[j];
                if (palabra != "") {
                    cabec += (intTOstring(pos_RVariable, LongBuff) + "," + intTOstring(valores_linea[j].length(), LongBuff) + "/");
                    pos_RVariable += valores_linea[j].length();
                }
            }

            registro = cabec + "#";
            for (int i = 0; i < indices_int.size(); i++) {
                int j = indices_int[i];
                std::string num = valores_linea[j];
                if (num != "") {
                    registro += (intTOstring(std::stoi(num), 4) + ",");
                }
            }
            registro += ("#" + std::to_string(decimal) + "#");

            for (int i = 0; i < indices_str.size(); i++) {
                int j = indices_str[i];
                std::string palabra = valores_linea[j];
                if (palabra != "") {
                    registro += palabra;
                }
            }


            registro += "|";
            AllPages.AddNewRegVar(MAXREG, numReg, registro);
        }
        else {
            std::string registroCompleto;
            int i2 = 0;
            int j2 = diccionarios.getEstructura(tabla).getTamAtIndex(i2);
            for (char c : registro) {
                if (c == '#') {
                    if (j2 == 0) {
                        i2++;
                        j2 = diccionarios.getEstructura(tabla).getTamAtIndex(i2);
                    }
                    else {
                        while (j2 != 0) {
                            registroCompleto += ' ';
                            j2--;
                        }
                        i2++;
                        j2 = diccionarios.getEstructura(tabla).getTamAtIndex(i2);
                    }
                }
                else {
                    registroCompleto += c;
                    j2--;
                }
            }
            while (j2 != 0) {
                registroCompleto += ' ';
                j2--;
            }
            ///std::cout << "Registro: " << registroCompleto << std::endl;
            //std::cout << "Bytes: " << registroCompleto.length() << std::endl;
            AllPages.AddNewRegFija(numReg, registroCompleto);
        }


    }

    void getValFreeSpaceMapping() {
        std::cout << "La página con mayor Registros eliminados es la PAGINA " << AllPages.freeSpaceMapping() << std::endl;
    }
};

int main() {

    int opcion = 0;
    BufferManager* PC = new BufferManager();
    while (true) {
        // Mostrar opciones disponibles
        std::cout << "--------------------------------------------------------------------------" << std::endl;
        std::cout << "Opciones disponibles:" << std::endl;
        std::cout << "1. Crear Disco" << std::endl;
        std::cout << "2. Cargar Disco" << std::endl;
        std::cout << "3. Crear Tabla" << std::endl;
        std::cout << "4. Insertar Documento de Longitud Fija" << std::endl;
        std::cout << "5. Mostrar Datos del FILE" << std::endl;
        std::cout << "6. Mostrar Bloque" << std::endl;
        std::cout << "7. Mostrar Sector" << std::endl;
        std::cout << "8. Mostrar Pagina" << std::endl;
        std::cout << "9. Guardar Cambios de Pagina" << std::endl;
        std::cout << "10. Guardar Cambios de Todas las Paginas" << std::endl;
        std::cout << "11. Mover Registros" << std::endl;
		std::cout << "12. Buscar un registro" << std::endl;
        std::cout << "13. Eliminar Registro" << std::endl;
        std::cout << "14. Free Space Mapping" << std::endl;
        std::cout << "15. Agregar Nuevo Registro" << std::endl;
        std::cout << "16. Insertar Documento de Longitud Variable" << std::endl;
        std::cout << "20. Apagar" << std::endl;

        // Solicitar opción al usuario
        std::cout << "Ingrese el numero de opcion: ";
        std::cin >> opcion;
        // Evaluar la opción ingresada
        if (opcion == 1) {
            std::cout << "Selecciono la opcion: Crear Disco" << std::endl;
            PC->MakeDisco();
        }
        else if (opcion == 2) {
            std::cout << "Selecciono la opcion: Cargar Disco" << std::endl;
            PC->LoadDisk();
        }
        else if (opcion == 3) {
            std::cout << "Selecciono la opcion: Crear Tabla" << std::endl;
            char nombreArchivo[256];
            std::string values;
            std::cout << "Ingrese el nombre de la Estructura: ";
            std::cin >> nombreArchivo;
            std::cout << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ingrese la composicion (tipo#valor#nombre|tipo2#valor2#nombre2|...): ";
            std::getline(std::cin, values);
            std::cout << std::endl;
            PC->InsertStruct(values.c_str(), nombreArchivo);
        }
        else if (opcion == 4) {
            std::cout << "Selecciono la opcion: Insertar Documento de Longitud Fija" << std::endl;
            char nombreArchivo[256];
            std::cout << "Ingrese el nombre del Documento con extension: ";
            std::cin >> nombreArchivo;
            std::cout << std::endl;
            PC->CargarArchivo(nombreArchivo);
        }
        else if (opcion == 5) {
            std::cout << "Selecciono la opcion: Mostrar Datos del FILE" << std::endl;
            char nombreArchivo[256];
            int opt = 0;
            std::cout << "Ingrese el nombre del Documento con extension: ";
            std::cin >> nombreArchivo;
            std::cout << "Ingrese el numero de Registro: ";
            std::cin >> opt;
            std::cout << std::endl;
            PC->ShowFILE(nombreArchivo, opt);
        }
        else if (opcion == 6) {
            std::cout << "Selecciono la opcion: Mostrar Bloque" << std::endl;
            int opt = 0;
            std::cout << "Ingrese el numero de Bloque: ";
            std::cin >> opt;
            std::cout << std::endl;
            PC->MetaDataBloq(opt);
        }
        else if (opcion == 7) {
            std::cout << "Selecciono la opcion: Mostrar Sector" << std::endl;
            int opt = 0;
            std::cout << "Ingrese el numero de Sector: ";
            std::cin >> opt;
            std::cout << std::endl;
            PC->MetaDataSect(opt + 1);
        }
        else if (opcion == 8) {
            std::cout << "Selecciono la opcion: Mostrar Pagina" << std::endl;
            int opt = 0;
            std::cout << "Ingrese el numero de Pagina: ";
            std::cin >> opt;
            std::cout << std::endl;

            PC->showPage(opt);
        }
        else if (opcion == 9) {
            std::cout << "Selecciono la opcion: Guardar Cambios de Pagina" << std::endl;
            int opt = 0;
            std::cout << "Ingrese el numero de Pagina a Guardar: ";
            std::cin >> opt;
            std::cout << std::endl;
            PC->Page_to_Bloq(opt);
        }
        else if (opcion == 10) {
            std::cout << "Selecciono la opcion: Guardar Cambios de Todas las Paginas" << std::endl;
            PC->allPages_to_bloqs();
        }
        else if (opcion == 11) {
            std::cout << "Selecciono la opcion: Mover Registros" << std::endl;
            int opt = 0, opt2 = 0;
            std::cout << "Ingrese el numero del Primer Registro: ";
            std::cin >> opt;
            std::cout << "Ingrese el numero del Segundo Registro: ";
            std::cin >> opt2;
            std::cout << std::endl;
            PC->MoverRegistro(opt, opt2);
        }
		else if (opcion == 12) {
			std::cout << "Selecciono la opcion: Buscar Registro" << std::endl;
			int opt = 0;
			std::cout << "Ingrese el numero del Registro: ";
			std::cin >> opt;
			std::cout << std::endl;
			PC->BuscarRegistro(opt);
		}
        else if (opcion == 13) {
            std::cout << "Selecciono la opcion: Eliminar Registro" << std::endl;
            int opt = 0;
            std::cout << "Ingrese el numero del Registro: ";
            std::cin >> opt;
            std::cout << std::endl;
            PC->EliminarRegistro(opt);
        }
        else if (opcion == 14) {
            std::cout << "Selecciono la opcion: Free Space Mapping" << std::endl;
            std::cout << std::endl;
            PC->getValFreeSpaceMapping();
        }
        else if (opcion == 15) {
            std::cout << "Selecciono la opcion: Agregar Nuevo Registro" << std::endl;
            char nombreTabla[256];
            std::string Registro;
            int numReg = 0;
            std::cout << "Ingrese el nombre de la Tabla: ";
            std::cin >> nombreTabla;
            std::cout << "Ingrese el numero de Registro: ";
            std::cin >> numReg;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ingrese el Registro: ";
            std::getline(std::cin, Registro);
            std::cout << std::endl;
            int i = 1;
            while (i != 0) {
                PC->AddRegistro(nombreTabla, numReg, Registro);
                i--;
            }

        }
        else if (opcion == 16) {
            std::cout << "Selecciono la opcion: Insertar Documento de Longitud Variable" << std::endl;
            char nombreArchivo[256];
            std::cout << "Ingrese el nombre del Documento con extension: ";
            std::cin >> nombreArchivo;
            std::cout << std::endl;
            PC->CargarArchivoVariable(nombreArchivo);
        }
        else if (opcion == 20) {
            std::cout << "Selecciono la opcion: Apagar" << std::endl;
            std::cout << "Apagando Sistema..." << std::endl;
            break;
        }
        else {
            std::cout << "Opcion invalida. Por favor, ingrese un numero de opcion valido." << std::endl;
        }
    }

    return 0;
}



//5-3-5-3280-3
//titanic.csv
//INT#PassengerId|STR#2#Survived|STR#2#Survived|STR#84#Name|STR#7#Sex|STR#5#Age|STR#2#SibSp|STR#2#Parch|STR#20#Ticket|STR#12#Fare|STR#20#Cabin|STR#2#Embarked|
//2#1#1#"Cumings, Mrs. John Bradley (Florence Briggs Thayer)"#female#38#1#0#PC 17599#71.2833#C85#C
//Movie_Ratings.csv
//STR#23#Pelicula|INT#Patrick C|INT#Heather|INT#Bryan|INT#Patrick T|INT#Thomas|INT#aaron|INT#vanessa|INT#greg|INT#brian|INT#ben|INT#Katherine|INT#Jonathan|INT#Zwe|INT#Erin|INT#Chris|INT#Zak|INT#Matt|INT#Chris|INT#Josh|INT#Amy|INT#Valerie|INT#Gary|INT#Stephen|INT#Jessica|INT#Jeff|
//"Avatar 2"#1#1#2######################2#
//_CRT_SECURE_NO_WARNINGS
