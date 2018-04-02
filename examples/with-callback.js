const seeta = require('..');

const imagePath = __dirname + '/../data/image_0002.png';
const modelPath = __dirname + '/../models/seeta_fd_frontal_v1.0.bin';

let image;

seeta.loadImage(imagePath, (err, destPath) => {
  image = destPath;

  seeta.detect(image, modelPath, (err, result) => {
    console.log(JSON.parse(result));
  });
});
