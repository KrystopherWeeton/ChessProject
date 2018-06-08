/*#include "RAFile.h"

#include <iostream>

template <class T>
int RAFile<T>::intOffset = sizeof(int);

template <class T>
RAFile<T>::RAFile() {
    byteOffset = sizeof(T);
}

template <class T>
RAFile<T>::~RAFile() {
    closeFile();
}

template <class T>
bool RAFile<T>::newfile(std::string name) {
    if (initialized)    // A file is already open
        return false;
    
    length = 0;
    initialized = true;
    return file(name, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
    
    file.seekp(0);
    file.write(reinterpret_cast<const char*>(&length), intOffset);
}

template <class T>
bool RAFile<T>::loadFile(std::string name) {
    if (initialized)    // A file is already open
        return false;
    
    auto alreadyExists = std::ifstream(name);
    
    initialized = true;
    if (!file(name, std::ios::in | std::ios::app | std::ios::binary))
        return false;
    
    if (!alreadyExists) {
        length = 0;
    } else {
        // We pull the integer stored at the beginning of the file, to the length
        file.read(reinterpret_cast<char*>(&length), RAFile::intOffset);
    }
    
    return true;
}

template <class T>
std::fstream RAFile<T>::toText(std::string saveDir) {
    std::cerr << "Fuck you for wanting this" << std::endl;
    return std::fstream();
}

template <class T>
bool RAFile<T>::closeFile() {
    if (!initialized)
        return false;
    
    // Writes the length of the file to the file itself
    file.seekp(0);
    file.write(reinterpret_cast<const char*>(&length), RAFile::intOffset);
    
    length = 0;
    file.close();
    return true;
}

template <class T>
bool RAFile<T>::get(int index, T& ref) {
    if (!initialized)
        return false;
    T element;
    file.seekg(intOffset + (index - 1) * byteOffset);
    file.read(reinterpret_cast<char*>(&element), byteOffset);
    return element;
}

template <class T>
bool RAFile<T>::overwrite(int index, T element) {
    if (!initialized)
        return false;
    //We must take into account, the size at the beginning of the file
    file.seekp(intOffset + (index - 1) * byteOffset);
    file.write(reinterpret_cast<const char*>(&element), byteOffset);
    return true;
}

template <class T>
bool RAFile<T>::append(T element) {
    if (!initialized)
        return false;
    
    file.seekp(intOffset + length++ * byteOffset);
    file.write(reinterpret_cast<const char*>(&element), byteOffset);
    return true;
}

template <class T>
bool RAFile<T>::append(std::vector<T> elements) {
    if (!initialized)
        return false;
    
    for (auto it = elements.cbegin(); it != elements.cend(); it++)
        append(*it);
    return true;
}

template <class T>
void RAFile<T>::forEveryEntry(std::function<bool(T input)> func) {
    if (!isOpen()) return;
    for (int i = 0; i < length; i++)
        if (!func(get(i))) return;
}

template <class T>
bool RAFile<T>::isOpen() const {
    return initialized;
}

template <class T>
int RAFile<T>::size() const {
    return length;
}
*/
