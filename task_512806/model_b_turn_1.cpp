#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>

// Represent a pixel as a 32-bit unsigned integer (assuming RGBA format)
typedef unsigned int Pixel;
constexpr Pixel BLACK = 0x00000000;
constexpr Pixel WHITE = 0xFFFFFFFF;
constexpr Pixel RED = 0xFF000000;
constexpr Pixel GREEN = 0x00FF0000;
constexpr Pixel BLUE = 0x0000FF00;

// Function to apply pixel masking using bitwise AND
void applyPixelMaskBitwise(std::vector<Pixel>& image, const Pixel& mask) {
    for (int i = 0; i < static_cast<int>(image.size()); ++i) {
        image[i] &= mask;
    }
}

// Function to apply pixel masking using arithmetic multiplication
void applyPixelMaskArithmetic(std::vector<Pixel>& image, const Pixel& mask) {
    for (int i = 0; i < static_cast<int>(image.size()); ++i) {
        image[i] *= (static_cast<double>(mask) / 0xFFFFFFFF);
    }
}

// Function to apply thresholding using bitwise AND and comparison
void applyThresholdBitwise(std::vector<Pixel>& image, const Pixel& threshold) {
    for (int i = 0; i < static_cast<int>(image.size()); ++i) {
        image[i] = image[i] > threshold ? WHITE : BLACK;
    }
}

// Function to apply thresholding using arithmetic comparison and multiplication
void applyThresholdArithmetic(std::vector<Pixel>& image, const Pixel& threshold) {
    for (int i = 0; i < static_cast<int>(image.size()); ++i) {
        image[i] = (static_cast<double>(image[i]) / 0xFFFFFFFF) > (static_cast<double>(threshold) / 0xFFFFFFFF) ? WHITE : BLACK;
    }
}

// Function to combine color channels using bitwise OR
void combineColorChannelsBitwise(std::vector<Pixel>& image, const Pixel& red, const Pixel& green, const Pixel& blue) {
    for (int i = 0; i < static_cast<int>(image.size()); ++i) {
        image[i] = (red & 0xFF0000) | ((green & 0x00FF00) << 8) | ((blue & 0x0000FF) << 16) | (image[i] & 0xFF000000);
    }
}

// Function to combine color channels using arithmetic multiplication and addition
void combineColorChannelsArithmetic(std::vector<Pixel>& image, const Pixel& red, const Pixel& green, const Pixel& blue) {
    for (int i = 0; i < static_cast<int>(image.size()); ++i) {
        const double r = static_cast<double>(red & 0xFF) / 255.0;
        const double g = static_cast<double>(green & 0xFF) / 255.0;
        const double b = static_cast<double>(blue & 0xFF) / 255.0;
        image[i] = static_cast<Pixel>(
            (image[i] & 0xFF000000)         // Retain alpha
            | static_cast<unsigned int>(r * 255.0) << 16
            | static_cast<unsigned int>(g * 255.0) << 8
            | static_cast<unsigned int>(b * 255.0)
        );
    }
}

// Function to measure the execution time of a given function
double measureTime(std::function<void(std::vector<Pixel>&)> func, std::vector<Pixel>& image) {
    auto start = std::chrono::high_resolution_clock::now();
    func(image);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return duration.count() / 1000.0; // Return milliseconds
}

int main() {
    const int imageWidth = 1920;
    const int imageHeight = 1080;
    size_t imageSize = imageWidth * imageHeight;

    // Generate a sample image
    std::vector<Pixel> image(imageSize);
    for (size_t i = 0; i < imageSize; ++i) {
        image[i] = (0xFF << 24) | // Alpha
                   static_cast<unsigned char>((i * 256) % 256) << 16 | // Red
                   static_cast<unsigned char>((i * 128) % 256) << 8 | // Green
                   static_cast<unsigned char>((i * 64) % 256); // Blue
    }
    
    // Pixel mask for testing
    const Pixel mask = 0xFFFFFF00; // Mask out the blue channel
    
    // Threshold for testing
    const Pixel threshold = 0x80808080;

    // Color channels for testing
    const Pixel red = RED;
    const Pixel green = GREEN;
    const Pixel blue = BLUE;
    
    // Measure performance for pixel masking
    std::cout << "\nPixel Masking Performance:\n";
    double bitwiseMaskTime = measureTime(applyPixelMaskBitwise, image);
    double arithmeticMaskTime = measureTime(applyPixelMaskArithmetic, image);
    std::cout << "Bitwise AND time: " << bitwiseMaskTime << " ms\n";
    std::cout << "Arithmetic multiplication time: " << arithmeticMaskTime << " ms\n";
    std::cout << "Speedup using bitwise: " << (arithmeticMaskTime / bitwiseMaskTime) << "x\n";

    // Measure performance for thresholding
    std::cout << "\nThresholding Performance:\n";
    double bitwiseThresholdTime = measureTime(applyThresholdBitwise, image);
    double arithmeticThresholdTime = measureTime(applyThresholdArithmetic, image);
    std::cout << "Bitwise comparison and AND time: " << bitwiseThresholdTime << " ms\n";
    std::cout << "Arithmetic comparison and multiplication time: " << arithmeticThresholdTime << " ms\n";
    std::cout << "Speedup using bitwise: " << (arithmeticThresholdTime / bitwiseThresholdTime) << "x\n";

    // Measure performance for combining color channels
    std::cout << "\nCombining Color Channels Performance:\n";
    double bitwiseCombineTime = measureTime(combineColorChannelsBitwise, image, red, green, blue);
    double arithmeticCombineTime = measureTime(combineColorChannelsArithmetic, image, red, green, blue);
    std::cout << "Bitwise OR time: " << bitwiseCombineTime << " ms\n";
    std::cout << "Arithmetic multiplication and addition time: " << arithmeticCombineTime << " ms\n";
    std::cout << "Speedup using bitwise: " << (arithmeticCombineTime / bitwiseCombineTime) << "x\n";

    return 0;
}