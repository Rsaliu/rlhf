#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <memory>
#include <string>
#include <mutex>
// Forward declarations
class Context;
class StateA;
class StateB;

// State Interface
class State {
public:
    virtual ~State() = default;

    virtual void handle(Context& context) = 0;
    virtual std::string getName() const = 0;
};

// Concrete States
class StateA : public State {
public:
    void handle(Context& context) override;
    std::string getName() const override { return "StateA"; }
};

class StateB : public State {
public:
    void handle(Context& context) override;
    std::string getName() const override { return "StateB"; }
};

// Context Class
class Context {
public:
    explicit Context(std::shared_ptr<State> initialState)
        : currentState(std::move(initialState)) {}

    void setState(std::shared_ptr<State> newState) {
        std::lock_guard<std::mutex> lock(stateMutex);
        currentState = std::move(newState);
    }

    void handle() {
        currentState->handle(*this);
    }

    std::string getCurrentStateName() const {
        std::lock_guard<std::mutex> lock(stateMutex);
        return currentState->getName();
    }

private:
    std::shared_ptr<State> currentState;
    mutable std::mutex stateMutex;
};

#endif // STATEMACHINE_H
