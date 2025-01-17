#include "StateMachine.h"
#include <iostream>
#include <stdexcept>

// Implementation of StateA
void StateA::handle(Context& context) {
    std::cout << "StateA: Handling logic and transitioning to StateB...\n";
    context.setState("StateB");
}

// Implementation of StateB
void StateB::handle(Context& context) {
    std::cout << "StateB: Handling logic and transitioning to StateA...\n";
    context.setState("StateA");
}

// StateFactory methods
std::shared_ptr<StateFactory> StateFactory::instance = nullptr;

std::shared_ptr<StateFactory> StateFactory::getInstance() {
    if (!instance) {
        std::lock_guard<std::mutex> lock(registryMutex);
        if (!instance) {
            instance.reset(new StateFactory());
        }
    }
    return instance;
}

std::shared_ptr<State> StateFactory::createState(const std::string& stateName) {
    {
        std::lock_guard<std::mutex> lock(registryMutex);
        // Check if the state already exists
        if (states.find(stateName) != states.end()) {
            if (!states[stateName].load()) {
                states[stateName].store(std::make_shared<State>());
            }
            return states[stateName].load();
        }
    }

    // If state does not exist, create a new instance
    if (stateName == "StateA") {
        return std::make_shared<StateA>();
    } else if (stateName == "StateB") {
        return std::make_shared<StateB>();
    } else {
        throw std::runtime_error("Invalid state name: " + stateName);
    }
}

void StateFactory::destroyState(const std::string& stateName) {
    {
        std::lock_guard<std::mutex> lock(registryMutex);
        states[stateName].store(nullptr);
    }
}
