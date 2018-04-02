'use strict';

const assert = require('assert');
const path = require('path');
const debug = require('debug')('node-seeta:test');

const imagePath = path.join(__dirname, '..', 'data/image_0001.png');
const faModelPath = path.join(__dirname, '..', 'models/seeta_fa_v1.1.bin');
const fdModelPath = path.join(__dirname, '..', 'models/seeta_fd_frontal_v1.0.bin');
const frModelPath = path.join(__dirname, '..', 'models/seeta_fr_v1.0.bin');


describe('test/addon.test.js', () => {

  let seeta;

  let image;

  before(() => {
    seeta = require('..');

    assert(seeta);
  });

  beforeEach(done => {
    seeta.loadImage(imagePath, (err, destPath) => {
      assert(!err);

      image = destPath;

      done();
    });
  });

  describe('face detect', () => {

    it('should has function named \'detect\'', () => {
      assert(seeta['detect']);
    });

    describe('detect with callback', () => {

      it('should get string result without error', done => {
        seeta.detect(image, fdModelPath, (err, result) => {
          assert(!err);

          debug(JSON.parse(result));

          done();
        });
      });
    });
  });

  describe('face align', () => {

    it('should has function named \'align\'', () => {
      assert(seeta['align']);
    });

    describe('align with callback', () => {

      it('should get string result without error', done => {
        seeta.align(image, fdModelPath, faModelPath, (err, result) => {
          assert(!err);

          debug(JSON.parse(result));

          done();
        });
      });
    });
  });
});
