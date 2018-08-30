#include <nan.h>

#include "image_v8.h"
#include "facedetector_v8.h"
#include "facerecognizer_v8.h"
#include "pointdetector_v8.h"

void InitAll(v8::Local<v8::Object> exports) {
  ImageWrap::Init(exports);

  FaceDetectorWrap::Init(exports);
  FaceRecognizerWrap::Init(exports);
  PointDetectorWrap::Init(exports);
}

NODE_MODULE(seeta, InitAll)
