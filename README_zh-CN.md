# node-seeta
[SeetaFaceEngine](seetaface/SeetaFaceEngine) 的 Node.js 插件安装

算法请参考以下文档:

- [Face Detection](docs/facedect.md)
- [Face Aligment](docs/facealign.md)
- [Face Identification](docs/viplnet.md)

## 使用方法

需要 [SeetaFaceEngine](seetaface/SeetaFaceEngine) 提供的模型. 你可以从 **[这里](https://drive.google.com/drive/folders/1JpPsdfMO3yqyX0Cj9eNO1wgXgFNuZOk6?usp=sharing)** 手动下载.

针对国人用户，请使用中科院提供的[团队文档库](http://ddl.escience.cn/t/bm9kZS1zZWV0YQ==)获取备用链接.

同时依赖于 [ImageMagick](https://github.com/ImageMagick/ImageMagick) 来灰度化图像.
你可以在 macOS 系统上使用 [Homebrew](https://brew.sh/) 安装, 参考下面的命令:

```bash
$ brew install imagemagick [--with-webp]
```

0. 从 [NPM](https://www.npmjs.com/package/seeta) 安装

作为依赖安装.

```bash
$ npm i -S seeta
```

1. 基本用法

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

参考 [example/with-callback.js](example/with-callback.js) 了解更多细节.
