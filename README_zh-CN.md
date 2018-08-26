# node-seeta
[SeetaFaceEngine](https://github.com/seetaface/SeetaFaceEngine2) 的 Node.js 插件.

## 如何安装

### 系统需求

- Ubuntu 16.04 x86_64

```yaml
最低配置:
  内存: 2GB
  CPU: 1core with 1GHz
  GPU: Not necessary

推荐配置:
  内存: 4GB
  CPU: 4core with 2.1GHz
  GPU: if better
```

- Windows 10 (x86/x86_64)

```yaml
编译环境:
  - Visual Studio 2013 / 2015 (msvc 支持 v120 v140 or v141)
  - minGW
  - CMake for windows
  - Node.js (lts/carbon)
```

### 依赖

需要 [OpenCV](https://github.com/opencv/opencv)。

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

需要 [SeetaFaceEngine2](https://github.com/seetaface/SeetaFaceEngine2) 预编译的模型. 你可以从[这里](https://pan.baidu.com/s/1HJj8PEnv3SOu6ZxVpAHPXg)手动下载他们.

### 安装

0. 编译依赖 `node-cmake`

请确保你以正确安装 `cmake`.

```bash
$ npm i -g node-cmake
```

1. 通过 [npm.js](https://www.npmjs.com/package/seeta)

```bash
$ npm i -S seeta
```

2. 通过本仓库

```bash
$ npm i -S https://github.com/Mitscherlich/node-seeeta.git
# 或者
$ git clone -b master --depth 1 https://github.com/Mitscherlich/node-seeeta.git node_modules/seeta
```

## 使用说明

### 人脸检测

检测图片中的人脸边界盒位置。

```js
const FaceDetection = require('seeta').FaceDetection
const path = require('path')
// 1. 指定模型路径
const fdModel = path.join('path', 'to', 'models/SeetaFaceDetector2.0.ats')
// 2. 创建检测器对象
const detector = new FaceDetection(fdModel)
// 3. 检测图片中的人脸
const results = detector.detect(path.join('path', 'to', 'image/test.png'))
console.log({ result })
// 结果如下:
// {
//   "count": Number,     // 人脸个数
//   "data": {
//     "faces": [{
//       "x": Number,     // 人脸边界盒左上角 x
//       "y": Number,     // 人脸边界盒左上角 y
//       "width": Number, // 人脸边界盒宽度
//       "height": Number // 人脸边界盒高度
//     }]
//   }
// }
```

### 面部特征点定位

标记图像中制定面孔的特征点 (5 点)

```js
const { FaceDetection, PointDetector } = require('seeta')
const path = require('path')
// 1. 指定模型位置
const fdModel = path.join('path', 'to', 'models/SeetaFaceDetector2.0.ats')
const faModel = path.join('path', 'to', 'models/SeetaPointDetector2.0.pts5.ats')
// 2. 创建检测器对象
const detector = new FaceDetection(fdModel)
const pointer = new PointDetector(faModel)
// 3. 检测人脸位置并标定特征点
const faces = detector.detect(path.join('path', 'to', 'image/test.png'))
if (faces.data && face.data.faces.length) {
  const points = pointer.detect(path.join('path', 'to', 'image/test.png', faces.data.faces[0]))
  console.log({ result })
}
// 结果如下
// {
//   "data": {
//     "points": [{
//       "x": Number,     // 特征点 x
//       "y": Number,     // 特征点 y
//     }]
//   }
// }
```

### 人脸识别

1. `1 vs 1` 比较

比较两张图片中包含的人脸并得出相似度

```js
const { FaceDetector, PointDetector, FaceRecognizer } = require('seeta')
const path = require('path')
// 1. 指定模型路径
const fdModel = path.join('path', 'to', 'models/SeetaFaceDetector2.0.ats')
const faModel = path.join('path', 'to', 'models/SeetaPointDetector2.0.pts5.ats')
const frModel = path.join('path', 'to', 'models/SeetaFaceRecognizer2.0.ats')
// 2. 创建检测器对象
console.time('initailize')
const detector = new FaceDetector(fdModel)
const pointer = new PointDetector(faModel)
const recognizer = new FaceRecognizer(frModel)
console.timeEnd('initailize')
// 3. 检测图片中的面孔并标定特征点位置
const imagePath = join('path', 'to', 'data/test.png')

const faces = detector.detect(imagePath)
const face1 = faces.data.faces[0]
const face2 = faces.data.faces[1]

const points1 = pointer.detect(imagePath, face1).data.points
const points2 = pointer.detect(imagePath, face2).data.points
// 4. 比较两张图片中的面孔
const similar = recognizer.compare(
  { path: imagePath, points: points1 }, { path: imagePath, points: points1 }
)
console.log({ similar })
```

2. `1 vs n` 比较

- 第一步: 在数据库中注册图片

```js
const { FaceDetector, PointDetector, FaceRecognizer } = require('seeta')
const path = require('path')
// 1. 指定模型路径
const fdModel = path.join('path', 'to', 'models/SeetaFaceDetector2.0.ats')
const faModel = path.join('path', 'to', 'models/SeetaPointDetector2.0.pts5.ats')
const frModel = path.join('path', 'to', 'models/SeetaFaceRecognizer2.0.ats')
// 2. 创建检测器对象
console.time('initailize')
const detector = new FaceDetector(fdModel)
const pointer = new PointDetector(faModel)
const recognizer = new FaceRecognizer(frModel)
console.timeEnd('initailize')
// 3. 检测图片中的面孔并标定特征点位置
const imagePath = path.join('path', 'to', 'data/test.png')

const faces = detector.detect(imagePath)
const face = faces.data.faces[0]

const points = pointer.detect(imagePath, face1).data.points
// 4. 注册一张图片
const index = recognizer.register(imagePath, points)
console.log({ index }) // ==> the index in recognizer face database
```

- 第二步：比较图片相似度

```js
// 5. 检测一张新图片
const newImage = path.join('path', 'to', 'data/another_image.png')
const faces1 = detector.detect(newImage)
const face1 = faces1.data.faces[0]

const points1 = pointer.detect(newImage, face1).data.points
// 6. 在数据库中比较相似度
const results = recognizer.recognition(newImage, face1)
if (result.data && result.data.length) {
  console.log({ similars: result.data }) // ==> all similars in face databse
}
```

## License

[MIT](LICENSE)
