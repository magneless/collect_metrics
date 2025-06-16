#include <atomic>
#include <thread>
#include <fstream>
#include <mutex>
#include <memory>
#include <vector>

class MetricInterface {
public:
	virtual ~MetricInterface() = default;
	virtual std::string Name() const = 0;
	virtual std::string Report() = 0;
};

class CountableMetric : public MetricInterface {
public:
	CountableMetric(const std::string& name) : name_(name), value_(0){};

	std::string Name() const override { return name_; };
	std::string Report() override {
		int total = value_.exchange(0, std::memory_order_relaxed);
		return std::to_string(total);
	}
	void Increment(int amount = 1) {
		value_.fetch_add(amount, std::memory_order_relaxed);
	}

private:
	std::string name_;
	std::atomic<int> value_;
};

class AveragingMetric : public MetricInterface {
public:
	AveragingMetric(const std::string& name)
		: name_(name), sum_(0), count_(0){};

	std::string Name() const override { return name_; };
	std::string Report() override;
	void Add(double value) {
        sum_.fetch_add(value, std::memory_order_relaxed);
        count_.fetch_add(1, std::memory_order_relaxed);
    }

private:
	std::string name_;
	std::atomic<double> sum_;
	std::atomic<int> count_;
};

class MetricsRegistry {
public:
	static MetricsRegistry& Instance() {
		static MetricsRegistry registry;
		return registry;
	}

	template <typename MetricType, typename... Args>
	std::shared_ptr<MetricType> CreateMetric(Args&&... args) {
		auto m = std::make_shared<MetricType>(std::forward<Args>(args)...);
		std::lock_guard<std::mutex> lock(mutex_);
		metrics_.push_back(m);
		return m;
	}

	std::vector<std::shared_ptr<MetricInterface>> Snapshot() {
		std::lock_guard<std::mutex> lock(mutex_);
		return metrics_;
	}

private:
	MetricsRegistry() = default;
	std::mutex mutex_;
	std::vector<std::shared_ptr<MetricInterface>> metrics_;
};

class MetricsWriter {
public:
	explicit MetricsWriter(const std::string& filename)
		: file_(filename, std::ios::app), running_(true) {
		writer_thread_ = std::thread(&MetricsWriter::Run, this);
	}

	~MetricsWriter() {
		running_.store(false, std::memory_order_relaxed);
		if (writer_thread_.joinable())
            writer_thread_.join();
		file_.close();
	}

private:
	void Run();

	std::ofstream file_;
	std::atomic<bool> running_;
	std::thread writer_thread_;
};