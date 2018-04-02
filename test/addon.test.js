'use strict';

const assert = require('assert');
const debug = require('debug')('node-seeta:test');


describe('test/addon.test.js', () => {

  let seeta;

  it('seeta should be an object', () => {
    seeta = require('..');

    assert(seeta)
  });

  describe('face detect', () => {

    it('should has function named \'detect\'', () => {
      assert(seeta['detect']);
    });
  })
});
