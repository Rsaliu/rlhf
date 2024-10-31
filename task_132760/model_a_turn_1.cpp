#include <iostream>
#include <string>
#include <librdkafka/rdkafkacpp.h>

int main() {
    const std::string brokers = "localhost:9092"; // Update with your Kafka broker address
    const std::string topic = "my-topic"; // Update with your topic name

    RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    conf->set("metadata.broker.list", brokers, errstr);

    // Create Kafka producer
    RdKafka::Producer* producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        return 1;
    }

    // Create topic handle
    RdKafka::Topic* topic_handle = RdKafka::Topic::create(producer, topic, NULL, errstr);
    if (!topic_handle) {
        std::cerr << "Failed to create topic handle: " << errstr << std::endl;
        delete producer;
        return 1;
    }

    // Produce messages
    int message_count = 10;
    for (int i = 0; i < message_count; ++i) {
        std::string message = "Message " + std::to_string(i);
        RdKafka::ErrorCode resp = producer->produce(topic_handle, RdKafka::Partition::ANY,
                                                   RdKafka::ProduceFlags::F_COPY,
                                                   const_cast<char*>(message.c_str()), message.size(),
                                                   nullptr, nullptr);

        if (resp != RdKafka::ERR_NO_ERROR) {
            std::cerr << "Failed to produce to topic " << topic << ": " << RdKafka::err2str(resp) << std::endl;
        }
    }

    // Poll to handle deliveries report callbacks
    producer->poll(0);

    // Wait for any outstanding messages to be delivered
    while (RdKafka::wait_destroyed(500) == 0) {
        producer->poll(500);
    }

    // Clean up
    delete topic_handle;
    delete producer;

    std::cout << "Message production completed." << std::endl;
    return 0;
}