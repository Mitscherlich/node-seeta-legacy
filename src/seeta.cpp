#include "seeta_v8.h"

namespace v8 {
  void initialize(Local<Object> exports) {
    FaceDetector::init(exports);
    PointDetector::init(exports);
    FaceRecognizer::init(exports);
  }

  NODE_MODULE(NODE_GYP_MODULE_NAME, initialize)
}
