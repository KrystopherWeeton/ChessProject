#ifndef RAFile_H
#define RAFile_H

#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <fstream>

template <class T>
class RAFile {
private:
    
    static int intOffset;
    
    std::fstream file;                          // The actual file
    std::string fileName = "Uninitialized";     // A string representing the files name
    bool initialized = false;                   // A boolean showing whether a file is open
    int length = 0;
    int byteOffset;
    
public:
    
    //Constructor that sets the byteOffset
    RAFile() {
        byteOffset = sizeof(T);
    }
    
    //Destructor, which closes any file that is open
    ~RAFile() {
        closeFile();
    }
    
    // Creates a new empty file to be used
    bool newfile(std::string name) {
        if (initialized)    // A file is already open
            return false;
        
        length = 0;
        initialized = true;
        file = std::fstream(name, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
        return !file.fail();
        
        file.seekp(0);
        file.write(reinterpret_cast<const char*>(&length), intOffset);
    }
    
    // Loads a file from memory, without truncating it
    bool loadFile(std::string name) {
        if (initialized)    // A file is already open
            return false;
        
        struct stat buffer;
        bool alreadyExists = (stat (name.c_str(), &buffer) == 0);
        
        initialized = true;
        file.open(name, std::ios::in | std::ios::app | std::ios::binary);
        if (file.fail())
            return false;
        
        if (!alreadyExists) {
            length = 0;
        } else {
            file.seekg(0);
            // We pull the integer stored at the beginning of the file, to the length
            file.read(reinterpret_cast<char*>(&length), RAFile::intOffset);
        }
        
        return true;
    }
    
    // Closes the file currently open
    bool closeFile() {
        if (!initialized)
            return false;
        
        // Writes the length of the file to the file itself
        file.seekp(0);
        file.write(reinterpret_cast<const char*>(&length), RAFile::intOffset);
        
        file.seekg(0);
        file.read(reinterpret_cast<char*>(&length), RAFile::intOffset);
        
        length = 0;
        file.close();
        initialized = false;
        return true;
    }
    
    // Gets the value at a given index (Works off of 0 -> size - 1
    bool get(int index, T& ref) {
        if (!initialized)
            return false;
        file.seekg(intOffset + (index - 1) * sizeof(T), std::ios::beg);
        file.read(reinterpret_cast<char*>(&ref), sizeof(T));
        return true;
    }
    
    // Puts an element into the file, at the given index (overwriting it)
    bool overwrite(int index, T element) {
        if (!initialized)
            return false;
        //We must take into account, the size at the beginning of the file
        file.seekp(intOffset + (index - 1) * byteOffset);
        file.write(reinterpret_cast<const char*>(&element), byteOffset);
        return true;
    }
    
    // Appends an element to the end of the file
    bool append(T element) {
        if (!initialized)
            return false;
        
        file.seekp(intOffset + (length++ * byteOffset));
        file.write(reinterpret_cast<const char*>(&element), byteOffset);
        return true;
    }
    
    // Appenda a list of elements to the end of the file
    bool append(std::vector<T> elements) {
        if (!initialized)
            return false;
        
        for (auto it = elements.cbegin(); it != elements.cend(); it++)
            append(*it);
        return true;
    }
    
    // Performs the function for every entry, until the function returns false
    void forEveryEntry(std::function<bool(T input)> func) {
        if (!isOpen()) return;
        for (int i = 0; i < length; i++)
            if (!func(get(i))) return;
    }

    
    // Returns true iff a file is open, and false otherwise
    bool isOpen() const {
        return initialized;
    }
    
    // Returns the current size of the file, if it is open and returns -1 if it is closed
    int size() const {
        return length;
    }
    
};


template <class T>
int RAFile<T>::intOffset = sizeof(int);

#endif



