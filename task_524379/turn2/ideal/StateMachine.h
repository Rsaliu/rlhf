#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <memory>
#include <string>
#include <map>
#include <mutex>
#include <iostream>

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

// State Factory
class StateFactory {
public:
    std::shared_ptr<State> getState(const std::string& stateName) {
        std::lock_guard<std::mutex> lock(factoryMutex);
        if (stateCache.find(stateName) != stateCache.end()) {
            return stateCache[stateName];
        }

        std::shared_ptr<State> newState = createState(stateName);
        if (newState) {
            stateCache[stateName] = newState;
        }
        return newState;
    }

    void unloadState(const std::string& stateName) {
        std::lock_guard<std::mutex> lock(factoryMutex);
        stateCache.erase(stateName);
    }

private:
    std::shared_ptr<State> createState(const std::string& stateName) {
        if (stateName == "StateA") {
            return std::make_shared<StateA>();
        } else if (stateName == "StateB") {
            return std::make_shared<StateB>();
        }
        return nullptr;
    }

    std::map<std::string, std::shared_ptr<State>> stateCache;
    std::mutex factoryMutex;
};

// Context Class
class Context {
public:
    explicit Context(std::shared_ptr<StateFactory> factory, const std::string& initialStateName)
        : stateFactory(factory), currentState(factory->getState(initialStateName)) {}

    void setState(const std::string& stateName) {
        auto newState = stateFactory->getState(stateName);
        if (newState) {
            std::lock_guard<std::mutex> lock(stateMutex);
            currentState = std::move(newState);
        } else {
            std::cerr << "State " << stateName << " not found!\n";
        }
    }

    void handle() {
        std::shared_ptr<State> stateCopy;
        {
            std::lock_guard<std::mutex> lock(stateMutex);
            stateCopy = currentState;
        }
        stateCopy->handle(*this);
    }

    std::string getCurrentStateName() const {
        std::lock_guard<std::mutex> lock(stateMutex);
        return currentState->getName();
    }

private:
    std::shared_ptr<StateFactory> stateFactory;
    std::shared_ptr<State> currentState;
    mutable std::mutex stateMutex;
};

#endif // STATEMACHINE_H
