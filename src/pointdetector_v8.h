#ifndef NODE_SEETA_POINT_DETECTOR_V8_H_
#define NODE_SEETA_POINT_DETECTOR_V8_H_

#include <nan.h>

#include <seeta/PointDetector2.h>

class FaceRecognizerWrap;

class PointDetectorWrap : public Nan::ObjectWrap {
  friend class FaceRecognizerWrap;
public:
  static void Init(v8::Local<v8::Object> exports);

private:
  explicit PointDetectorWrap(std::string path);
  ~PointDetectorWrap();

  static NAN_METHOD(New);
  static NAN_METHOD(Detect);
  static NAN_METHOD(DetectSync);
  static NAN_METHOD(Landmarks);

  static Nan::Persistent<v8::Function> constructor;

  seeta::PointDetector2 *holder;

  class DetectionWorker;
};

#endif // NODE_SEETA_POINT_DETECTOR_V8_H_
