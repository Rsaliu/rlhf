// File: supply_chain_system.cpp
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <unistd.h> // For usleep

// Load Shedder class implementation (same as before)
// ...

// Simple Order class
class Order {
public:
    Order(int id, int amount) : id(id), amount(amount) {}
    int id;
    int amount;
};

// Order Processing Class
class OrderProcessor {
public:
    OrderProcessor(int thread_count) : stop_(false) {
        for (int i = 0; i < thread_count; ++i) {
            workers_.emplace_back(&OrderProcessor::process_orders, this);
        }
    }

    ~OrderProcessor() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        cond_.notify_all();
        for (std::thread& worker : workers_) {
            worker.join();
        }
    }

    void enqueue_order(Order order) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            order_queue_.push(order);
        }
        cond_.notify_one();
    }

private:
    void process_orders() {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock, [this] { return stop_ || !order_queue_.empty(); });
            if (stop_ && order_queue_.empty()) {
                return;
            }

            Order order = order_queue_.front();
            order_queue_.pop();
            lock.unlock();

            // Simulate processing time
            usleep(rand() % 100000 + 50000); // 50-150 ms
        }
    }

    std::vector<std::thread> workers_;
    std::queue<Order> order_queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
    bool stop_;
};

// Reporting Module Class
class ReportingModule {
public:
    ReportingModule(int thread_count, LoadShedder& load_shedder) : stop_(false), load_shedder_(load_shedder) {
        for (int i = 0; i < thread_count; ++i) {
            workers_.emplace_back(&ReportingModule::generate_reports, this);
        }
    }
    
    ~ReportingModule() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        cond_.notify_all();
        for (std::thread& worker : workers_) {
            worker.join();
        }
    }

    void enqueue_report(int order_id) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            report_queue_.push(order_id);
        }
        cond_.notify_one();
    }

private:
    void generate_reports() {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock, [this] { return stop_ || !report_queue_.empty(); });
            if (stop_ && report_queue_.empty()) {
                return;
            }

            int order_id = report_queue_.front();
            report_queue_.pop();
            lock.unlock();

            // Simulate report generation time
            usleep(rand() % 100000 + 200000); // 200-3