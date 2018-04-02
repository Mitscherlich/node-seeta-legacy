{
  'targets': [{

      'target_name': 'seeta',

      'sources': [
        'src/lodepng.cpp',

        'src/seeta.cc',
        'src/face_detect.cc',
        'src/face_align.cc'
      ],

      'cflags': [ '-Wall', '-Wextra', '-pedantic', '-ansi', '-O3' ],

      'conditions': [
        [ # cflags on OS X are stupid and have to be defined like this
          'OS==\"mac\"', {

            'include_dirs': [
              'deps/seetaface-engine/darwin/x64/include/facealign',
              'deps/seetaface-engine/darwin/x64/include/facedect',
              'deps/seetaface-engine/darwin/x64/include/viplnet',
              '<!(node -e "require(\'nan\')")', 'src'
            ],

            'cflags': [ '-flat_namespace', '-undefined suppress' ],
            'cflags_cc': [ '-flat_namespace', '-undefined suppress' ],

            'libraries': [
              # TODO: fix node-gyp behavior that requires ../
              '../deps/seetaface-engine/darwin/x64/lib/seeta_facedet_lib.framework/seeta_facedet_lib',
              '../deps/seetaface-engine/darwin/x64/lib/seeta_fa_lib.framework/seeta_fa_lib',
              '../deps/seetaface-engine/darwin/x64/lib/viplnet.framework/viplnet',
            ]

          }
        ]
      ]
    }
  ]
}
