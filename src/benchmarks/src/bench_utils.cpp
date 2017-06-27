#include <clipper/json_util.hpp>
#include <clipper/logging.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "include/bench_utils.hpp"

using namespace clipper;

namespace bench_utils {

constexpr int CIFAR_PLANE_INDEX = 0;
constexpr int CIFAR_BIRD_INDEX = 2;
constexpr double SKLEARN_PLANE_LABEL = 1;
constexpr double SKLEARN_BIRD_LABEL = 0;

BenchMetrics::BenchMetrics(std::string app_name) {
  latency_ = clipper::metrics::MetricsRegistry::get_metrics().create_histogram(
      "app:" + app_name + ":prediction_latency", "microseconds", 4096);
  throughput_ = clipper::metrics::MetricsRegistry::get_metrics().create_meter(
      "app:" + app_name + ":prediction_throughput");
  send_rate_ = clipper::metrics::MetricsRegistry::get_metrics().create_meter(
      "app:" + app_name + ":send_rate");
  num_predictions_ =
      clipper::metrics::MetricsRegistry::get_metrics().create_counter(
          "app:" + app_name + ":num_predictions");
  accuracy_ratio_ =
      clipper::metrics::MetricsRegistry::get_metrics().create_ratio_counter(
          "app:" + app_name + ":accuracy_ratio");
  default_pred_ratio_ =
      clipper::metrics::MetricsRegistry::get_metrics().create_ratio_counter(
          "app:" + app_name + ":default_prediction_ratio");
}

std::unordered_map<std::string, std::string> get_config_from_json(
    std::string json_path, std::vector<std::string> desired_vars) {
  std::ifstream json_file(json_path);
  std::stringstream buffer;
  buffer << json_file.rdbuf();
  std::string json_text = buffer.str();
  rapidjson::Document d;
  json::parse_json(json_text, d);

  std::unordered_map<std::string, std::string> responses;
  std::string response;
  for (std::string desired_var : desired_vars) {
    response = json::get_string(d, desired_var.c_str());
    responses[desired_var] = response;
  }
  return responses;
}

std::unordered_map<int, std::vector<std::vector<double>>> load_cifar(
    std::string &cifar_data_path) {
  // A loose check to ensure that the binary dataset (not the python-compatible
  // dataset) is being used
  if (cifar_data_path.find(".bin") == std::string::npos) {
    log_error(
        "BENCH",
        "Please specify the full path of the binary CIFAR-100 data file.");
    std::terminate();
  }

  std::ifstream cifar_file(cifar_data_path, std::ios::binary);
  std::istreambuf_iterator<char> cifar_data(cifar_file);
  std::unordered_map<int, std::vector<std::vector<double>>> vecs_map;
  for (int i = 0; i < 10000; i++) {
    int label = static_cast<int>(*cifar_data);
    cifar_data++;
    std::vector<uint8_t> cifar_byte_vec;
    cifar_byte_vec.reserve(3072);
    std::copy_n(cifar_data, 3072, std::back_inserter(cifar_byte_vec));
    cifar_data++;
    std::vector<double> cifar_double_vec(cifar_byte_vec.begin(),
                                         cifar_byte_vec.end());

    std::unordered_map<int, std::vector<std::vector<double>>>::iterator
        label_vecs = vecs_map.find(label);
    if (label_vecs != vecs_map.end()) {
      label_vecs->second.push_back(cifar_double_vec);
    } else {
      std::vector<std::vector<double>> new_label_vecs;
      new_label_vecs.push_back(cifar_double_vec);
      vecs_map.emplace(label, new_label_vecs);
    }
  }
  return vecs_map;
}

std::pair<std::vector<std::vector<double>>, std::vector<double>>
concatenate_cifar_datapoints(
    std::unordered_map<int, std::vector<std::vector<double>>> cifar_data) {
  std::vector<std::vector<double>> planes_vecs =
      cifar_data.find(CIFAR_PLANE_INDEX)->second;
  std::vector<std::vector<double>> birds_vecs =
      cifar_data.find(CIFAR_BIRD_INDEX)->second;

  std::vector<double> labels(planes_vecs.size() + birds_vecs.size());
  for (size_t i = 0; i < labels.size(); i++) {
    if (i < planes_vecs.size()) {
      labels[i] = SKLEARN_PLANE_LABEL;
    } else {
      labels[i] = SKLEARN_BIRD_LABEL;
    }
  }

  planes_vecs.insert(planes_vecs.end(), birds_vecs.begin(), birds_vecs.end());

  return std::make_pair(planes_vecs, labels);
}

std::string get_str(const std::string &key,
                    std::unordered_map<std::string, std::string> &config) {
  auto entry = config.find(key);
  if (entry == config.end()) {
    std::stringstream ss;
    ss << "Key '" << key << "' does not exist in config.";
    throw std::invalid_argument(ss.str());
  }
  return entry->second;
}

int get_int(const std::string &key,
            std::unordered_map<std::string, std::string> &config) {
  return std::stoi(get_str(key, config));
}

long get_long(const std::string &key,
              std::unordered_map<std::string, std::string> &config) {
  return static_cast<long>(get_int(key, config));
}

bool get_bool(const std::string &key,
              std::unordered_map<std::string, std::string> &config) {
  return get_str(key, config) == "true";
}

}  // namespace bench_utils
