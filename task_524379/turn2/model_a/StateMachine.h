#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <memory>
#include <string>
#include <mutex>
#include <unordered_map>

// Forward declarations
class Context;
class StateFactory;
class State;

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

// State Factory
class StateFactory {
public:
    static std::shared_ptr<State> createState(const std::string& stateName) {
        std::lock_guard<std::mutex> lock(factoryMutex);
        if (stateName == "StateA") {
            return std::make_shared<StateA>();
        } 
        if (stateName == "StateB") {
            return std::make_shared<StateB>();
        }
        return nullptr; // or throw an exception
    }

    static void destroyState(std::shared_ptr<State> state) {
        std::lock_guard<std::mutex> lock(factoryMutex);
        // For demonstration, assumes shared_ptr will manage lifecycle.
        // Logic for cleanup could be placed here if needed
    }

private:
    static std::mutex factoryMutex; // Mutex to protect factory operations
};

// Context Class
class Context {
public:
    Context(std::shared_ptr<State> initialState)
        : currentState(std::move(initialState)) {}

    void setState(const std::string& stateName) {
        auto newState = StateFactory::createState(stateName);
        if (newState) {
            std::lock_guard<std::mutex> lock(stateMutex);
            currentState = std::move(newState);
        }
    }

    void handle() {
        currentState->handle(*this);
    }

    std::string getCurrentStateName() const {
        std::lock_guard<std::mutex> lock(stateMutex);
        return currentState->getName();
    }

private:
    std::shared_ptr<State> currentState; // Manage current state
    mutable std::mutex stateMutex;       // Mutex for current state access
};

#endif // STATEMACHINE_H