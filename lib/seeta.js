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

exports = module.exports = seeta;
