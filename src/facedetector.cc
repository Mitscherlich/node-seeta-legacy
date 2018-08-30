#include <iostream>
#include <sstream>

#include <node.h>

#include "image_v8.h"
#include "facedetector_v8.h"

namespace {

std::string bowlingJson(SeetaRect* faces, int count) {
  // Build detection result JSON
  std::ostringstream istr;
  istr << "{\"count\":" << count << ",";
  istr << "\"faces\":[ ";
  for (int i = 0; i < count; ++i) {
    istr << "{\"x\":" << faces[i].x << ",";
    istr << "\"y\":" << faces[i].y << ",";
    istr << "\"width\":" << faces[i].width << ",";
    istr << "\"height\":" << faces[i].height << "},";
  }
  istr.seekp(-1, istr.cur); // backspace
  istr << "]}";
  return istr.str();
}

} // anonymous namespce

class FaceDetectorWrap::DetectionWorker : public Nan::AsyncWorker {
public:
  DetectionWorker(Nan::Callback *callback, FaceDetectorWrap *holder, ImageWrap *data)
    : Nan::AsyncWorker(callback), count(0), detector(holder), image(data) {}
  ~DetectionWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute() {
    faces = detector->holder->Detect(*image, &count);
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback() {
    Nan::HandleScope scope;

    v8::Local<v8::String> json = Nan::New(bowlingJson(faces, count).c_str()).ToLocalChecked();
    Nan::JSON NanJSON;
    Nan::MaybeLocal<v8::Value> result = NanJSON.Parse(json);
    if (!result.IsEmpty()) {
      v8::Local<v8::Value> val = result.ToLocalChecked();
      v8::Local<v8::Value> argv[] = { Nan::Null(), val };
      callback->Call(2, argv, async_resource);
    }
  }

private:
  int count;
  FaceDetectorWrap *detector;
  SeetaRect *faces;
  ImageWrap *image;
};

FaceDetectorWrap::FaceDetectorWrap(std::string path): holder(new seeta::FaceDetector2(path.c_str())) {}

FaceDetectorWrap::~FaceDetectorWrap() {
  delete holder;
}

Nan::Persistent<v8::Function> FaceDetectorWrap::constructor;

void FaceDetectorWrap::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("FaceDetector").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(2);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "__detect__", Detect);
  Nan::SetPrototypeMethod(tpl, "__detect_sync__", DetectSync);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("FaceDetector").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(FaceDetectorWrap::New) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new FaceDetector(...)`
    Nan::Utf8String path(info[0]->ToString());
    FaceDetectorWrap* it = new FaceDetectorWrap(std::string(*path));
    it->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    // Invoked as plain function `FaceDetector(...)`, turn into construct call.
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { info[0] };
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

// Asynchronous access to the `Detect()` function
NAN_METHOD(FaceDetectorWrap::Detect) {
  FaceDetectorWrap* it = ObjectWrap::Unwrap<FaceDetectorWrap>(info.Holder());
  // Nan::Utf8String path(info[0]->ToString());
  ImageWrap* image = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());

  Nan::AsyncQueueWorker(new DetectionWorker(callback, it, image));
}

// Synchronous access to the `DetectSync()` function
NAN_METHOD(FaceDetectorWrap::DetectSync) {
  FaceDetectorWrap* it = ObjectWrap::Unwrap<FaceDetectorWrap>(info.Holder());
  // Nan::Utf8String path(info[0]->ToString());
  ImageWrap* image = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
  int count = 0;

  SeetaRect* faces = it->holder->Detect(*image, &count);
  v8::Local<v8::String> json = Nan::New(bowlingJson(faces, count).c_str()).ToLocalChecked();
  Nan::JSON NanJSON;
  Nan::MaybeLocal<v8::Value> result = NanJSON.Parse(json);
  if (!result.IsEmpty()) {
    v8::Local<v8::Value> val = result.ToLocalChecked();
    info.GetReturnValue().Set(val);
  }
}
