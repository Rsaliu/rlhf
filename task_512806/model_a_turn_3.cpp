#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>

const int WIDTH = 1280;  // Example width for HD
const int HEIGHT = 720;   // Example height for HD

void processImage(const std::vector<uint8_t>& baseImage, 
                  const std::vector<uint8_t>& overlayImage, 
                  const std::vector<uint8_t>& mask, 
                  std::vector<uint8_t>& resultImage) {
    for (int i = 0; i < WIDTH * HEIGHT * 4; i += 4) {  // Assuming RGBA
        // Masking and blending combined together using bitwise operations
        uint8_t alphaMask = mask[i + 3]; // Use alpha channel of mask for transparency
        resultImage[i + 0] = ((overlayImage[i + 0] & alphaMask) + (baseImage[i + 0] & ~alphaMask)) >> 1;  // Red channel
        resultImage[i + 1] = ((overlayImage[i + 1] & alphaMask) + (baseImage[i + 1] & ~alphaMask)) >> 1;  // Green channel
        resultImage[i + 2] = ((overlayImage[i + 2] & alphaMask) + (baseImage[i + 2] & ~alphaMask)) >> 1;  // Blue channel
        resultImage[i + 3] = 255;  // Alpha, can adjust if needed
    }
}

void measurePerformance() {
    std::vector<uint8_t> baseImage(WIDTH * HEIGHT * 4);   // Base image (RGBA)
    std::vector<uint8_t> overlayImage(WIDTH * HEIGHT * 4); // Overlay image (RGBA)
    std::vector<uint8_t> mask(WIDTH * HEIGHT * 4);         // Mask image (RGBA)
    std::vector<uint8_t> resultImage(WIDTH * HEIGHT * 4);  // Resulting image

    // Initialize random seed for demo
    std::srand(std::time(nullptr));
    std::generate(baseImage.begin(), baseImage.end(), []() { return std::rand() % 256; });
    std::generate(overlayImage.begin(), overlayImage.end(), []() { return std::rand() % 256; });
    std::generate(mask.begin(), mask.end(), []() { return std::rand() % 256; }); // Example random mask

    // Measure processing performance
    auto start = std::chrono::high_resolution_clock::now();
    processImage(baseImage, overlayImage, mask, resultImage);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Combined Processing Time: " << elapsed.count() << " seconds.\n";
}

int main() {
    measurePerformance();
    return 0;
}