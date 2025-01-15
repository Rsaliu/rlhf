#include <iostream>
#include <fstream>
#include <cstring>
#include <mutex>
#include <zlib.h> // zlib library header

#define MAX_POST_SIZE 1024
#define COMPRESSED_BUFFER_SIZE (MAX_POST_SIZE * 2) // Buffer size should be at least twice the data size for compression
std::mutex file_mutex;

// Compression function
int compressData(const char* input, unsigned char* output, int* output_len) {
    int ret;
    unsigned have;
    z_stream strm;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK) {
        return ret;
    }

    strm.next_in = (Bytef*)input;
    strm.avail_in = strlen(input);
    strm.next_out = (Bytef*)output;
    strm.avail_out = COMPRESSED_BUFFER_SIZE;

    /* compress until end of input */
    do {
        ret = deflate(&strm, Z_FINISH);
        if (ret != Z_OK && ret != Z_STREAM_END) {
            deflateEnd(&strm);
            return ret;
        }
    } while (ret != Z_STREAM_END);

    /* put the last two bytes of strm.total_out */
    *output_len = strm.total_out & 0xFFFF;

    /* clean up and return */
    deflateEnd(&strm);
    return Z_OK;
}

// Decompression function
int decompressData(const unsigned char* input, char* output, int input_len) {
    int ret;
    unsigned have;
    z_stream strm;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK) {
        return ret;
    }

    strm.next_in = (Bytef*)input;
    strm.avail_in = input_len;
    strm.next_out = (Bytef*)output;
    strm.avail_out = MAX_POST_SIZE;

    /* decompress until end of input */
    do {
        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret != Z_OK && ret != Z_STREAM_END) {
            inflateEnd(&strm);
            return ret;
        }
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    inflateEnd(&strm);
    return Z_OK;
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

    unsigned char compressedData[COMPRESSED_BUFFER_SIZE];
    int compressedLen;

    std::lock_guard<std::mutex> lock(file_mutex);

    std::ofstream outFile(filename, std::ios::app | std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return;
    }

    // Compress the post data
    int ret = compressData(post, compressedData, &compressedLen);
    if (ret != Z_OK) {
        std::cerr << "Error compressing post: " << zlib_error(ret) << std::endl;
        return;
    }

    // Write the compressed data size and data to the file
    outFile.write((char*)&compressedLen, sizeof(int));
    outFile.write((char*)compressedData, compressedLen);
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

    char buffer[MAX_POST_SIZE + 1];
    int compressedLen;

    while (inFile.read((char*)&compressedLen, sizeof(int))) {
        if (inFile.gcount() != sizeof(int)) {
            std::cerr << "Error: Failed to read compressed post size." << std::endl;
            break;
        }

        // Read the compressed data
        unsigned char compressedData[COMPRESSED_BUFFER_SIZE];
        if (!inFile.read((char*)compressedData, compressedLen)) {
            std::cerr << "Error: Failed to read compressed post data." << std::endl;
            break;
        }

        // Decompress the data
        int ret = decompressData(compressedData, buffer, compressedLen);
        if (ret != Z_OK) {
            std::cerr << "Error decompressing post: " << zlib_error(ret) << std::endl;
            continue;
        }

        std::cout << "Post: " << buffer << std::endl;
    }

    inFile.close();
}

const char* zlib_error(int ret) {
    switch (ret) {
        case Z_OK: return "Z_OK";
        case Z_STREAM_END: return "Z_STREAM_END";
        case Z_NEED_DICT: return "Z_NEED_DICT";
        case Z_ERRNO: return "Z_ERRNO";
        case Z_STREAM_ERROR: return "Z_STREAM_ERROR";
        case Z_DATA_ERROR: return "Z_DATA_ERROR";
        case Z_MEM_ERROR: return "Z_MEM_ERROR";
        case Z_BUF_ERROR: return "Z_BUF_ERROR";
        case Z_VERSION_ERROR: return "Z_VERSION_ERROR";
        default: return "unknown error";
    }
}

int main() {
    const char* filename = "posts.txt";

    // Save some posts
    savePost("Hello, World!", filename);
    savePost("This is a test post for the community platform.", filename);

    // Load and display all posts
    loadPosts(filename);

    return 0;
}