'use strict';

const assert = require('assert');
const debug = require('debug')('node-seeta:test');
const fs = require('fs');
const path = require('path');

const seeta = require('..');

const imagePath = path.join(__dirname, '..', 'data/image_0001.png');
const modelPath = path.join(__dirname, '..', 'models/seeta_fd_frontal_v1.0.bin');


describe('test/detect.test.js', () => {

  describe('detect async', () => {

    let image;

    before(done => {
      seeta.loadImage(imagePath, (err, destPath) => {
        debug({ error: err ? err.message : '', destPath });

        assert(!err);

        image = destPath;

        done();
      });
    });

    it('should get string result without error', done => {
      seeta.detect(image, modelPath, (err, result) => {
        assert(!err);

        debug(JSON.parse(result));

        done();
      });
    });
  });
});
