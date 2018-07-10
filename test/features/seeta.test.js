const FaceDetector = require('../../').FaceDetector
const PointDetector = require('../../').PointDetector
const FaceRecognizer = require('../../').FaceRecognizer
const should = require('should')
const path = require('path')

console.debug = require('debug')('seeta:test')

const FD_MODEL = path.join(__dirname, '../..', 'models/SeetaFaceDetector2.0.ats')
const FA_MODEL = path.join(__dirname, '../..', 'models/SeetaPointDetector2.0.pts5.ats')
const FR_MODEL = path.join(__dirname, '../..', 'models/SeetaFaceRecognizer2.0.ats')
const IMAGE_PATH = path.join(__dirname, '../..', 'benchmark/data/image_0003.png')

describe('features/seeta.test.js', () => {
  let detector
  let pointer
  let recognizer

  describe('> FaceDetector', () => {
    describe('contructor()', () => {
      it('should throw an error if use as a normal function', () => {
        try {
          detector = FaceDetector(FD_MODEL)

          should.fail()
        } catch (e) {
          e.should.be.an.instanceOf(TypeError)
          e.message.should.equal('Do not use as a normal function.')
          delete detector
          detector = undefined
        }
      })

      it('should throw an error if `modelPath` is missing', () => {
        try {
          detector = new FaceDetector()

          should.fail()
        } catch (e) {
          e.message.should.equal('Too few arguments: Except 1 but got 0. \'modelPath\' is needed in arguments.')
          delete detector
          detector = undefined
        }
      })

      it('should throw an error if `modelPath` is wrong', () => {
        const sillyModel = '/path/to/wrong/place/SeetaFaceDetector2.0.ats'
        try {
          detector = new FaceDetector(sillyModel)

          should.fail()
        } catch (e) {
          e.message.should.equal('Cannot locate model file in path: ' + sillyModel)
          delete detector
          detector = undefined
        }
      })

      it('should wrap a detector object', () => {
        detector = new FaceDetector(FD_MODEL)

        detector.should.be.an.instanceOf(FaceDetector)

        detector.should.has.property('detect')
        detector.detect.should.be.a.Function()
      })
    })

    describe('detect()', () => {
      before(() => {
        if (!detector) detector = new FaceDetector(FD_MODEL)
      })

      it('should return an object with face data', () => {
        console.time('detection')
        let faces = detector.detect(IMAGE_PATH)
        console.timeEnd('detection')

        faces.should.be.an.Object()
        faces.should.have.property('count')
        faces.should.have.property('data')
        faces.data.faces.should.be.an.Array()

        console.debug({ faces: faces.data.faces })
      })
    })
  })

  describe('> PointDetector', () => {
    describe('contructor()', () => {
      it('should throw an error if use as a normal function', () => {
        try {
          pointer = PointDetector(FD_MODEL)

          should.fail()
        } catch (e) {
          e.should.be.an.instanceOf(TypeError)
          e.message.should.equal('Do not use as a normal function.')
          delete pointer
          pointer = undefined
        }
      })

      it('should throw an error if `modelPath` is missing', () => {
        try {
          pointer = new PointDetector()

          should.fail()
        } catch (e) {
          e.message.should.equal('Too few arguments: Except 1 but got 0. \'modelPath\' is needed in arguments.')
          delete pointer
          pointer = undefined
        }
      })

      it('should throw an error if `modelPath` is wrong', () => {
        const sillyModel = '/path/to/wrong/place/SeetaPointDetector2.0.pts5.ats'
        try {
          pointer = new PointDetector(sillyModel)

          should.fail()
        } catch (e) {
          e.message.should.equal('Cannot locate model file in path: ' + sillyModel)
          delete pointer
          pointer = undefined
        }
      })

      it('should wrap a pointer object', () => {
        pointer = new PointDetector(FA_MODEL)

        pointer.should.be.an.instanceOf(PointDetector)

        pointer.should.has.property('detect')
        pointer.detect.should.be.a.Function()
      })
    })

    describe('detect()', () => {
      before(() => {
        if (!detector) detector = new FaceDetector(FD_MODEL)
        console.debug(2)
        if (!pointer) pointer = new PointDetector(FA_MODEL)
        console.debug(3)
      })

      it('should return an object with face data', () => {
        console.time('alignment')
        let faces = detector.detect(IMAGE_PATH)
        console.timeEnd('alignment')

        console.time('alignment')
        let points = pointer.detect(IMAGE_PATH, faces.data.faces[0])
        console.timeEnd('alignment')

        points.should.be.an.Object()
        points.should.have.property('data')
        points.data.points.should.be.an.Array()

        console.debug({ points: points.data.points })
      })
    })
  })

  describe('> FaceRecognizer', () => {
    describe('contructor()', () => {
      it('should throw an error if use as a normal function', () => {
        try {
          recognizer = FaceRecognizer(FR_MODEL)

          should.fail()
        } catch (e) {
          e.should.be.an.instanceOf(TypeError)
          e.message.should.equal('Do not use as a normal function.')
          delete recognizer
          recognizer = undefined
        }
      })

      it('should throw an error if `modelPath` is missing', () => {
        try {
          recognizer = new FaceRecognizer()

          should.fail()
        } catch (e) {
          e.message.should.equal('Too few arguments: Except 1 but got 0. \'modelPath\' is needed in arguments.')
          delete recognizer
          recognizer = undefined
        }
      })

      it('should throw an error if `modelPath` is wrong', () => {
        const sillyModel = '/path/to/wrong/place/SeetaFaceRecognizer2.0.pts5.ats'
        try {
          recognizer = new FaceRecognizer(sillyModel)

          should.fail()
        } catch (e) {
          e.message.should.equal('Cannot locate model file in path: ' + sillyModel)
          delete recognizer
          recognizer = undefined
        }
      })

      it('should wrap a recognizer object', () => {
        recognizer = new FaceRecognizer(FR_MODEL)

        recognizer.should.be.an.instanceOf(FaceRecognizer)

        recognizer.should.has.property('compare')
        recognizer.compare.should.be.a.Function()

        recognizer.should.has.property('register')
        recognizer.register.should.be.a.Function()

        recognizer.should.has.property('recognition')
        recognizer.recognition.should.be.a.Function()

        recognizer.should.has.property('clear')
        recognizer.clear.should.be.a.Function()
      })
    })

    describe('1 vs 1 compare', () => {

      let faces

      before(() => {
        if (!detector) detector = new FaceDetector(FD_MODEL)
        if (!pointer) pointer = new PointDetector(FA_MODEL)
        if (!recognizer) recognizer = new FaceRecognizer(FR_MODEL)

        faces = detector.detect(IMAGE_PATH).data.faces
        faces.forEach(face => {
          face.points = pointer.detect(IMAGE_PATH, face).data.points
        })
      })

      describe('compare()', () => {
        it('should return a number for similar', () => {
          let face1 = faces[0]
          let face2 = faces[1]
          console.time('compare')
          let similar = recognizer.compare(
            { path: IMAGE_PATH, points: face1.points }, { path: IMAGE_PATH, points: face2.points }
          )
          console.timeEnd('compare')
          console.debug({ similar })
        })
      })
    })

    describe('1 vs n compare', () => {
      let faces
      let database

      before(() => {
        if (!detector) detector = new FaceDetector(FD_MODEL)
        if (!pointer) pointer = new PointDetector(FA_MODEL)
        if (!recognizer) recognizer = new FaceRecognizer(FR_MODEL)

        faces = detector.detect(IMAGE_PATH).data.faces
        faces.forEach(face => {
          face.points = pointer.detect(IMAGE_PATH, face).data.points
        })

        database = []
      })

      describe('register', () => {
        it('should return a number for index', () => {
          console.time('register')
          faces.forEach(face => {
            let index = recognizer.register(IMAGE_PATH, face.points)
            index.should.greaterThan(-1)  // -1 for failure
            database.push(index)
          })
          console.timeEnd('register')
          console.debug({ database })
        })
      })

      describe('recognition()', () => {
        it('should return all similarities for faces in database', () => {
          console.time('recognition')
          let result = recognizer.recognition(IMAGE_PATH, faces[0].points)
          console.timeEnd('recognition')
          result.should.has.property('data')
          result.data.should.be.Array()
          console.debug({ result: result.data })
        })
      })

      describe('clear()', () => {
        it('should clear data without error', () => {
          recognizer.clear()
        })
      })
    })
  })
})
