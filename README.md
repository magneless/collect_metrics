# Metrics collecting and saving

## Classes

- **MetricInterface**
An abstract base class representing a generic metric.

- **CountableMetric**
Counts integer events (e.g., requests per second).
*Usage*: Call Increment() in producer threads. Value is reset after each Report().

- **AveragingMetric**
Accumulates real-valued samples and calculates their average per reporting interval.
*Usage*: Call Add(value) for each sample. Report() returns sum / count or 0 if no samples.

- **MetricsRegistry**
Singleton registry for managing all metrics.
*Usage*: Call MetricsRegistry::Instance().CreateMetric<YourMetric>("Name") to register and retrieve a shared pointer to your metric.

- **MetricsWriter**
Background writer that periodically dumps all metrics to a log file.
*Behavior*: Constructor opens the file and starts Run() in a new thread. Run() sleeps for one second, captures the current timestamp, retrieves all metrics via Snapshot(), writes their names and reported values, then flushes. Destructor stops the loop and joins the thread.

## Usage Example

You can see usage of this library by runing this commands:

1. Example Building:
```bash
mkdir build
cd build
cmake ..
cmake --build ./
cd ..
```

2. Example Running:
```bash
./build/main_exec
```

You can open **metrics.log** and observe receiving metrics.
