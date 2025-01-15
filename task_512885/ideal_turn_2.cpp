#include <iostream>
#include <fstream>
#include <cstring>
#include <mutex>
#include <zlib.h>
#include <vector>

#define MAX_POST_SIZE 1024
std::mutex file_mutex;

// Compress data using Zlib
bool compressData(const char* input, size_t inputSize, std::string& output) {
    uLongf compressedSize = compressBound(inputSize);
    std::vector<char> compressedData(compressedSize);

    if (compress(reinterpret_cast<Bytef*>(compressedData.data()), &compressedSize,
                 reinterpret_cast<const Bytef*>(input), inputSize) != Z_OK) {
        return false;
    }

    output.assign(compressedData.data(), compressedSize);
    return true;
}

// Decompress data using Zlib
bool decompressData(const char* input, size_t inputSize, std::string& output) {
    uLongf decompressedSize = MAX_POST_SIZE;
    std::vector<char> decompressedData(decompressedSize);

    if (uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), &decompressedSize,
                   reinterpret_cast<const Bytef*>(input), inputSize) != Z_OK) {
        return false;
    }

    output.assign(decompressedData.data(), decompressedSize);
    return true;
}

void savePost(const char* post, const char* filename) {
    if (strlen(post) == 0) {
        std::cerr << "Error: Post is empty." << std::endl;
        return;
    }

    if (strlen(post) > MAX_POST_SIZE) {
        std::cerr << "Error: Post exceeds maximum allowed size." << std::endl;
        return;
    }

    std::string compressedPost;
    if (!compressData(post, strlen(post), compressedPost)) {
        std::cerr << "Error: Compression failed." << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(file_mutex);

    std::ofstream outFile(filename, std::ios::app | std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return;
    }

    size_t size = compressedPost.size();
    outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
    outFile.write(compressedPost.data(), compressedPost.size());
    outFile.close();

    if (outFile.fail()) {
        std::cerr << "Error: Failed to write to file." << std::endl;
    }
}

void loadPosts(const char* filename) {
    std::lock_guard<std::mutex> lock(file_mutex);

    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error: Unable to open file for reading." << std::endl;
        return;
    }

    while (!inFile.eof()) {
        size_t size;
        inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
        if (inFile.eof()) break;

        std::vector<char> compressedPost(size);
        inFile.read(compressedPost.data(), size);

        std::string decompressedPost;
        if (!decompressData(compressedPost.data(), size, decompressedPost)) {
            std::cerr << "Error: Decompression failed." << std::endl;
            continue;
        }

        std::cout << "Post: " << decompressedPost << std::endl;
    }

    inFile.close();
}

int main() {
    const char* filename = "posts_compressed.txt";

    // Save some posts
    savePost("Hello, World! This is a compressed post.", filename);
    savePost("Compression helps optimize storage for our platform.", filename);

    // Load and display all posts
    loadPosts(filename);

    return 0;
}
