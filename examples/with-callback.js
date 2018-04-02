const seeta = require('..');

const imagePath = __dirname + '/../data/image_0001.png';
const fdModelPath = __dirname + '/../models/seeta_fd_frontal_v1.0.bin';
const faModelPath = __dirname + '/../models/seeta_fa_v1.1.bin';
const frModelPath = __dirname + '/../models/seeta_fr_v1.0.bin';

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
