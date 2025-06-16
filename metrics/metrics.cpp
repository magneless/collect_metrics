#include "metrics.hpp"

#include <chrono>
#include <sstream>
#include <iomanip>

std::string AveragingMetric::Report() {
	double total = sum_.exchange(0, std::memory_order_relaxed);
	int count = count_.exchange(0, std::memory_order_relaxed);
	return std::to_string(count > 0 ? total / count : 0);
}

void MetricsWriter::Run() {
	while (running_.load(std::memory_order_relaxed)) {
		std::this_thread::sleep_for(std::chrono::seconds(1));

		auto now = std::chrono::system_clock::now();
		auto now_t = std::chrono::system_clock::to_time_t(now);
		std::tm tm = *std::localtime(&now_t);

		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << '.' << std::setw(3)
			<< std::setfill('0') << ms;

		file_ << oss.str();

		auto metrics = MetricsRegistry::Instance().Snapshot();

		for (auto& m : metrics) {
			file_ << " \"" << m->Name() << "\" " << m->Report();
		}
		file_ << '\n';
		file_.flush();
	}
}