const { FaceDetector, PointDetector } = require('..')
const { join, resolve } = require('path')
const { readdirSync, statSync } = require('fs')

console.debug = require('debug')('seeta:benchmark:test')

const fdModel = join(__dirname, '..', 'models/SeetaFaceDetector2.0.ats')
const faModel = join(__dirname, '..', 'models/SeetaPointDetector2.0.pts5.ats')

const detector = new FaceDetector(fdModel)
const pointer = new PointDetector(faModel)

let images = []

const walk = path => {
  readdirSync(path)
    .forEach(file => {
      const newPath = path + '/' + file
      const stat = statSync(newPath)

      if (stat.isFile()) {
        if (/(.*)\.(jpg|png)/.test(file))
          images.push(newPath)
      }
      else if (stat.isDirectory()) {
        walk(newPath)
      }
    })
}
walk(__dirname)

images.forEach(image => {
  let face = detector.detect(image)
  if (face.data) {
    face.data.faces.forEach(rect => {
      let point = pointer.detect(image, rect)
      if (point.data)
        rect.points = point.data.points
    })
  }
  console.debug({ face })
})
