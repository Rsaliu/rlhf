void applyDynamicMask(const std::vector<uint8_t>& overlay, const std::vector<uint8_t>& mask, std::vector<uint8_t>& output) {
    assert(overlay.size() == mask.size());
    for (size_t i = 0; i < overlay.size(); ++i) {
        output[i] = overlay[i] & mask[i]; // Apply bitwise AND with the mask
    }
}
int main() {
    // Initialize random seed for demo
    std::srand(std::time(nullptr));

    // Prepare overlay and mask data
    std::vector<uint8_t> overlay(WIDTH * HEIGHT);
    std::vector<uint8_t> mask(WIDTH * HEIGHT);
    std::vector<uint8_t> output(WIDTH * HEIGHT);

    for (size_t i = 0; i < overlay.size(); ++i) {
        overlay[i] = std::rand() % 256; // Fill with random overlay pixel values
        mask[i] = (std::rand() % 2) * 255; // Fill with random mask values (0 or 255) for dynamic masking
    }

    // Measure performance for dynamic masking
    auto start = std::chrono::high_resolution_clock::now();
    applyDynamicMask(overlay, mask, output);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dynamicMaskTime = end - start;
    std::cout << "Dynamic Masking Time: " << dynamicMaskTime.count() << " seconds.\n";

    return 0;
}