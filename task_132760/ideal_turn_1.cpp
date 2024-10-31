#include <iostream>
#include <string>
#include <librdkafka/rdkafkacpp.h>

int main() {
    const std::string brokers = "localhost:9092"; // Update with your Kafka broker address
    const std::string topic = "my-topic";         // Update with your topic name
    std::string errstr;

    // Create Kafka configuration
    RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    if (!conf) {
        std::cerr << "Failed to create global config" << std::endl;
        return 1;
    }

    if (conf->set("metadata.broker.list", brokers, errstr) != RdKafka::Conf::CONF_OK) {
        std::cerr << "Failed to set broker list: " << errstr << std::endl;
        delete conf;
        return 1;
    }

    // Create Kafka producer
    RdKafka::Producer* producer = RdKafka::Producer::create(conf, errstr);
    delete conf;  // Configuration is no longer needed after the producer is created
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        return 1;
    }

    // Create topic handle
    RdKafka::Topic* topic_handle = RdKafka::Topic::create(producer, topic, nullptr, errstr);
    if (!topic_handle) {
        std::cerr << "Failed to create topic handle: " << errstr << std::endl;
        delete producer;
        return 1;
    }

    // Produce messages
    int message_count = 10;
    for (int i = 0; i < message_count; ++i) {
        std::string message = "Message " + std::to_string(i);
        RdKafka::ErrorCode resp = producer->produce(topic_handle, RdKafka::Topic::PARTITION_UA,
                                                    RdKafka::Producer::RK_MSG_COPY,
                                                    message.data(), message.size(),
                                                    nullptr, nullptr);

        if (resp != RdKafka::ERR_NO_ERROR) {
            std::cerr << "Failed to produce to topic " << topic << ": " << RdKafka::err2str(resp) << std::endl;
        }

        // Poll to handle delivery report callbacks
        producer->poll(0);
    }

    // Wait for any outstanding messages to be delivered
    while (producer->outq_len() > 0) {
        producer->poll(100);
    }

    // Clean up
    delete topic_handle;
    delete producer;

    // Wait for destruction of internal Kafka objects
    RdKafka::wait_destroyed(5000);

    std::cout << "Message production completed." << std::endl;
    return 0;
}
