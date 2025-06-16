#include <chrono>
#include <iostream>
#include <random>
#include <thread>

#include "metrics.hpp"

int main() {
	MetricsWriter writer("metrics.log");

	auto cpuUtil =
		MetricsRegistry::Instance().CreateMetric<AveragingMetric>("CPU");
	auto httpRps =
		MetricsRegistry::Instance().CreateMetric<CountableMetric>("HTTP RPS");

	std::mt19937 rng((unsigned)std::time(nullptr));
	std::uniform_real_distribution<double> cpuDist(0.0, 2.0);
	std::poisson_distribution<int> httpDist(100.0);

	std::thread cpuThread([&]() {
		while (true) {
			double u = cpuDist(rng);
			cpuUtil->Add(u);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	});
	cpuThread.detach();

	std::thread httpThread([&]() {
		while (true) {
			int hits = httpDist(rng);
			httpRps->Increment(hits);
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	});
	httpThread.detach();

	std::this_thread::sleep_for(std::chrono::seconds(10));

	return 0;
}
