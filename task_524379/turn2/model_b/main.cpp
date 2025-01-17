#include "StateMachine.h"
#include <thread>
#include <vector>
#include <iostream>
#include <random>

void simulate(Context& context, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        context.handle();
        std::cout << "Current State: " << context.getCurrentStateName() << "\n";
        if (i % 2 == 0) {
            std::string stateToDestroy;
            // Randomly decide to destroy a state (for demonstration purposes)
            if (std::rand() % 2 == 0) {
                stateToDestroy = context.getCurrentStateName() == "StateA" ? "StateB" : "StateA";
                StateFactory::getInstance()->destroyState(stateToDestroy);
                std::cout << "Destroyed state: " << stateToDestroy << "\n";
            }
        }
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    auto initialState = std::string("StateA");
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
