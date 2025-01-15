#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
// Define the dimensions of the image
const int WIDTH = 1000;
const int HEIGHT = 1000;

// Function for traditional pixel masking
void pixelMaskingTraditional(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, uint8_t mask) {
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = (input[i] * mask) / 255; // Applying mask by multiplication and division
    }
}

// Function for bitwise pixel masking
void pixelMaskingBitwise(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, uint8_t mask) {
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = input[i] & mask; // Applying mask with bitwise AND
    }
}

// Function for traditional thresholding
void thresholdTraditional(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, uint8_t threshold) {
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = (input[i] > threshold) ? 255 : 0; // Standard comparison
    }
}

// Function for bitwise thresholding (using shift)
void thresholdBitwise(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, uint8_t threshold) {
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = (input[i] ^ threshold) >> 7; // Using bitwise XOR and shift
    }
}
void measurePerformance() {
    // Initialize random seed for demo
    std::srand(std::time(nullptr));

    // Prepare input data
    std::vector<uint8_t> input(WIDTH * HEIGHT);
    for (size_t i = 0; i < input.size(); ++i) {
        input[i] = std::rand() % 256; // Fill with random pixel values
    }
    
    std::vector<uint8_t> output(WIDTH * HEIGHT);
    uint8_t mask = 128; // Example mask for masking
    uint8_t threshold = 128; // Example threshold

    // Measure traditional pixel masking performance
    auto start = std::chrono::high_resolution_clock::now();
    pixelMaskingTraditional(input, output, mask);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> traditionalMaskTime = end - start;
    std::cout << "Traditional Pixel Masking Time: " << traditionalMaskTime.count() << " seconds.\n";

    // Measure bitwise pixel masking performance
    start = std::chrono::high_resolution_clock::now();
    pixelMaskingBitwise(input, output, mask);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> bitwiseMaskTime = end - start;
    std::cout << "Bitwise Pixel Masking Time: " << bitwiseMaskTime.count() << " seconds.\n";

    // Measure traditional threshold performance
    start = std::chrono::high_resolution_clock::now();
    thresholdTraditional(input, output, threshold);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> traditionalThresholdTime = end - start;
    std::cout << "Traditional Thresholding Time: " << traditionalThresholdTime.count() << " seconds.\n";

    // Measure bitwise threshold performance
    start = std::chrono::high_resolution_clock::now();
    thresholdBitwise(input, output, threshold);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> bitwiseThresholdTime = end - start;
    std::cout << "Bitwise Thresholding Time: " << bitwiseThresholdTime.count() << " seconds.\n";
}
int main() {
    measurePerformance();
    return 0;
}