{
  'variables': {
    'seeta_root%': '<!(node ./utils/configure seeta)'
  },
  'targets': [
    {
      'target_name': 'seeta',
      'include_dirs': [
        '<!(node -e \'require("nan")\')',
        '<(seeta_root)/include'
      ],
      'sources': [
        'src/seeta.cc',
        'src/image.cc',
        'src/facedetector.cc',
        'src/pointdetector.cc',
        'src/facerecognizer.cc'
      ],
      'conditions': [
        ['OS=="linux"', {
          'cflags': [ '<!@(node utils/find-opencv.js --cflags)', '-Wno-deprecated-declarations' ],
          'cflags!': [ '-fno-exceptions' ],
          'cflags_cc!': [ '-fno-rtti', '-fno-exceptions' ],
          'ldflags': [
            '-L<(seeta_root)/lib', '-Wl,-rpath,<(seeta_root)/lib',
            '<!@(node utils/find-opencv.js "--libs-only-L --libs-only-other")'
          ],
          'libraries': [
            '-lholiday',
            '-lSeetaFaceCropper200',
            '-lSeetaFaceDetector200',
            '-lSeetaFaceRecognizer200',
            '-lSeetaPointDetector200',
            '<!@(node utils/find-opencv.js --libs)'
          ]
        }],
        ['OS=="win"', {
          'libraries': [
            # TODO: fix node-gyp behaviour like that...
            '<(seeta_root)/lib/SeetaFaceCropper200.lib',
            '<(seeta_root)/lib/SeetaFaceDetector200.lib',
            '<(seeta_root)/lib/SeetaFaceRecognizer200.lib',
            '<(seeta_root)/lib/SeetaPointDetector200.lib'
          ],
          'copies': [{
            'destination': '<(PRODUCT_DIR)',
            'files': [
              '<(seeta_root)/lib/holiday.dll',
              '<(seeta_root)/lib/libeay32.dll',
              '<(seeta_root)/lib/libgcc_s_sjij-1.dll',
              '<(seeta_root)/lib/libgfortran-3.dll',
              '<(seeta_root)/lib/libopenblas.dll',
              '<(seeta_root)/lib/libquadmath-0.dll',
              '<(seeta_root)/lib/SeetaFaceCropper200.dll',
              '<(seeta_root)/lib/SeetaFaceDetector200.dll',
              '<(seeta_root)/lib/SeetaFaceRecognizer200.dll',
              '<(seeta_root)/lib/SeetaPointDetector200.dll',
              '<(seeta_root)/lib/ssleay32.dll'
            ]
          }]
        }]
      ]
    }
  ]
}
