# node-seeta
Seetaface engine [SeetaFaceEngine](https://github.com/seetaface/SeetaFaceEngine2) 的 Node.js 插件.

## 如何安装

### 依赖

需要 [OpenCV](https://github.com/opencv/opencv)。你可以参考下面的命令来安装 OpenCV:

- `macOS`

```bash
$ brew install opecv
```

你可以使用下面的命令了解更多关于使用 `Homebrew` 安装 `OpenCV` 的信息:

```bash
$ brew info opencv
```

对于 `Linux` 或 `Windows` 平台, 我更推荐使用 OpenCV 源码构建安装. 参考 OpenCV 安装教程： [Windows](https://docs.opencv.org/3.4.1/d3/d52/tutorial_windows_install.html) [Linux](https://docs.opencv.org/3.4.1/d7/d9f/tutorial_linux_install.html).

在 Ubuntu 上, 你可能还需要安装 `libopenblas`:

```bash
$ [sudo] apt install -y libopenblas-dev
# 或者从源码编译
$ git clone https://github.com/xianyi/OpenBLAS.git
$ cd OpenBLAS
$ make
$ [sudo] make PREFIX=/usr/local/OpenBLAS install
```

需要 [SeetaFaceEngine2](https://github.com/seetaface/SeetaFaceEngine2) 预编译的模型. 你可以从[这里](http://115.159.114.29:8090/s/wNzPNdA7w8FPaZK)手动下载他们.

### 安装

1. 通过 [npm.js](https://www.npmjs.com/package/seeta)

```bash
$ npm i -S seeta
```

2. 通过本仓库

```bash
$ npm i -S https://github.com/Mitscherlich/node-seeeta.git
# 或者
$ git clone -b stable --depth 1 https://github.com/Mitscherlich/node-seeeta.git node_modules/seeta
```

## 使用说明

## License

[MIT](LICENSE)
