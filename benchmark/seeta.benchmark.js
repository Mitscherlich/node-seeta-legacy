const FaceDetector = require('..').FaceDetector
const PointDetector = require('..').PointDetector
const FaceRecognizer = require('..').FaceRecognizer
const readdirSync = require('fs').readdirSync
const statSync = require('fs').statSync
const path = require('path')

console.debug = require('debug')('seeta:benchmark')

const FD_MODEL = path.join(__dirname, '..', 'models/SeetaFaceDetector2.0.ats')
const FA_MODEL = path.join(__dirname, '..', 'models/SeetaPointDetector2.0.pts5.ats')
const FR_MODEL = path.join(__dirname, '..', 'models/SeetaFaceRecognizer2.0.ats')
const IMAGE_PATH = path.join(__dirname, 'lfw')

describe('benchmark/seeta.benchmark.js', () => {
  let detector
  let pointer
  let recognizer
  let database
  let images

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

  before(() => {
    images = []
    walk(IMAGE_PATH)
    console.debug({ total: images.length })
  })

  process.env.BENCHMARK == 'Detection' && describe('detection benchmark', () => {
    before(() => {
      if (!detector) detector = new FaceDetector(FD_MODEL)
    })

    it('detect()', () => {
      console.time('detection')
      images.forEach(image => {
        console.debug({ image, faces: detector.detect(image).data.faces })
      })
      console.timeEnd('detection')
    })
  })

  process.env.BENCHMARK == 'Alignment' && describe('aligment benchmark', () => {
    before(() => {
      if (!detector) detector = new FaceDetector(FD_MODEL)
      if (!pointer) pointer = new PointDetector(FA_MODEL)
    })

    it('detect()', () => {
      console.time('aligment')
      images.forEach(image => {
        console.debug({ image })
        detector.detect(image).data.faces.forEach((face, i) => {
          console.debug(`face[${i}]:`, pointer.detect(image, face).data.points)
        })
      })
      console.timeEnd('aligment')
    })
  })
})
