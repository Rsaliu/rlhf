#include <iostream>
#include <string>
#include <curl/curl.h>

// Abstract Base Class for Webhook Handlers
class WebhookHandler {
public:
    virtual ~WebhookHandler() = default;
    virtual void handleWebhook(const std::string& payload) = 0;

protected:
    void sendWebhookRequest(const std::string& url, const std::string& payload);
};

// Concrete Webhook Handler for Service A
class ServiceAWebhookHandler : public WebhookHandler {
public:
    void handleWebhook(const std::string& payload) override {
        // Service A-specific processing
        // ...

        // Common webhook request
        sendWebhookRequest("https://example.com/service-a/webhook", payload);
    }
};

// Concrete Webhook Handler for Service B
class ServiceBWebhookHandler : public WebhookHandler {
public:
    void handleWebhook(const std::string& payload) override {
        // Service B-specific processing
        // ...

        // Common webhook request
        sendWebhookRequest("https://example.com/service-b/webhook", payload);
    }
};

// Implementation of the common webhook request using libcurl
void WebhookHandler::sendWebhookRequest(const std::string& url, const std::string& payload) {
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.size());
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}

int main() {
    // Usage example:
    ServiceAWebhookHandler serviceAHandler;
    serviceAHandler.handleWebhook("{\"key\": \"value\"}");

    ServiceBWebhookHandler serviceBHandler;
    serviceBHandler.handleWebhook("{\"another_key\": \"another_value\"}");
    return 0;
}