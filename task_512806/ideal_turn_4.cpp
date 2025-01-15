#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cmath>

// Constants for resolutions
const int HD_WIDTH = 1280, HD_HEIGHT = 720;
const int FHD_WIDTH = 1920, FHD_HEIGHT = 1080;
const int UHD_WIDTH = 3840, UHD_HEIGHT = 2160;

// Function to simulate memory usage (simple estimation)
size_t calculateMemoryUsage(int width, int height) {
    return width * height * 4 * sizeof(uint8_t); // RGBA
}

// Measure frame processing time and frame rate
double measureFrameProcessingTime(int width, int height, 
                                  const std::vector<uint8_t>& baseImage, 
                                  const std::vector<uint8_t>& overlayImage, 
                                  const std::vector<uint8_t>& mask, 
                                  std::vector<uint8_t>& resultImage) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < width * height * 4; i += 4) {
        uint8_t alphaMask = mask[i + 3];
        resultImage[i + 0] = ((overlayImage[i + 0] & alphaMask) + (baseImage[i + 0] & ~alphaMask)) >> 1;
        resultImage[i + 1] = ((overlayImage[i + 1] & alphaMask) + (baseImage[i + 1] & ~alphaMask)) >> 1;
        resultImage[i + 2] = ((overlayImage[i + 2] & alphaMask) + (baseImage[i + 2] & ~alphaMask)) >> 1;
        resultImage[i + 3] = 255;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    return elapsed.count();
}

// Function to test performance under various resolutions
void testPerformance() {
    int resolutions[][2] = {
        {HD_WIDTH, HD_HEIGHT},
        {FHD_WIDTH, FHD_HEIGHT},
        {UHD_WIDTH, UHD_HEIGHT}
    };
    
    for (const auto& res : resolutions) {
        int width = res[0];
        int height = res[1];
        std::vector<uint8_t> baseImage(width * height * 4);
        std::vector<uint8_t> overlayImage(width * height * 4);
        std::vector<uint8_t> mask(width * height * 4);
        std::vector<uint8_t> resultImage(width * height * 4);

        std::generate(baseImage.begin(), baseImage.end(), []() { return std::rand() % 256; });
        std::generate(overlayImage.begin(), overlayImage.end(), []() { return std::rand() % 256; });
        std::generate(mask.begin(), mask.end(), []() { return std::rand() % 256; });

        double processingTime = measureFrameProcessingTime(width, height, baseImage, overlayImage, mask, resultImage);
        double fps = 1.0 / processingTime;

        std::cout << "Resolution: " << width << "x" << height << "\n";
        std::cout << "Processing Time: " << processingTime << " seconds\n";
        std::cout << "Frame Rate: " << fps << " FPS\n";
        std::cout << "Estimated Memory Usage: " << calculateMemoryUsage(width, height) / (1024 * 1024) << " MB\n\n";
    }
}

int main() {
    std::srand(std::time(nullptr));
    testPerformance();
    return 0;
}
