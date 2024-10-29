#include <iostream>
#include <string>
#include <vector>
#include <memory>

// Abstract Base Class for Webhook Handlers
class WebhookHandler {
public:
    virtual ~WebhookHandler() = default;
    virtual void handleWebhook(const std::string& payload) = 0;
};

// Concrete Webhook Handlers for specific events (e.g., 'order_placed', 'payment_received')
class OrderPlacedHandler : public WebhookHandler {
public:
    void handleWebhook(const std::string& payload) override {
        // Process order placement event
        // ...
        std::cout << "Handling Order Placed event." << std::endl;
    }
};

class PaymentReceivedHandler : public WebhookHandler {
public:
    void handleWebhook(const std::string& payload) override {
        // Process payment received event
        // ...
        std::cout << "Handling Payment Received event." << std::endl;
    }
};

//  **Context Class that uses the Strategy Pattern**
class WebhookDispatcher {
public:
    void registerHandler(const std::string& event, std::unique_ptr<WebhookHandler> handler) {
        handlers[event] = std::move(handler);
    }

    void handleWebhook(const std::string& event, const std::string& payload) {
        auto it = handlers.find(event);
        if (it != handlers.end()) {
            it->second->handleWebhook(payload);
        } else {
            std::cout << "No handler found for event: " << event << std::endl;
        }
    }

private:
    std::unordered_map<std::string, std::unique_ptr<WebhookHandler>> handlers;
};

int main() {
    // Usage example:
    WebhookDispatcher dispatcher;
    
    // Register event handlers for different services
    dispatcher.registerHandler("order_placed", std::make_unique<OrderPlacedHandler>());
    dispatcher.registerHandler("payment_received", std::make_unique<PaymentReceivedHandler>());

    // Handle webhooks for different events
    dispatcher.handleWebhook("order_placed", "{\"order_id\": 123}");
    dispatcher.handleWebhook("payment_received", "{\"payment_id\": 456}");
    dispatcher.handleWebhook("unknown_event", "{}");
    return 0;
}  