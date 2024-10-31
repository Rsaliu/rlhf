#include <iostream>
#include <string>
#include <vector>
#include <atomic>
#include <librdkafka/rdkafkacpp.h>

class KafkaProducer {
    public:
    KafkaProducer(const std::string& brokers, const std::string& topic): brokers(brokers), topic(topic) {
        // Initialize Kafka producer
        // (Same code as before)
    }

    void produceMessage(const std::string& message) {
        // Produce a message using the existing producer instance
        // (Same code as before)
    }

    private:
    std::string brokers;
    std::string topic;
    // Kafka producer handle
    RdKafka::Producer* producer;
    // Topic handle
    RdKafka::Topic* topic_handle;
};

class MessageProcessor {
    public:
    MessageProcessor(const std::string& brokers, const std::string& topic, int num_producers) {
        for (int i = 0; i < num_producers; ++i) {
            producers.push_back(new KafkaProducer(brokers, topic));
        }
    }

    void processMessage(const std::string& message) {
        // Select a producer randomly to distribute the load
        std::size_t random_index = std::rand() % producers.size();
        producers[random_index]->produceMessage(message);
    }

    void poll() {
        // Poll for producer delivery reports (Optional)
        // (Same code as before)
    }

    private:
    std::vector<KafkaProducer*> producers;
};

int main() {
    const std::string brokers = "localhost:9092";         // Update with your Kafka broker address
    const std::string topic = "my-topic";                 // Update with your topic name
    const int initial_num_producers = 1;                   // Set the initial number of producers

    // Create MessageProcessor instance with initial number of producers
    MessageProcessor messageProcessor(brokers, topic, initial_num_producers);

    // Variable to track load
    int load_threshold = 1000; // Set an appropriate load threshold
    std::atomic<int> message_count{0};

    // Main processing loop
    while (true) {
        // Receive message from Kafka consumer (Using another thread/polling mechanism)
        // For demonstration purposes, we'll increment the message count manually.
        message_count++;

        // Process the message
        std::string message = "Message"; // Your actual message here
        messageProcessor.processMessage(message);

        // Check if the load threshold is reached
        if (message_count.load() > load_threshold) {
            // Increase the number of producers
            int new_num_producers = messageProcessor.producers.size() * 2; // Doubling the number of producers
            messageProcessor.producers.reserve(new_num_producers);
            for (int i = messageProcessor.producers.size(); i < new_num_producers; ++i) {
                messageProcessor.producers.push_back(new KafkaProducer(brokers, topic));