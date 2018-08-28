try {
  require('.');
} catch (e) {
  // just exit
  process.stdout.write(e + '\r\n');
  process.exit(-1);
}

process.stdout.write('binary ok\r\n');
