#ifndef NODE_SEETA_FACE_DETECTOR_V8_H_
#define NODE_SEETA_FACE_DETECTOR_V8_H_

#include <nan.h>

#include <seeta/FaceDetector2.h>

class PointDetectorWrap;
class FaceRecognizerWrap;

class FaceDetectorWrap : public Nan::ObjectWrap {
  friend class PointDetectorWrap;
  friend class FaceRecognizerWrap;
public:
  static void Init(v8::Local<v8::Object> exports);

private:
  explicit FaceDetectorWrap(std::string path);
  ~FaceDetectorWrap();

  static NAN_METHOD(New);
  static NAN_METHOD(Detect);
  static NAN_METHOD(DetectSync);

  static Nan::Persistent<v8::Function> constructor;

  seeta::FaceDetector2 *holder;

  class DetectionWorker;
};

#endif // NODE_SEETA_FACE_DETECTOR_V8_H_
