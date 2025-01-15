#include <iostream>
#include <fstream>
#include <cstring>
#include <mutex>
#include <zlib.h>  // Include zlib header

#define MAX_POST_SIZE 1024
std::mutex file_mutex;

// Function to compress data using zlib
int compressData(const char *data, size_t dataSize, byte** compressedData) {
    uLongf compressedLength = compressBound(dataSize);
    *compressedData = new byte[compressedLength];

    if ( compress( *compressedData, &compressedLength, reinterpret_cast<const Bytef*>(data), dataSize) != Z_OK ) {
        delete[] *compressedData;
        return -1; // Compression failed
    }
    return static_cast<int>(compressedLength);
}

// Function to decompress data using zlib
char* decompressData(const byte* compressedData, size_t compressedSize, size_t originalSize) {
    char *decompressedData = new char[originalSize + 1]; // +1 for null-terminator

    if (uncompress(reinterpret_cast<Bytef*>(decompressedData), &originalSize, compressedData, compressedSize) != Z_OK) {
        delete[] decompressedData;
        return nullptr; // Decompression failed
    }
    decompressedData[originalSize] = '\0'; // Ensure null-termination
    return decompressedData;
}

void savePost(const char *post, const char *filename) {
    if (strlen(post) == 0) {
        std::cerr << "Error: Post is empty." << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(file_mutex);

    byte* compressedData = nullptr;
    int compressedSize = compressData(post, strlen(post), &compressedData);
    if (compressedSize < 0) {
        std::cerr << "Error: Compression failed." << std::endl;
        return;
    }

    std::ofstream outFile(filename, std::ios::app | std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        delete[] compressedData; // Clean up
        return;
    }

    outFile.write(reinterpret_cast<const char*>(&compressedSize), sizeof(compressedSize)); // Write the size
    outFile.write(reinterpret_cast<char*>(compressedData), compressedSize); // Write compressed data
    outFile.close();
    
    delete[] compressedData; // Clean up
}

void loadPosts(const char *filename) {
    std::lock_guard<std::mutex> lock(file_mutex);

    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error: Unable to open file for reading." << std::endl;
        return;
    }

    while (true) {
        size_t compressedSize;
        inFile.read(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));
        if (inFile.eof()) break; // End of file
        
        byte* compressedData = new byte[compressedSize];
        inFile.read(reinterpret_cast<char*>(compressedData), compressedSize);

        char* post = decompressData(compressedData, compressedSize, MAX_POST_SIZE);
        if (post) {
            std::cout << "Post: " << post << std::endl;
            delete[] post;
        } else {
            std::cerr << "Error: Decompression failed" << std::endl;
        }

        delete[] compressedData; // Clean up
    }

    inFile.close();
}

int main() {
    const char* filename = "posts.bin";

    // Save some posts
    savePost("Hello, World!", filename);
    savePost("This is a test post for the community platform.", filename);

    // Load and display all posts
    loadPosts(filename);

    return 0;
}