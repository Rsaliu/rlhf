#include "StateMachine.h"
#include <iostream>

std::mutex StateFactory::factoryMutex; // Definition of the static mutex

// Implementation of StateA
void StateA::handle(Context& context) {
    std::cout << "StateA: Handling logic and transitioning to StateB...\n";
    context.setState("StateB");
    StateFactory::destroyState(shared_from_this()); // Optional: manage lifecycle
}

// Implementation of StateB
void StateB::handle(Context& context) {
    std::cout << "StateB: Handling logic and transitioning to StateA...\n";
    context.setState("StateA");
    StateFactory::destroyState(shared_from_this()); // Optional: manage lifecycle
}