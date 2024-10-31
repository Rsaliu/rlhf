

#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>

int main() {
    // Original data to be compressed
    std::string originalData = "This is some sample data that needs to be compressed.";

    // Compress the data
    int compressionLevel = Z_DEFAULT_COMPRESSION;
    z_stream compressStream;
    compressStream.zalloc = Z_NULL;
    compressStream.zfree = Z_NULL;
    compressStream.opaque = Z_NULL;

    if (deflateInit2(&compressStream, compressionLevel, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        std::cerr << "Error initializing compression stream." << std::endl;
        return 1;
    }

    std::vector<char> compressedData;
    compressStream.next_in = (Bytef*)originalData.data();
    compressStream.avail_in = originalData.size();

    int flush = Z_NO_FLUSH;
    while (compressStream.avail_in != 0 || flush != Z_FINISH) {
        char buffer[1024];
        compressStream.next_out = (Bytef*)buffer;
        compressStream.avail_out = sizeof(buffer);

        int ret = deflate(&compressStream, flush);
        if (ret != Z_OK && ret != Z_STREAM_END) {
            std::cerr << "Error compressing data." << std::endl;
            deflateEnd(&compressStream);
            return 1;
        }

        compressedData.insert(compressedData.end(), buffer, buffer + compressStream.total_out);
    }

    deflateEnd(&compressStream);

    // Decompress the data
    z_stream decompressStream;
    decompressStream.zalloc = Z_NULL;
    decompressStream.zfree = Z_NULL;
    decompressStream.opaque = Z_NULL;

    if (inflateInit2(&decompressStream, -MAX_WBITS) != Z_OK) {
        std::cerr << "Error initializing decompression stream." << std::endl;
        return 1;
    }

    std::string decompressedData;
    decompressStream.next_in = (Bytef*)compressedData.data();
    decompressStream.avail_in = compressedData.size();

    flush = Z_NO_FLUSH