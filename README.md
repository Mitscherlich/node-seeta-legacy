# node-seeta
Node.js wrap for [SeetaFaceEngine](seetaface/SeetaFaceEngine)

see those docs for more info about it:

- [Face Detection](docs/facedect.md)
- [Face Aligment](docs/facealign.md)
- [Face Identification](docs/viplnet.md)

see other language version:

- [中文](README_zh-CN.md)

## Usage

require [SeetaFaceEngine](seetaface/SeetaFaceEngine) models. you may manually down them [HERE](https://drive.google.com/drive/folders/1JpPsdfMO3yqyX0Cj9eNO1wgXgFNuZOk6?usp=sharing).

also, dependent on [ImageMagick](https://github.com/ImageMagick/ImageMagick) for grayscaling RGBA image.
you can install it on macOS with [Homebrew](https://brew.sh/) using following command:

```bash
$ brew install imagemagick [--with-webp]
```

0. Install from [NPM](https://www.npmjs.com/package/seeta)

install as a dependency.

```bash
$ npm i -S seeta
```

1. Basic usage

```js
/*
 * with callback
 */

const seeta = require('seeta');


const imagePath = '/path/to/image_name.png';
const fdModelPath = '/path/to/models/seeta_fd_frontal_v1.0.bin';
const faModelPath = '/path/to/models/seeta_fa_v1.1.bin';
const frModelPath = '/path/to/models/seeta_fr_v1.0.bin';

seeta.loadImage(imagePath, (err, destPath) => {

  seeta.detect(image, faModelPath, (err, result) => {
    // result looks like '{ "faces": [ [ x, y, width, height ], ... ] }'
    console.log(JSON.parse(result));
  });

  seeta.align(image, faModelPath, faModelPath, (err, result) => {
    // result looks like '{ "faces": [ { "points": [ x, y ] }, ... }'
    console.log(JSON.parse(result));
  });
});


```

see [example/with-callback.js](example/with-callback.js) for more detail.
