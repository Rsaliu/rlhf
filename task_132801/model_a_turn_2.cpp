#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <queue>
#include <chrono>
#include <random>

// Third-party library for CPU usage measurement
#include <psapi.h>

class LoadShedder {
public:
	LoadShedder(double threshold_cpu_usage, size_t worker_threads) : threshold_cpu_usage_(threshold_cpu_usage), stop_(false), worker_threads_(worker_threads) {
		for (size_t i = 0; i < worker_threads_; ++i) {
			workers_.emplace_back(&LoadShedder::worker, this);
		}
		monitor_thread_ = std::thread(&LoadShedder::run, this);
	}

	~LoadShedder() {
		{
			std::unique_lock<std::mutex> lock(mutex_);
			stop_ = true;
		}
		cond_.notify_all();
		for (std::thread& worker : workers_) {
			worker.join();
		}
		monitor_thread_.join();
	}

	void submit_task(const std::function<void()>& task) {
		{
			std::unique_lock<std::mutex> lock(mutex_);
			tasks_.push(task);
		}
		cond_.notify_one();
	}

private:
	void run() {
		std::unique_lock<std::mutex> lock(mutex_);
		while (!stop_) {
			cond_.wait(lock, [this] { return stop_ || !tasks_.empty() || load_shed_; });

			if (load_shed_) {
				// Implement load shedding actions here
				std::cout << "Load shedding initiated... Reducing worker threads." << std::endl;

				// Simulate load shedding duration
				std::this_thread::sleep_for(std::chrono::seconds(5));

				load_shed_ = false;

				// Reduce the number of active worker threads
				size_t num_threads_to_shed = worker_threads_ / 4;
				for (size_t i = 0; i < num_threads_to_shed; ++i) {
					if (!workers_.empty()) {
						workers_.back().join();
						workers_.pop_back();
						worker_threads_--;
					}
				}
			}

			if (!tasks_.empty()) {
				auto task = std::move(tasks_.front());
				tasks_.pop();
				lock.unlock();
				task();
			}
		}
	}

	void worker() {
		std::unique_lock<std::mutex> lock(mutex_);
		while (!stop_) {
			cond_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

			if (!tasks_.empty()) {
				auto task = std::move(tasks_.front());
				tasks_.pop();
				lock.unlock();
				task();
			}
		}
	}

	double get_current_cpu_usage() {
		FILETIME creationTime, exitTime, kernelTime, userTime;
		if (!GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime)) {
			return 0.0;
		}

		auto sysTime = kernelTime + userTime;
		auto timeDelta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() -
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::from_time_t(creationTime.dwLowDateTime)).count();

		if (timeDelta == 0) {
			return 0.0;
		}

		return static_cast<double>(sysTime.dwLowDateTime) / timeDelta;
	}

	void check_load() {
		double current_cpu_usage = get_current_cpu_usage();
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (current_cpu_usage > threshold_cpu_usage_) {
				load_shed_ = true;
				cond_.notify_one();
			}
		}
	}

	std::vector<std::thread> workers_;
	std::thread monitor_thread_;
	std::mutex mutex_;
	std::condition_variable cond_;
	std::queue<std::function<void()>> tasks_;
	bool stop_;
	bool load_shed_;
	double threshold_cpu_usage_;
	size_t worker_threads_;
};

// Simulate task that performs analysis
void perform_analysis() {
	std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 50 + 1));
}

int main() {
	// Install psapi.h: sudo apt-get install libpapi-dev

	LoadShedder loadShedder(90.0, std::thread::hardware_concurrency());

	// Simulate high demand by submitting tasks continuously
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(1, 10);

	while (true) {
		for (int i = 0; i < dist(gen); ++i) {
			loadShedder.submit_task(perform_analysis);
		}

		// Check load periodically
		loadShedder.check_load();

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}