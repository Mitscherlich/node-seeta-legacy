//
// Load bindings binary
//
const oldDir = process.cwd();
try {
  // ensure we're in the right location so we can dynamically load the bundled Seetaface Engine libraries
  process.chdir(__dirname + '/../deps/seetaface-engine-wrapper/' + process.platform + '/' + process.arch);

} catch (e) {
  // if no local deps/ dir, assume shared lib linking. keep going
}
const seeta = require(__dirname + '/../build/Release/seeta.node');
process.chdir(oldDir);

const fs = require('fs');
const path = require('path');
const exec = require('child_process').execFile;
const tmpdir = require('os').tmpdir();
const debug = require('debug')('node-seeta:addon');
const im = '/usr/local/bin/convert';
const suffix = 'png';


seeta.loadImage = (imagePath, cb) => {
  if (!fs.existsSync(imagePath)) {
    return cb(new Error(`file: '${imagePath}' not exist!`));
  }

  const extension = imagePath.split('.').pop();
  const timestamp = Date.now();
  const destPath = path.join(tmpdir, `${timestamp}.${suffix}`)

  if (extension !== 'png') {
    return cb(new Error(`file: expect be '.${suffix}' but got '.${extension}'`));
  }

  exec(im, [ imagePath, '-colorspace', 'Gray', destPath ], (err, stdout, stderr) => {
    if (err) {
      return cb(err);
    }

    if (stderr) {
      return cb(stderr);
    }

    debug({ stdout, stderr });

    if (!fs.existsSync(destPath)) {
      return cb(new Error('gernate grayscale image failed'));
    }

    return cb(null, destPath);
  });
};


module.exports = seeta;
