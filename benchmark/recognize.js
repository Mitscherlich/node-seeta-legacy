const { FaceDetector, PointDetector, FaceRecognizer } = require('..')
const { join, resolve } = require('path')

console.debug = require('debug')('seeta:benchmark:recognize')

const fdModel = join(__dirname, '..', 'models/SeetaFaceDetector2.0.ats')
const faModel = join(__dirname, '..', 'models/SeetaPointDetector2.0.pts5.ats')
const frModel = join(__dirname, '..', 'models/SeetaFaceRecognizer2.0.ats')

console.time('initailize')
const detector = new FaceDetector(fdModel)
const pointer = new PointDetector(faModel)
const recognizer = new FaceRecognizer(frModel)
console.timeEnd('initailize')

const imagePath = join(__dirname, 'data/image_0003.png')

const faces = detector.detect(imagePath)
const face1 = faces.data.faces[0]
const face2 = faces.data.faces[1]

console.debug({ face1, face2 })

const points1 = pointer.detect(imagePath, face1).data.points
const points2 = pointer.detect(imagePath, face2).data.points

console.debug({ points1, points2 })

const similar = recognizer.compare(
  { path: imagePath, points: points1 }, { path: imagePath, points: points1 }
)

console.debug({ similar })
