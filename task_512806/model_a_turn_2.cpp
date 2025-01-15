#include <iostream>
#include <string>
#include <map>
#include <memory>

// Abstract Base Class for Webhook Handlers
class WebhookHandler {
public:
	virtual ~WebhookHandler() = default;
	virtual void handleWebhook(const std::string& payload) = 0;
};

// Base Strategy Class for Webhook Responses
class WebhookResponseStrategy {
public:
	virtual ~WebhookResponseStrategy() = default;
	virtual void sendResponse(const std::string& url, const std::string& payload) = 0;
};

// Concrete Strategy Class for Common Webhook Response
class CommonWebhookResponse : public WebhookResponseStrategy {
public:
	void sendResponse(const std::string& url, const std::string& payload) override {
		// Implementation of common webhook response logic
		std::cout << "Sending common webhook response to: " << url << std::endl;
	}
};

// Concrete Webhook Handlers using Strategy Pattern
class ServiceAWebhookHandler : public WebhookHandler {
public:
	ServiceAWebhookHandler(std::unique_ptr<WebhookResponseStrategy> responseStrategy) : responseStrategy(std::move(responseStrategy)) {}

	void handleWebhook(const std::string& payload) override {
		// Service A-specific processing
		// ...

		responseStrategy->sendResponse("https://example.com/service-a/webhook", payload);
	}

private:
	std::unique_ptr<WebhookResponseStrategy> responseStrategy;
};

// WebhookHandler Factory
class WebhookHandlerFactory {
public:
	static std::unique_ptr<WebhookHandler> createHandler(const std::string& serviceName) {
		if (serviceName == "ServiceA") {
			return std::make_unique<ServiceAWebhookHandler>(std::make_unique<CommonWebhookResponse>());
		} 
		// Add more handler creations for other services here

		return nullptr;
	}
};

int main() {
	std::string serviceName = "ServiceA";
	auto handler = WebhookHandlerFactory::createHandler(serviceName);
	if (handler) {
		handler->handleWebhook("{\"key\": \"value\"}");
	} else {
		std::cout << "Invalid service name: " << serviceName << std::endl;
	}
	return 0;
}