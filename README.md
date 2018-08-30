# <center>![seeta logo](docs/seeta_logo.png)<br/>node-seeta</center>

[![NPM version][npm-image]][npm-url]
[![npm download][download-image]][download-url]
[![MIT License][license-image]](License)

[npm-image]: https://img.shields.io/npm/v/seeta.svg?style=flat-square
[npm-url]: https://npmjs.org/package/seeta
[travis-image]: https://img.shields.io/travis/Mitscherlich/node-seeta.svg?style=flat-square
[travis-url]: https://travis-ci.org/Mitscherlich/node-seeta
[download-image]: https://img.shields.io/npm/dm/seeta.svg?style=flat-square
[download-url]: https://npmjs.org/package/seeta
[license-image]: https://img.shields.io/badge/license-MIT-blue.svg?longCache=true&style=flat-square

Node.js wrap for [SeetaFaceEngine](https://github.com/seetaface/SeetaFaceEngine2).

* **[Examples](#examples)**
* **[How to install](#how-to-install)**
* **[Usage](#usage)**

see other language version:

* [中文](README_zh-CN.md)

<a name="examples"></a>

# Examples

See [examples](examples) for more implementations.

<a name="how-to-install"></a>

# How to install

## System require

|              | Ubuntu      | Windows    |
|:------------:|:-----------:|:----------:|
| version      | 16.04/18.04 | 7/8/8.1/10 |
| arch         | x64         | x86/x86_64 |
| minimum      | 1 GB        | 1 GB       |
| recommend    | 2+GB        | 2+GB       |
| build status | [![build status][travis-image]][travis-url] | [![build status][travis-image]][travis-url] |

## Download

* Git tree: https://github.com/Mitscherlich/node-seeta.git
  * Clone with `git clone https://github.com/Mitscherlich/node-seeta.git` :hammer: [Build instructions](https://github.com/Mitscherlich/node-seeta/wiki/Build).

<a name="usage"></a>

# Usage

## Face detection

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

## Facial points detection

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

## Face recognize

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

# License

[MIT](LICENSE)
