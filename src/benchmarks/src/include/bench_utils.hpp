#include <unordered_map>
#include <clipper/metrics.hpp>

namespace bench_utils {

/*
 * Keys for config
 */
const std::string CIFAR_DATA_PATH = "cifar_data_path";
const std::string NUM_THREADS = "num_threads";
const std::string NUM_BATCHES = "num_batches";
const std::string BATCH_SIZE = "batch_size";
const std::string BATCH_DELAY_MILLIS = "batch_delay_millis";
const std::string BATCH_DELAY_MICROS = "batch_delay_micros";
const std::string LATENCY_OBJECTIVE = "latency_objective";
const std::string REPORT_DELAY_SECONDS = "report_delay_seconds";
const std::string REPORTS_PATH = "reports_path";
const std::string REPORTS_PATH_VERBOSE = "reports_path_verbose";
const std::string POISSON_DELAY = "poisson_delay";
const std::string MODEL_NAME = "model_name";
const std::string MODEL_VERSION = "model_version";

class BenchMetrics {
  public:
    explicit BenchMetrics(std::string app_name)
      : app_name_(app_name),
        latency_(
            clipper::metrics::MetricsRegistry::get_metrics().create_histogram(
                    "app:" + app_name + ":prediction_latency", "microseconds",
                    4096)),
        throughput_(
            clipper::metrics::MetricsRegistry::get_metrics().create_meter(
                    "app:" + app_name + ":prediction_throughput")),
        num_predictions_(
            clipper::metrics::MetricsRegistry::get_metrics().create_counter(
                    "app:" + app_name + ":num_predictions")),
        accuracy_ratio_(
            clipper::metrics::MetricsRegistry::get_metrics().create_ratio_counter(
                    "app:" + app_name + ":accuracy")),
        default_pred_ratio_(
            clipper::metrics::MetricsRegistry::get_metrics()
                .create_ratio_counter("app:" + app_name +
                                      ":default_prediction_ratio")) {}
    ~BenchMetrics() = default;

    BenchMetrics(const BenchMetrics &) = default;

    BenchMetrics &operator=(const BenchMetrics &) = default;

    BenchMetrics(BenchMetrics &&) = default;

    BenchMetrics &operator=(BenchMetrics &&) = default;

    std::string app_name_;
    std::shared_ptr<clipper::metrics::Histogram> latency_;
    std::shared_ptr<clipper::metrics::Meter> throughput_;
    std::shared_ptr<clipper::metrics::Counter> num_predictions_;
    std::shared_ptr<metrics::RatioCounter> accuracy_ratio_;
    std::shared_ptr<clipper::metrics::RatioCounter> default_pred_ratio_;
};

/**
 * Creates a configuration from data received through the command prompt
 */
std::unordered_map<std::string, std::string> get_config_from_prompt(
    std::string setup_message, std::vector<std::string> desired_vars);

/**
 * Loads a configuration from the json file at the specified path
 */
std::unordered_map<std::string, std::string> get_config_from_json(
    std::string json_path, std::vector<std::string> desired_vars);

/**
 * Loads CIFAR data from the specified path
 */
std::unordered_map<int, std::vector<std::vector<double>>> load_cifar(
    std::unordered_map<std::string, std::string> &config);

/**
 * Returns a vector of all datapoints provided cifar data, ignoring labels.
 * Warning: this function mutates the input cifar data.
 */
std::vector<std::vector<double>> concatenate_cifar_datapoints(
    std::unordered_map<int, std::vector<std::vector<double>>> cifar_data);

/**
 * Returns the value corresponding to `key` in `config` as a string
 */
std::string get_str(const std::string &key,
                    std::unordered_map<std::string, std::string> &config);

/**
 * Returns the value corresponding to `key` in `config` as an int
 */
int get_int(const std::string &key,
            std::unordered_map<std::string, std::string> &config);

/**
* Returns the value corresponding to `key` in `config` as a long
*/
long get_long(const std::string &key,
              std::unordered_map<std::string, std::string> &config);

/**
 * Returns true iff the value corresponding to `key` in `config` is "true"
 */
bool get_bool(const std::string &key,
              std::unordered_map<std::string, std::string> &config);

}  // namespace bench_utils
