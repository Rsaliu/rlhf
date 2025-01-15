#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip> // for std::fixed and std::setprecision
#include <thread>
#include <numeric> // for std::accumulate
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
size_t getMemoryUsage() {
    // Placeholder for real memory usage code
    return sizeof(uint8_t) * (WIDTH * HEIGHT * 4);
}
double measureFrameRate(int frames, int width, int height) {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < frames; ++i) {
        // Simulate image processing (the underlying processing function)
        std::vector<uint8_t> baseImage(width * height * 4);
        std::vector<uint8_t> overlayImage(width * height * 4);
        std::vector<uint8_t> mask(width * height * 4);
        std::generate(baseImage.begin(), baseImage.end(), []() { return std::rand() % 256; });
        std::generate(overlayImage.begin(), overlayImage.end(), []() { return std::rand() % 256; });
        std::generate(mask.begin(), mask.end(), []() { return std::rand() % 256; });

        std::vector<uint8_t> resultImage(width * height * 4);
        processImage(baseImage, overlayImage, mask, resultImage);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    return frames / elapsed.count(); // Frames per second
}
int main() {
    const std::vector<std::pair<int, int>> resolutions = {
        {1280, 720},  // HD
        {1920, 1080}, // Full HD
        {3840, 2160}  // 4K
    };

    for (const auto& [width, height] : resolutions) {
        size_t memoryUsage = getMemoryUsage();
        double frameRate = measureFrameRate(100, width, height); // Measure over 100 frames

        std::cout << "Resolution: " << width << "x" << height << '\n';
        std::cout << "Memory Usage: " << memoryUsage / (1024.0 * 1024.0) << " MB\n"; // Convert to MB
        std::cout << std::fixed << std::setprecision(2) << "Frame Rate: " << frameRate << " FPS\n";
        std::cout << "--------\n";
    }

    return 0;
}