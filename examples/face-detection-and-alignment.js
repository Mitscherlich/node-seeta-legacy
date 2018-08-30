const fs = require('fs');

const { Image, FaceDetector, PointDetector } = require('..');

const fdModel = 'models/SeetaFaceDetector2.0.ats';
const faModel = 'models/SeetaPointDetector2.0.pts5.ats';
const images = [];

function walk(path) {
  fs.readdirSync(path).forEach(file => {
    const newPath = path + '/' + file;
    const stat = fs.statSync(newPath);

    if (stat.isFile()) {
      if (/(.*)\.(jpg|png)/.test(file)) {
        const image = new Image(newPath);
        // console.log('image', newPath);
        // console.log('image.width', image.width);
        // console.log('image.height', image.height);
        // console.log('image.channels', image.channels);
        images.push(image);
      }
    } else if (stat.isDirectory()) {
      walk(newPath);
    }
  });
}

walk('benchmark/data');

if (!fdModel || !fs.existsSync(fdModel)) {
  console.error(new Error());
}

console.time('Load detector');
const detector = new FaceDetector(fdModel);
const pointer = new PointDetector(faModel);
console.timeEnd('Load detector');

// detect sync
console.log(detector.detectSync(images[0]));
console.log(pointer.detectSync(images[0], detector));
