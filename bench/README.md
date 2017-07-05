# Running the performance benchmark

We have provided a script to benchmark Clipper's performance. The tool bypasses frontend REST APIs, but beyond that, it should test Clipper end to end. To make use of it, you need to:

* Download the CIFAR dataset
* Get a model-container up and running (or use one of our predefined scripts)
* Have a Redis server running for Clipper to connect to
* Define your desired benchmarking parameters
* Run the tool

This document goes over details on how to use the tool.

## Download dataset(s)

### Download CIFAR10 Binary Dataset for Query Execution
The C++ benchmark works by sending CIFAR10 query vectors to the container serving the trained SKLearn model. To achieve this, the **binary dataset** is required. It can also be obtained from [https://www.cs.toronto.edu/~kriz/cifar.html](https://www.cs.toronto.edu/~kriz/cifar.html). You can click [here](https://www.cs.toronto.edu/~kriz/cifar-10-binary.tar.gz) to download directly.

You'll want to unzip the .tar.gz file manually.


### (Optional) Download CIFAR10 Python Dataset for SKLearn Model 
You can skip this section if you do not intend to use our SKLearn Model for benchmarking.

Our SKLearn model depends on the CIFAR10 Python dataset for training. In order to train the model with this dataset, it must be converted to CSV format. The [CIFAR10 python download utility](../examples/tutorial/download_cifar.py) can be used to obtain the required CSV-formatted dataset.

```sh
../examples/tutorial/download_cifar.py /path/to/save/dataset
```


## Deploy a model container to query

The benchmarking tool will send requests through Clipper to a model. You'll want to deploy this model in a container that implements Clipper's Model-Container RPC interface.

You can also use one of our predefined model-container scripts:

- If you want to run the model outside of a Docker container

  - [`bench/setup_noop_bench.sh`](https://github.com/ucbrise/clipper/tree/develop/bench/setup_noop_bench.sh) runs the [`noop_container`](https://github.com/ucbrise/clipper/blob/develop/containers/python/noop_container.py)
  - [`bench/setup_sum_bench.sh`](https://github.com/ucbrise/clipper/tree/develop/bench/setup_sum_bench.sh) runs the [`sum_container`](https://github.com/ucbrise/clipper/blob/develop/containers/python/sum_container.py)
  - [`bench/setup_sklearn_bench.sh`](https://github.com/ucbrise/clipper/tree/develop/bench/setup_sklearn_bench.sh) runs the [`sklearn_cifar_container`](https://github.com/ucbrise/clipper/blob/develop/containers/python/sklearn_cifar_container). If you wish to use this option, remember to download the CIFAR10 python dataset and run
  ```sh
  ./setup_bench.sh <path_to_cifar_python_dataset>
  ```
where `<path_to_cifar_python_dataset>` is the path to the **directory** containing a parsed CIFAR10 CSV data file with name `cifar_train.data`.

- If you want to deploy the model in a Docker container on an AWS instance, you'll need to build the model container Docker image.
  - Create the Docker images for the [`noop_container`](https://github.com/ucbrise/clipper/blob/develop/containers/python/noop_container.py)  and [`sum_container`](https://github.com/ucbrise/clipper/blob/develop/containers/python/sum_container.py) by running `./bench/build_bench_docker_images.sh`.
  - Run `docker run <image_id>` on the same host that the benchmarking tool will be run from.

When creating your JSON configuration file (the whole procedure is detailed in the section below), you'll need to provide a `model_name` and `model_version`. When using one of our predefined model-container scripts, set the `model_version` to 1 and `model_name` to:

- `bench_noop` for the [`noop_container`](https://github.com/ucbrise/clipper/blob/develop/containers/python/noop_container.py)
- `bench_sum` for the [`sum_container`](https://github.com/ucbrise/clipper/blob/develop/containers/python/sum_container.py)
- `bench_sklearn_cifar` for the [`sklearn_cifar_container`](https://github.com/ucbrise/clipper/blob/develop/containers/python/sklearn_cifar_container)


## Define your benchmarking parameters

Create a JSON configuration file that specifies values for the following parameters:

- **cifar\_data_path**: The path to a *specific binary data file* within the CIFAR10 binary dataset with a name of the form `data_batch_<n>.bin`. For example, `<path_to_unzipped_cifar_directory>/data_batch_1.bin`.
- **num_threads**: The number of threads used to send benchmark requests. This should be a positive integer. Each of these threads will access its own copy of the data and will send requests according to *num_batches*, *request\_batch_size*, *request\_batch\_delay_micros*, *poisson_delay*, and *prevent\_cache_hits*, all described below.
- **num_batches**: The total number of request batches to be sent by each thread.
- **request\_batch_size**: The number of requests to be sent in each batch.
- **request\_batch\_delay_micros**: The per-thread delay between batches, in microseconds. *request\_batch\_delay_micros* and *request\_batch_size* together determine the burstiness of your supplied workload.
- **poisson_delay**: `"true"` if you wish for the delays between request batches to be drawn from a poisson distribution with mean `request_batch_delay_micros`. `"false"` if you wish for the delay between request batches to be uniform.
- **prevent\_cache_hits**: `"true"` if you wish for the script to modify datapoints (possibly at the expense of prediction accuracy) in order to prevent hitting Clipper's internal prediction cache. `"false"` otherwise.
- **latency_objective**: The latency objective for the app that will be created, in microseconds
- **benchmark_\report_path**: Path to the file in which you want your benchmarking reports saved
- **report\_delay_seconds**: The delay between each flush of benchmarking metrics to your reports file, in seconds. At each flush, the metrics will reset.
- **model_name**: The name of the model Clipper should connect to. Note that this must be the same as the model name your model-container uses.
- **model_version**: Your model's version. Again, this must be the same version that your model-container uses.

Your JSON config file should look like:

```
{
   "cifar_data_path":"<cifar_data_path>",
   "num_threads":"<num_threads>",
   "num_batches":"<num_batches>",
   "request_batch_size":"<request_batch_size>",
   "request_batch_delay_micros":"<request_batch_delay_micros>",
   "poisson_delay":"<true/false>",
   "prevent_cache_hits":"<true/false>",
   "latency_objective":"<latency_objective>",
   "benchmark_report_path":"<benchmark_report_path>",
   "report_delay_seconds":"<report_delay_seconds>",
   "model_name":"<model_name>",
   "model_version":"<model_version>"
}
```

If a configuration file is not specified, the benchmark will prompt you for the values of these attributes at runtime.

## Run the benchmarking tool
**These steps are given relative to the main clipper source directory.**

1. Build the Clipper source for release: 
`./configure --release`, `cd release && make`

2. Confirm you have a Redis server running. If you do not, run `redis-server`

3. Confirm your model-container is running. If it is not, follow the instructions in the "Deploy up a model container for querying" section.

4. Run the tool with your JSON configuration file: `./release/src/benchmarks/generic_bench -f <path_to_your_config_file>`
  
5. Check the logs/output from your model container to confirm it's being queried

6. View the benchmarking reports at your specified **benchmark_report_path**.
