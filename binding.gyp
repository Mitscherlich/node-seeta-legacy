{
  "variables": {
    "seeta_root%": "<!(node ./utils/configure seeta)"
  },
  "targets": [
    {
      "target_name": "seeta",
      "sources": [
        "src/seeta.cpp",
        "src/face-detector.cpp",
        "src/point-detector.cpp",
        "src/face-recognizer.cpp"
      ],
      "conditions": [
        ["OS==\"linux\"", {
          "include_dirs": [ "<(seeta_root)/include" ],
          "cflags": [
            "<!@(pkg-config --cflags opencv)"
          ],
          "ldflags": [
            "-L<(seeta_root)/lib", "-Wl,-rpath,<(seeta_root)/lib",
            "<!@(pkg-config --libs-only-L --libs-only-other opencv)"
          ],
          "libraries": [
            "-lholiday",
            "-lSeetaFaceDetector200",
            "-lSeetaFaceRecognizer200",
            "-lSeetaPointDetector200",
            "<!@(pkg-config --libs-only-l opencv)"
          ]
        }]
      ]
    }
  ]
}
