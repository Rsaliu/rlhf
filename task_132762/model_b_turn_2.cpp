#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>

int compressData(const std::string& input, std::vector<unsigned char>& output, int compressionLevel) {
    // ... (Same as before, but use compressionLevel in deflateInit2)
}

int main() {
    std::string message = "This is a sample message to be compressed.";

    // Compression with different levels
    for (int compressionLevel = 1; compressionLevel <= 9; compressionLevel++) {
        std::vector<unsigned char> compressedData;
        int ret = compressData(message, compressedData, compressionLevel);
        if (ret != Z_OK) {
            std::cerr << "Compression failed at level " << compressionLevel << std::endl;
            return 1;
        }

        std::cout << "Compression level: " << compressionLevel << ", Compressed size: " << compressedData.size() << std::endl;
    }
    return 0;
}