const exec = require('child_process').exec;
const fs = require('fs');
const flag = process.argv[2] || '--exists';

function main() {
  //  try using `pkg-config`, but fails on windows, try the fallback
  exec('pkg-config opencv ' + flag, function (err, stdout, stderr) {
    if (err) {
      if (process.platform === 'win32') {
        fallback();
      } else {
        throw new Error('ERROR: failed to run: pkg-config opencv', flag);
      }
    } else {
      process.stdout.write(stdout);
    }
  });
}

// ====================== Windows Specific =======================================

function fallback() {
  exec('echo %OPENCV_DIR%', function (err, stdout, stderr) {
    cleanupEchoOutput(stdout);
    if (err) {
      throw new Error('ERROR: There was an error reading OPENCV_DIR');
    } else if (stdout === '%OPENCV_DIR%') {
      throw new Error('ERROR: OPENCV_DIR doesn\'t seem to be defined');
    } else {
      printPaths(stdout);
    }
  })
}

function printPaths(opencvPath) {
  if (flag === '--cflags') {
    console.log('"' + opencvPath + '\\..\\..\\include"');
    console.log('"' + opencvPath + '\\..\\..\\include\\opencv"');
  } else if (flag === '--libs') {
    const libPath = opencvPath + '\\lib\\';
    fs.readdir(libPath, function(err, files) {
      if (err) {
        throw new Error('ERROR: couldn\'t read the lib directory', err);
      }

      let libs = '';
      for (let i = 0; i < files.length; i++) {
        if (getExtension(files[i]) === 'lib') {
          libs = libs + ' "' + libPath + files[i] + '" \r\n ';
        }
      }
      process.stdout.write(libs);
    });
  } else {
    throw new Error('Error: unknown argument \"' + flag + '\"');
  }
}

function cleanupEchoOutput(s) {
  return s.slice(0, s.length - 2);
}

function getExtension(s) {
  return s.substr(s.lastIndexOf('.') + 1);
}

main();
