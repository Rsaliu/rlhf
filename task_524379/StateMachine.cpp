#include "StateMachine.h"
#include <iostream>

// Implementation of StateA
void StateA::handle(Context& context) {
    std::cout << "StateA: Handling logic and transitioning to StateB...\n";
    context.setState(std::make_shared<StateB>());
}

// Implementation of StateB
void StateB::handle(Context& context) {
    std::cout << "StateB: Handling logic and transitioning to StateA...\n";
    context.setState(std::make_shared<StateA>());
}
