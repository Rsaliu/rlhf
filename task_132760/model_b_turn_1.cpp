#include <iostream>
#include <librdkafka/rdkafkacpp.h>

int main() {
    RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    conf->set("bootstrap.servers", "localhost:9092", nullptr);

    RdKafka::Producer* producer = RdKafka::Producer::create(conf);

    RdKafka::Topic* topic = RdKafka::Topic::create(producer, "my-topic", nullptr, nullptr);

    for (int i = 0; i < 10000; i++) {
        RdKafka::ErrorCode err = producer->produce(topic, RdKafka::Partition::any(),
                                                   RdKafka::Producer::RK_MSG_COPY,
                                                   "Hello World!", strlen("Hello World!"),
                                                   nullptr, nullptr);

        if (err != RdKafka::ERR_NO_ERROR) {
            std::cerr << "Failed to produce message: " << err << std::endl;
        }
    }

    producer->flush(10000);

    delete topic;
    delete producer;
    delete conf;

    return 0;
}