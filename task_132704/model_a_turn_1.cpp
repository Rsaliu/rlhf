#include <iostream>
#include <string>
#include <map>
#include <memory>
// Base WebhookHandler interface
class WebhookHandler {
public:
	virtual void handleWebhook(const std::string& payload) = 0;
	virtual ~WebhookHandler() = default;
};

// Concrete WebhookHandlers for different types of webhooks
class OrderWebhookHandler : public WebhookHandler {
public:
	void handleWebhook(const std::string& payload) override {
		std::cout << "Handling Order Webhook: " << payload << std::endl;
		// Order-specific processing here
	}
};

class PaymentWebhookHandler : public WebhookHandler {
public:
	void handleWebhook(const std::string& payload) override {
		std::cout << "Handling Payment Webhook: " << payload << std::endl;
		// Payment-specific processing here
	}
};

// WebhookHandlerFactory to create appropriate handler based on type
class WebhookHandlerFactory {
public:
	std::unique_ptr<WebhookHandler> createHandler(const std::string& type) {
		auto handler = handlers.find(type);
		if (handler != handlers.end()) {
			return handler->second();
		}
		return nullptr;
	}

private:
	using HandlerCreator = std::function<std::unique_ptr<WebhookHandler>()>;
	std::map<std::string, HandlerCreator> handlers = {
		{"order", []() { return std::make_unique<OrderWebhookHandler>(); }},
		{"payment", []() { return std::make_unique<PaymentWebhookHandler>(); }},
	};
};

// Main function to handle webhooks
int main() {
	WebhookHandlerFactory factory;

	std::string webhookType1 = "order";
	std::string webhookPayload1 = "{\"order_id\": 123}";

	std::string webhookType2 = "payment";
	std::string webhookPayload2 = "{\"payment_id\": 456}";

	if (auto handler = factory.createHandler(webhookType1)) {
		handler->handleWebhook(webhookPayload1);
	} else {
		std::cout << "Invalid webhook type: " << webhookType1 << std::endl;
	}

	if (auto handler = factory.createHandler(webhookType2)) {
		handler->handleWebhook(webhookPayload2);
	} else {
		std::cout << "Invalid webhook type: " << webhookType2 << std::endl;
	}
	return 0;
}