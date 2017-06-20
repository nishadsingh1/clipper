#include <unordered_map>

namespace bench_utils {

/*
 * Keys for config
 */
const std::string CIFAR_DATA_PATH = "cifar_data_path";
const std::string NUM_THREADS = "num_threads";
const std::string NUM_BATCHES = "num_batches";
const std::string BATCH_SIZE = "batch_size";
const std::string BATCH_DELAY_MILLIS = "batch_delay_millis";
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

}  // namespace bench_utils
