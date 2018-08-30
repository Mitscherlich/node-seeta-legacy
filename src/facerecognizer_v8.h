#ifndef NODE_SEETA_FACE_RECOGNIZER_V8_H_
#define NODE_SEETA_FACE_RECOGNIZER_V8_H_

#include <nan.h>

#include <seeta/FaceRecognizer.h>

class FaceRecognizerWrap : public Nan::ObjectWrap {
public:
  static void Init(v8::Local<v8::Object> exports);

private:
  explicit FaceRecognizerWrap(std::string path, bool lazy);
  ~FaceRecognizerWrap();

  static NAN_METHOD(New);
  static NAN_METHOD(Clear);
  static NAN_METHOD(Compare);
  static NAN_METHOD(CompareSync);
  static NAN_METHOD(IsOperational);
  static NAN_METHOD(Recognize);
  static NAN_METHOD(RecognizeSync);
  static NAN_METHOD(Register);

  static Nan::Persistent<v8::Function> constructor;

  bool isOperational;
  std::string model;
  seeta::FaceRecognizer2 *holder;

  class LazyLoader;
  class CompareWorker;
  class RecognitionWorker;
};

#endif // NODE_SEETA_FACE_RECOGNIZER_V8_H_
