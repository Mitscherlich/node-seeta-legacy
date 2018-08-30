const {
  Image,
  FaceDetector,
  FaceRecognizer,
  PointDetector
} = require('..');

const path = require('path');
const fs = require('fs');

const fdModel = path.join(__dirname, '..', 'models/SeetaFaceDetector2.0.ats');
const faModel = path.join(__dirname, '..', 'models/SeetaPointDetector2.0.pts5.ats');
const frModel = path.join(__dirname, '..', 'models/SeetaFaceRecognizer2.0.ats');

// console.time('fd model loaded');
const detector = new FaceDetector(fdModel);
// console.timeEnd('fd model loaded');
// console.time('fa model loaded');
const pointer = new PointDetector(faModel);
// console.timeEnd('fa model loaded');
// console.time('fr model loaded');
const recognizer = new FaceRecognizer(frModel, { lazyload: true });
// console.timeEnd('fr model loaded');
// console.log(recognizer.operational);

// 1 vs 1 compare
const image = new Image(path.join('benchmark/data/image_0003.png'));
// const faces = detector.detectSync(image);
// const facial1 = pointer.detectSync(image, detector).faces[0].points;
// const facial2 = pointer.detectSync(image, detector).faces[1].points;
const similar = recognizer.compareSync([image, image], detector, pointer);
console.log({ similar });

// 1 vs n compare
const images = [
  new Image(path.join('benchmark/data/image_0001.png')),
  new Image(path.join('benchmark/data/image_0002.png')),
];

images.forEach((image, index) => console.log('image', index, '=>', recognizer.register(image, detector, pointer)));
console.log({ similars: recognizer.recognizeSync(images[0], detector, pointer) });
