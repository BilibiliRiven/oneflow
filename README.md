**OneFlow is a performance-centered and open-source deep learning framework.**

- [Install OneFlow](#install-oneflow)
  - [System Requirements](#system-requirements)
  - [Install with Pip Package](#install-with-pip-package)
  - [Build from Source](#build-from-source)
  - [Troubleshooting](#troubleshooting)
  - [Advanced features](#advanced-features)
- [Getting Started](#getting-started)
- [Documentation](#documentation)
    - [Usage & Design Docs](#usage--design-docs)
    - [API Reference](#api-reference)
    - [OneFlow System Design](#oneflow-system-design)
- [Model Zoo and Benchmark](#model-zoo-and-benchmark)
  - [CNNs(ResNet-50, VGG-16, Inception-V3, AlexNet)](#cnnsresnet-50-vgg-16-inception-v3-alexnet)
  - [Wide&Deep](#widedeep)
  - [BERT](#bert)
- [Communication](#communication)
- [Contributing](#contributing)
- [The Team](#the-team)
- [License](#license)

## Install OneFlow

  ### System Requirements

  - Python >= 3.5
  - CUDA Toolkit Linux x86_64 Driver

    - CUDA runtime is statically linked into OneFlow. OneFlow will work on a minimum supported driver, and any driver beyond. For more information, please refer to [CUDA compatibility documentation](https://docs.nvidia.com/deploy/cuda-compatibility/index.html).

    - Please upgrade your Nvidia driver to version 440.33 or above and install OneFlow for CUDA 10.2 if possible.

  ### Install with Pip Package

  - To install latest stable release of OneFlow with CUDA support:

    ```
    python3 -m pip install -f https://release.oneflow.info oneflow_cu102 --user
    ```

  - To install nightly release of OneFlow with CUDA support:

    ```
    python3 -m pip install oneflow --user -f https://staging.oneflow.info/branch/master/cu102
    ```

  - To install other available builds for different variants:
    ```
    python3 -m pip install oneflow --user -f https://staging.oneflow.info/branch/master/[PLATFORM]
    ```

    All available `[PLATFORM]`:
    | Platform |CUDA Driver Version| Supported GPUs |
    |---|---|---|
    | cu111  | >= 450.80.02  | GTX 10xx, RTX 20xx, A100, RTX 30xx |
    | cu110, cu110_xla  | >= 450.36.06  | GTX 10xx, RTX 20xx, A100|
    | cu102, cu102_xla  | >= 440.33  | GTX 10xx, RTX 20xx |
    | cu101, cu101_xla  | >= 418.39  | GTX 10xx, RTX 20xx |
    | cu100, cu100_xla  | >= 410.48  | GTX 10xx, RTX 20xx |
    | cpu  | N/A | N/A |

  - To install latest release of CPU-only OneFlow:

    ```
    python3 -m pip install --find-links https://release.oneflow.info oneflow_cpu --user
    ```

  - To install legacy version of OneFlow with CUDA support:

    ```
    python3 -m pip install --find-links https://release.oneflow.info oneflow_cu102==0.3.1 --user
    ```

    Some legacy versions available: `0.1.10`, `0.2.0`, `0.3.0`, `0.3.1`

  - If you are in China, you could run this to have pip download packages from domestic mirror of pypi:
    ```
    python3 -m pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple
    ```
    For more information on this, please refer to [pypi 镜像使用帮助](https://mirror.tuna.tsinghua.edu.cn/help/pypi/)

  - Releases are built with G++/GCC 4.8.5, cuDNN 7 and MKL 2020.0-088.

### Build from Source

1. #### Clone Source Code

    - #### Option 1: Clone source code from GitHub

      ```bash
      git clone https://github.com/Oneflow-Inc/oneflow --depth=1
      ```

    - #### Option 2: Download from Aliyun

      If you are in China, please download OneFlow source code from: https://oneflow-public.oss-cn-beijing.aliyuncs.com/oneflow-src.zip

      ```bash
      curl https://oneflow-public.oss-cn-beijing.aliyuncs.com/oneflow-src.zip -o oneflow-src.zip
      unzip oneflow-src.zip
      ```

2. #### Build and Install OneFlow

    - #### Option 1: Build in docker container (recommended)
      - In the root directory of OneFlow source code, run:

        ```
        python3 docker/package/manylinux/build_wheel.py
        ```

        This should produce `.whl` files in the directory `wheelhouse`

      - If you are in China, you might need to add these flags:

        ```
        --use_tuna --use_system_proxy --use_aliyun_mirror
        ```

      - You can choose CUDA/Python versions of wheel by adding:

        ```
        --cuda_version=10.1 --python_version=3.6,3.7
        ```

      - For more useful flags, plese run the script with flag `--help` or refer to the source code of the script.

    - #### Option 2: Build on bare metal
      - Install dependencies. For instance, on Ubuntu 20.04, run:
        ```
        sudo apt install -y libmkl-full-dev nasm libc++-11-dev libncurses5 g++ gcc cmake gdb python3-pip
        ```
        If there is a prompt, it is recommended to select the option to make mkl the default BLAS library.
      - In the root directory of OneFlow source code, run:

        ```
        mkdir build
        cd build
        cmake ..
        make -j$(nproc)
        make pip_install
        ```

      - If you are in China, please add this CMake flag `-DTHIRD_PARTY_MIRROR=aliyun` to speed up the downloading procedure for some dependency tar files.
      - For pure CPU build, please add this CMake flag `-DBUILD_CUDA=OFF`.

### Troubleshooting

Please refer to [troubleshooting](docs/source/troubleshooting.md) for common issues you might encounter when compiling and running OneFlow.

### Advanced features

- #### XRT

  You can check this [doc](oneflow/xrt/README.md) to obtain more details about how to use XLA and TensorRT with OneFlow.

## Getting Started
3 minutes to run MNIST.
1. Clone the demo code from OneFlow documentation
```
git clone https://github.com/Oneflow-Inc/oneflow-documentation.git
cd oneflow-documentation/cn/docs/code/quick_start/
```
2. Run it in Python
```
python mlp_mnist.py
```

3. Oneflow is running and you got the training loss
```
2.7290366
0.81281316
0.50629824
0.35949975
0.35245502
...
```
More info on this demo, please refer to [doc on quick start](http://docs.oneflow.org/quick_start/quickstart_in_3_min.html).

## Documentation
#### Usage & Design Docs
* [link](http://docs.oneflow.org/)
#### API Reference
* [link](https://oneflow.readthedocs.io/en/master/)
#### OneFlow System Design
For those who would like to understand the OneFlow internals, please read the document below:
* [link](https://github.com/Oneflow-Inc/oneflow-documentation/blob/master/en/docs/basics_topics/essentials_of_oneflow.md)

## Model Zoo and Benchmark
* [link](https://github.com/Oneflow-Inc/OneFlow-Benchmark)
### CNNs(ResNet-50, VGG-16, Inception-V3, AlexNet)
* [CNNs](https://github.com/Oneflow-Inc/OneFlow-Benchmark/tree/master/Classification/cnns)

### Wide&Deep
* [OneFlow-WDL](https://github.com/Oneflow-Inc/OneFlow-Benchmark/tree/master/ClickThroughRate/WideDeepLearning)
### BERT
* [BERT](https://github.com/Oneflow-Inc/OneFlow-Benchmark/tree/master/LanguageModeling/BERT)

## Communication
* GitHub issues : any install, bug, feature issues.
* [www.oneflow.org](http://www.oneflow.org) : brand related information.

## Contributing
*  [link](http://docs.oneflow.org/contribute/intro.html)

## The Team
OneFlow was originally developed by [OneFlow Inc](http://www.oneflow.org) and [Zhejiang Lab](http://www.zhejianglab.com/).

## License
[Apache License 2.0](LICENSE)
