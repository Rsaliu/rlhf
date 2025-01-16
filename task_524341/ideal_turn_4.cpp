#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
#include <memory>
#include <list>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <sys/resource.h> // For memory usage
#include <sys/time.h>    

struct MemoryBlock {
    size_t size;          // Size of the object
    MemoryBlock* next;    // Link to the next free block
    // Additional data follows
};

class VariableSizeObjectPool {
public:
    VariableSizeObjectPool(size_t minSize, size_t maxSize) {
        for (size_t size = minSize; size <= maxSize; size += alignment) {
            pools[size] = std::make_unique<ObjectPool>(size);
        }
    }

    ~VariableSizeObjectPool() {
        pools.clear();
    }

    void* allocate(size_t size) {
        auto alignedSize = align_size(size);
        if (pools.find(alignedSize) != pools.end()) {
            return pools[alignedSize]->borrowObject();
        }
        return nullptr; // No suitable pool found
    }

    void deallocate(void* ptr, size_t size) {
        auto alignedSize = align_size(size);
        if (pools.find(alignedSize) != pools.end()) {
            pools[alignedSize]->returnObject(static_cast<MemoryBlock*>(ptr));
        } else {
            free(ptr);
        }
    }

private:
    size_t align_size(size_t size) const {
        return (size + (alignment - 1)) & ~(alignment - 1);
    }

    class ObjectPool {
    public:
        ObjectPool(size_t objectSize) : objectSize(objectSize), freeList(nullptr) {
            reservePool();
        }

        ~ObjectPool() {
            for (auto block : blocks) {
                free(block);
            }
        }

        MemoryBlock* borrowObject() {
            std::lock_guard<std::mutex> lock(mutex);
            if (freeList) {
                MemoryBlock* obj = freeList;
                freeList = freeList->next;
                return obj;
            }
            return nullptr; // Pool exhausted
        }

        void returnObject(MemoryBlock* obj) {
            std::lock_guard<std::mutex> lock(mutex);
            obj->next = freeList;
            freeList = obj;
        }

    private:
        void reservePool() {
            for (size_t i = 0; i < initialBlocksCount; ++i) {
                auto block = static_cast<MemoryBlock*>(malloc(objectSize + sizeof(MemoryBlock)));
                block->size = objectSize;
                block->next = freeList;
                freeList = block;
                blocks.push_back(block);
            }
        }

        const size_t objectSize;
        const size_t initialBlocksCount = 100;
        MemoryBlock* freeList;
        std::vector<MemoryBlock*> blocks;
        std::mutex mutex; // Protects access to the free list
    };

    const size_t alignment = 8; // Set alignment for objects
    std::map<size_t, std::unique_ptr<ObjectPool>> pools; // Map to hold object pools
};

class Benchmark {
public:
    static long getPeakMemoryUsage() {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        return usage.ru_maxrss; // Peak resident set size in kilobytes
    }

    static double getCPUTime() {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        return (usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6) +
               (usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6); // User + system time
    }

    void measureBaseline(int numThreads, int allocationsPerThread) {
        std::cout << "Running baseline (malloc/free)...\n";

        auto start = std::chrono::high_resolution_clock::now();
        double startCPU = getCPUTime();
        long startMemory = getPeakMemoryUsage();

        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back([=]() {
                for (int j = 0; j < allocationsPerThread; ++j) {
                    void* obj = malloc(100); // Allocate 100 bytes
                    if (obj) free(obj);      // Deallocate immediately
                }
            });
        }
        for (auto& t : threads) t.join();

        auto end = std::chrono::high_resolution_clock::now();
        double endCPU = getCPUTime();
        long endMemory = getPeakMemoryUsage();

        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Baseline Results:\n";
        std::cout << " - Latency: " << elapsed.count() << " seconds\n";
        std::cout << " - Peak Memory Usage: " << endMemory - startMemory << " KB\n";
        std::cout << " - CPU Utilization: " << endCPU - startCPU << " seconds\n";
    }

    void measureWithObjectPool(int numThreads, int allocationsPerThread, VariableSizeObjectPool& pool) {
        std::cout << "Running with object pool...\n";

        auto start = std::chrono::high_resolution_clock::now();
        double startCPU = getCPUTime();
        long startMemory = getPeakMemoryUsage();

        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back([&pool, allocationsPerThread]() {
                for (int j = 0; j < allocationsPerThread; ++j) {
                    void* obj = pool.allocate(100);
                    if (obj) pool.deallocate(obj, 100);
                }
            });
        }
        for (auto& t : threads) t.join();

        auto end = std::chrono::high_resolution_clock::now();
        double endCPU = getCPUTime();
        long endMemory = getPeakMemoryUsage();

        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Object Pool Results:\n";
        std::cout << " - Latency: " << elapsed.count() << " seconds\n";
        std::cout << " - Peak Memory Usage: " << endMemory - startMemory << " KB\n";
        std::cout << " - CPU Utilization: " << endCPU - startCPU << " seconds\n";
    }
};

int main() {
    Benchmark benchmark;
    VariableSizeObjectPool pool(16, 1024);

    const int numThreads = 8;
    const int allocationsPerThread = 100000;

    // Measure baseline
    benchmark.measureBaseline(numThreads, allocationsPerThread);

    // Measure with object pool
    benchmark.measureWithObjectPool(numThreads, allocationsPerThread, pool);

    return 0;
}
