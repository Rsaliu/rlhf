#include "StateMachine.h"

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
