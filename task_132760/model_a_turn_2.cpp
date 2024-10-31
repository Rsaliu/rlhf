
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <functional>

class DynamicThreadPool {
public:
	DynamicThreadPool(size_t minThreads = 1, size_t maxThreads = std::thread::hardware_concurrency()) :
		minThreads(minThreads), maxThreads(maxThreads), stop(false) {
		for (size_t i = 0; i < minThreads; ++i) {
			workers.emplace_back([this] {
				for (;;) {
					std::function<void()> task;

					{
						std::unique_lock<std::mutex> lock(queueMutex);
						condition.wait(lock, [this] { return stop || !tasks.empty(); });
						if (stop && tasks.empty()) {
							return;
						}
						task = std::move(tasks.front());
						tasks.pop();
					}

					task();
				}
			});
		}
	}

	template<class F, class... Args>
	auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
		using return_type = typename std::result_of<F(Args...)>::type;

		auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(queueMutex);

			if (stop) {
				throw std::runtime_error("enqueue on stopped ThreadPool");
			}

			tasks.emplace([task]() { (*task)(); });
		}
		condition.notify_one();
		return res;
	}

	void stopThreads() {
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			stop = true;
		}
		condition.notify_all();
		for (std::thread& worker : workers) {
			worker.join();
		}
	}

	// Add method to dynamically adjust thread pool size
	void adjustThreadPoolSize(size_t newSize) {
		std::unique_lock<std::mutex> lock(queueMutex);

		size_t currentSize = workers.size();

		if (newSize > currentSize) {
			// Spawn new threads
			for (size_t i = currentSize; i < newSize; ++i) {
				workers.emplace_back([this] {
					for (;;) {
						std::function<void()> task;

						{
							std::unique_lock<std::mutex> lock(queueMutex);
							condition.wait(lock, [this] { return stop || !tasks.empty(); });
							if (stop && tasks.empty()) {
								return;
							}
							task = std::move(tasks.front());
							tasks.pop();
						}

						task();
					}
				});
			}
		} else if (newSize < currentSize) {
			// Join excess threads
			for (size_t i = 0; i < (currentSize - newSize); ++i) {
				{
					std::unique_lock<std::mutex> lock(queueMutex);
					stop = true;
				}
				condition.notify_one();
				workers.back().join();
				workers.pop_back();
			}
		}
	}

private:
	// The thread pool contains a vector of workers
	std::vector<std::thread> workers;

	// The pool uses a queue of tasks to be completed
	std::queue<std::function<void()>> tasks;

	// Mutex and condition variable to ensure thread safety
	std::mutex queueMutex;
	std::condition_variable condition;
	bool stop;

	const size_t minThreads;
	const size_t maxThreads;
};

 // Example usage:
int main() {
	DynamicThreadPool pool;

	// Simulate dynamic load changes
	for (int i = 0; i < 10; ++i) {
		// Add tasks to the thread pool
		for (int j = 0; j < 100; ++j) {
			pool.enqueue([]() {
				// Simulate work
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			});
		}

		// Simulate load increase, scale up thread pool
		size_t newThreadCount = std::min(pool.maxThreads, pool.workers.size() * 2);
		std::cout << "Scaling up to " << newThreadCount << " threads" << std::endl;
		pool.adjustThreadPoolSize(newThreadCount);

		// Simulate work duration
		std::this_thread::sleep_for(std::chrono::seconds(2));

		// Simulate load decrease, scale down thread pool
		newThreadCount = std::max(pool.minThreads, pool.workers.size() / 2);
		std::cout << "Scaling down to " << newThreadCount << " threads" << std::endl;
		pool.adjustThreadPoolSize(newThreadCount);
	}

	pool.stopThreads();
	return 0;
}