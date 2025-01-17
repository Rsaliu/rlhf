#include "StateMachine.h"
#include <thread>
#include <vector>
#include <iostream>

// Simulate state handling logic in context
void simulate(Context& context, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        context.handle();
        std::cout << "Current State: " << context.getCurrentStateName() << "\n";
    }
}

int main() {
    auto initialState = StateFactory::createState("StateA");
    Context context(initialState);

    const int numThreads = 4; // Number of parallel threads
    const int iterations = 5; // State transitions per thread

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(simulate, std::ref(context), iterations);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}