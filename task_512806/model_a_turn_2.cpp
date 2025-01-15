#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cstring>  // For memcpy

// Overlay function using bitwise operations
void applyOverlay(const std::vector<uint8_t>& baseImage, 
                  const std::vector<uint8_t>& overlayImage, 
                  const std::vector<uint8_t>& mask, 
                  std::vector<uint8_t>& resultImage, 
                  int width, int height) {
    for (int i = 0; i < width * height * 4; i += 4) {  // Assuming RGBA
        // Use mask to blend the overlay
        resultImage[i + 0] = (overlayImage[i + 0] & mask[i + 0]) | (baseImage[i + 0] & ~mask[i + 0]); // Red
        resultImage[i + 1] = (overlayImage[i + 1] & mask[i + 1]) | (baseImage[i + 1] & ~mask[i + 1]); // Green
        resultImage[i + 2] = (overlayImage[i + 2] & mask[i + 2]) | (baseImage[i + 2] & ~mask[i + 2]); // Blue
        resultImage[i + 3] = 255;  // Alpha, can be adjusted based on your needs
    }
}

void measureOverlayPerformance(int width, int height) {
    std::vector<uint8_t> baseImage(width * height * 4);   // Base image (RGBA)
    std::vector<uint8_t> overlayImage(width * height * 4); // Overlay image (RGBA)
    std::vector<uint8_t> mask(width * height * 4);         // Mask image (RGBA)
    std::vector<uint8_t> resultImage(width * height * 4);  // Resulting image

    // Fill images with random data for demonstration
    std::srand(std::time(nullptr));
    std::generate(baseImage.begin(), baseImage.end(), []() { return std::rand() % 256; });
    std::generate(overlayImage.begin(), overlayImage.end(), []() { return std::rand() % 256; });
    std::generate(mask.begin(), mask.end(), []() { return std::rand() % 256; });  // Binary options for the mask

    // Measure overlay performance
    auto start = std::chrono::high_resolution_clock::now();
    applyOverlay(baseImage, overlayImage, mask, resultImage, width, height);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> overlayTime = end - start;
    std::cout << "Overlay Time: " << overlayTime.count() << " seconds.\n";
}

int main() {
    const int width = 1920;  // Example for HD
    const int height = 1080; // Example for HD
    measureOverlayPerformance(width, height);
    return 0;
}