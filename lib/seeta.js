/**
 * Load binary library
 */
const oldDir = process.cwd();
try {
  // ensure we're in the right location so we can dynamically load the SeetafaceEngine libraries
  process.chdir(__dirname + '/../deps/seetaface2/' + process.platform + '/' + process.arch + '/lib')
} catch (e) {
  // if no local deps/ dir, assume shared lib linking. keep going
}
// process.chdir(__dirname + '/../deps/seetaface2/' + process.platform + '/' + process.arch + '/lib');
const seeta = require(__dirname + '/../build/Release/seeta.node');
process.chdir(oldDir);

const util = require('util');

function noop() {}

function validCaller() {
  switch (arguments.length) {
    case 3:
      if (!(arguments[2] instanceof seeta.PointDetector)) {
        throw new Error('`pointDetector` is not a valid seeta::PointDetector object');
      }
    case 2:
      if (!(arguments[1] instanceof seeta.FaceDetector)) {
        throw new Error('`faceDetector` is not a valid seeta::FaceDetector object');
      }
    case 1:
      if (!(arguments[0] instanceof seeta.Image)) {
        throw new Error('`image` is not a valid seeta::Image object');
      }
    default: break;
  }
}

/**
 * seeta::Image
 */
seeta.Image.prototype.__defineGetter__('width', function () { return this.__width__(); });
seeta.Image.prototype.__defineGetter__('height', function () { return this.__height__(); });
seeta.Image.prototype.__defineGetter__('channels', function () { return this.__channels__(); });

/**
 * seeta::FaceDetector
 */
seeta.FaceDetector.prototype.detectSync = seeta.FaceDetector.prototype.__detect_sync__;

seeta.FaceDetector.prototype.detect = function (image) {
  const that = this; // TODO: flatten namespace

  if (typeof image === 'string') {
    image = new seeta.Image(image);
  }

  try {
    validCaller.call(this, image);
  } catch (err) {
    // reject by default
    return Promise.reject(err);
  }

  return new Promise(function (resolve, reject) {
    that.__detect__(image, function (err, result) {
      if (err) {
        // reject by default
        return reject(err);
      }

      if (!result) {
        return reject(new Error('Error occurred while detection'));
      }

      return resolve(result);
    });
  });
};

seeta.FaceDetector.prototype.detect = util.deprecate(
  seeta.FaceDetector.prototype.detectSync,
  'detect() is experimental. Use detectSync() instead'
);

/**
 * seeta::PointDetector
 */
seeta.PointDetector.prototype.detectSync = seeta.PointDetector.prototype.__detect_sync__;

seeta.PointDetector.prototype.detect = function (image, detector) {
  const that = this; // TODO: flatten namespace

  if (typeof image === 'string') {
    image = new seeta.Image(image);
  }

  try {
    validCaller.call(this, image, detector);
  } catch (err) {
    // reject by default
    return Promise.reject(err);
  }

  return new Promise(function (resolve, reject) {
    that.__detect__(image, detector, function (err, result) {
      if (err) {
        // reject by default
        return reject(err);
      }

      if (!result) {
        return reject(new Error('Error occurred while aignment'));
      }

      return resolve(result);
    });
  });
}

seeta.PointDetector.prototype.detect = util.deprecate(
  seeta.PointDetector.prototype.detectSync,
  'detect() is experimental. Use detectSync() instead'
);

/**
 * seeta::FaceRecognizer
 */
const __FaceRecognizer__ = seeta.FaceRecognizer;

seeta.FaceRecognizer.prototype.__defineGetter__('operational', function () { return this.isOperational(); })

seeta.FaceRecognizer.prototype.compareSync = function (images, detector, pointer) {
  const that = this; // TODO: flatten namespace

  if (!Array.isArray(images)) {
    throw new TypeError('`images` should be an array');
  }

  images.forEach(function (image, index) {
    if (typeof image === 'string') {
      image = new seeta.Image(image);
    }

    validCaller.call(that, image, detector, pointer);

    images[index] = image;
  });

  return this.__compare_sync__(images[0], images[1], detector, pointer);
}

seeta.FaceRecognizer.prototype.compare = function (images, detector, pointer) {
  const that = this; // TODO: flatten namespace

  images.forEach(function (image, index) {
    if (typeof image === 'string') {
      image = new seeta.Image(image);
    }

    try {
      validCaller.call(that, image, detector, pointer);
    } catch (err) {
      // reject by default
      return Promise.reject(err);
    }

    images[index] = image;
  });

  return new Promise(function (resolve, reject) {
    that.__compare__(image[0], image[1], detector, pointer, function (err, similar) {
      if (err) {
        // reject by default
        return reject(err);
      }

      return resolve(similar);
    });
  });
}

seeta.FaceRecognizer.prototype.recognizeSync = seeta.FaceRecognizer.prototype.__recognize_sync__;

seeta.FaceRecognizer.prototype.recognize = function (image, detector, pointer) {
  const that = this; // TODO: flatten namespace

  try {
    validCaller.call(that, image, detector, pointer);
  } catch (err) {
    // reject by default
    return Promise.reject(err);
  }

  return new Promise(function (resolve, reject) {
    that.__recognize__(image, detector, pointer, function (err, similars) {
      if (err) {
        // reject by default
        return reject(err);
      }

      return resolve(similars);
    });
  });
}

seeta.FaceRecognizer = function FaceRecognizer (model, opt) {
  opt = opt || {};
  this.constructor = __FaceRecognizer__;
  const lazyload = opt.lazyLoad || false;
  const callback = opt.onModelLoaded || noop;

  return new __FaceRecognizer__(model, lazyload, callback);
};

exports = module.exports = seeta;
