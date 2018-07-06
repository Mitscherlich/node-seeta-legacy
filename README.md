# node-seeta
Node.js wrap for [SeetaFaceEngine](https://github.com/seetaface/SeetaFaceEngine2).

see other language version:

- [中文](README_zh-CN.md)

## Install

### Dependencies

[OpenCV](https://github.com/opencv/opencv) is required to compile. You may use the following commands to install OpenCV:

- `macOS`

```bash
$ brew install opecv
```

Use the following command to check this formula more details:

```bash
$ brew info opencv
```

On `Linux` or `Windows`, we recommend to build OpenCV lib from source. See OpenCV installation tutorials for [Windows](https://docs.opencv.org/3.4.1/d3/d52/tutorial_windows_install.html) and [Linux](https://docs.opencv.org/3.4.1/d7/d9f/tutorial_linux_install.html).

On Ubuntu, you also need install `libopenblas` first:

```bash
$ [sudo] apt install -y libopenblas-dev
# or compile from source
$ git clone https://github.com/xianyi/OpenBLAS.git
$ cd OpenBLAS
$ make
$ [sudo] make PREFIX=/usr/local/OpenBLAS install
```

This module also requires [SeetaFaceEngine2](https://github.com/seetaface/SeetaFaceEngine2) models. You may manually down them from [HERE](http://115.159.114.29:8090/s/wNzPNdA7w8FPaZK).

### Installation

1. via [npm.js](https://www.npmjs.com/package/seeta)

```bash
$ npm i -S seeta
```

2. via this repo

```bash
$ npm i -S https://github.com/Mitscherlich/node-seeeta.git
# or
$ git clone -b stable --depth 1 https://github.com/Mitscherlich/node-seeeta.git node_modules/seeta
```

## Usage

## License

[MIT](LICENSE)
