const path = require('path');

const query = process.argv[2];

function main(query) {
  switch (query) {
    case 'seeta':
      return path.join(__dirname, '..', 'deps/seetaface2', process.platform, process.arch);
  }
}

process.stdout.write(main(query).toString());
