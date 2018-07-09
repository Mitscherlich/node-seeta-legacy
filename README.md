# node-seeta
Node.js wrap for [SeetaFaceEngine](https://github.com/seetaface/SeetaFaceEngine2).

see other language version:

- [中文](README_zh-CN.md)

## Install

### System require

- Ubuntu 16.04 x86_64

```yaml
minium:
  memory: 2GB
  CPU: 1core with 1GHz
  GPU: Not necessary

recommend:
  memory: 4GB
  CPU: 4core with 2.1GHz
  GPU: if better
```

- Windows 10 (x86/x86_64)

```yaml
compile enviroment:
  - Visual Studio 2013 / 2015 (msvc require v120 v140 or v141)
  - minGW
  - CMake for windows
  - Node.js (lts/carbon)
```

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

This module also requires [SeetaFaceEngine2](https://github.com/seetaface/SeetaFaceEngine2) models. You may manually down them from [HERE](https://pan.baidu.com/s/1HJj8PEnv3SOu6ZxVpAHPXg).

### Installation

0. install `node-cmake` for compiling

make sure you have setup `cmake` correctly.

```bash
$ npm i -g node-cmake
```

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

### Face detection

Detect face(s) in a image.

```js
const FaceDetection = require('seeta').FaceDetection
const path = require('path')
// 1. name face detection model path.
const fdModel = path.join('path', 'to', 'models/SeetaFaceDetector2.0.ats')
// 2. create a detector
const detector = new FaceDetection(fdModel)
// 3. detect face(s) in a image
const results = detector.detect(path.join('path', 'to', 'image/test.png'))
console.log({ result })
// result will in this form:
// {
//   "count": Number,     // count of face(s)
//   "data": {
//     "faces": [{
//       "x": Number,     // face left-upper point x
//       "y": Number,     // face left-upper point y
//       "width": Number, // face bounding box width
//       "height": Number // face bounding box height
//     }]
//   }
// }
```

### Facial points detection

Mark facial points for compare and recognize.

```js
const { FaceDetection, PointDetector } = require('seeta')
const path = require('path')
// 1. name detector models path.
const fdModel = path.join('path', 'to', 'models/SeetaFaceDetector2.0.ats')
const faModel = path.join('path', 'to', 'models/SeetaPointDetector2.0.pts5.ats')
// 2. create a point detector
const detector = new FaceDetection(fdModel)
const pointer = new PointDetector(faModel)
// 3. detect face(s) in a image
const faces = detector.detect(path.join('path', 'to', 'image/test.png'))
if (faces.data && face.data.faces.length) {
  const points = pointer.detect(path.join('path', 'to', 'image/test.png', faces.data.faces[0]))
  console.log({ result })
}
// result will in this form:
// {
//   "data": {
//     "points": [{
//       "x": Number,     // facial point x
//       "y": Number,     // facial point y
//     }]
//   }
// }
```

### Face recognize

1. `1 vs 1` compare

compare two image contains face and compare for its similar.

```js
const { FaceDetector, PointDetector, FaceRecognizer } = require('seeta')
const path = require('path')
// 1. name detector models path.
const fdModel = path.join('path', 'to', 'models/SeetaFaceDetector2.0.ats')
const faModel = path.join('path', 'to', 'models/SeetaPointDetector2.0.pts5.ats')
const frModel = path.join('path', 'to', 'models/SeetaFaceRecognizer2.0.ats')
// 2. create detectors
console.time('initailize')
const detector = new FaceDetector(fdModel)
const pointer = new PointDetector(faModel)
const recognizer = new FaceRecognizer(frModel)
console.timeEnd('initailize')
// 3. detect face(s) in a image
const imagePath = join('path', 'to', 'data/test.png')

const faces = detector.detect(imagePath)
const face1 = faces.data.faces[0]
const face2 = faces.data.faces[1]

const points1 = pointer.detect(imagePath, face1).data.points
const points2 = pointer.detect(imagePath, face2).data.points
// 4. compare face(s) in two images
const similar = recognizer.compare(
  { path: imagePath, points: points1 }, { path: imagePath, points: points1 }
)
console.log({ similar })
```

2. `1 vs n` compare

- first: register a image

```js
const { FaceDetector, PointDetector, FaceRecognizer } = require('seeta')
const path = require('path')
// 1. name detector models path.
const fdModel = path.join('path', 'to', 'models/SeetaFaceDetector2.0.ats')
const faModel = path.join('path', 'to', 'models/SeetaPointDetector2.0.pts5.ats')
const frModel = path.join('path', 'to', 'models/SeetaFaceRecognizer2.0.ats')
// 2. create detectors
console.time('initailize')
const detector = new FaceDetector(fdModel)
const pointer = new PointDetector(faModel)
const recognizer = new FaceRecognizer(frModel)
console.timeEnd('initailize')
// 3. detect face(s) in a image
const imagePath = path.join('path', 'to', 'data/test.png')

const faces = detector.detect(imagePath)
const face = faces.data.faces[0]

const points = pointer.detect(imagePath, face1).data.points
// 4. register a face
const index = recognizer.register(imagePath, points)
console.log({ index }) // ==> the index in recognizer face database
```

- find all similar in database

```js
// 5. detect face(s) in a image
const newImage = path.join('path', 'to', 'data/another_image.png')
const faces1 = detector.detect(newImage)
const face1 = faces1.data.faces[0]

const points1 = pointer.detect(newImage, face1).data.points
// 6. find all similar in database
const results = recognizer.recognition(newImage, face1)
if (result.data && result.data.length) {
  console.log({ similars: result.data }) // ==> all similars in face databse
}
```

## License

[MIT](LICENSE)
