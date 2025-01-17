#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <memory>
#include <string>
#include <mutex>
#include <unordered_map>
#include <atomic>

// Forward declarations
class Context;
class StateFactory;
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

// StateFactory Class
class StateFactory {
public:
    static std::shared_ptr<StateFactory> getInstance();
    std::shared_ptr<State> createState(const std::string& stateName);
    void destroyState(const std::string& stateName);

private:
    StateFactory() = default;
    ~StateFactory() = default;

    std::unordered_map<std::string, std::atomic<std::shared_ptr<State>>> states;
    std::mutex registryMutex;

    // Static instance of the factory
    static std::shared_ptr<StateFactory> instance;
};

// Context Class
class Context {
public:
    explicit Context(const std::string& initialStateName)
        : currentState(StateFactory::getInstance()->createState(initialStateName)) {}

    void setState(const std::string& newStateName) {
        currentState = StateFactory::getInstance()->createState(newStateName);
    }

    void handle() {
        currentState->handle(*this);
    }

    std::string getCurrentStateName() const {
        return currentState->getName();
    }

private:
    std::shared_ptr<State> currentState;
};

#endif // STATEMACHINE_H
