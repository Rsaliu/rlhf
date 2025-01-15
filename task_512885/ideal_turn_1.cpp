#include <iostream>
#include <fstream>
#include <cstring>
#include <mutex>

#define MAX_POST_SIZE 1024
std::mutex file_mutex;

void savePost(const char* post, const char* filename) {
    if (strlen(post) == 0) {
        std::cerr << "Error: Post is empty." << std::endl;
        return;
    }

    if (strlen(post) > MAX_POST_SIZE) {
        std::cerr << "Error: Post exceeds maximum allowed size." << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(file_mutex);

    std::ofstream outFile(filename, std::ios::app | std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return;
    }

    outFile.write(post, strlen(post));
    outFile.put('\n');  // Add a newline to separate posts
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

    char buffer[MAX_POST_SIZE + 1]; // Buffer to store loaded posts
    while (inFile.getline(buffer, MAX_POST_SIZE + 1)) {
        char localCopy[MAX_POST_SIZE + 1];
        strcpy(localCopy, buffer); // Copy the read line to another buffer
        std::cout << "Post: " << localCopy << std::endl;
    }

    inFile.close();
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