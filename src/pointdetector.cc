#include <iostream>
#include <sstream>

#include <node.h>

#include "image_v8.h"
#include "facedetector_v8.h"
#include "pointdetector_v8.h"

namespace {

std::string bowlingJson(SeetaRect* faces, SeetaPointF** points,int count, int landmarks) {
  // std::cout << "&face: " << face << std::endl;
  // std::cout << "x: " << face->x << std::endl;
  // std::cout << "y: " << face->y << std::endl;
  // std::cout << "width: " << face->width << std::endl;
  // std::cout << "height: " << face->height << std::endl;
  // Build detection result JSON
  std::ostringstream istr;
  istr << "{\"count\":" << count << ",";
  istr << "\"faces\":[ ";
  for (int i = 0; i < count; ++i) {
    istr << "{\"x\":" << faces[i].x << ",";
    istr << "\"y\":" << faces[i].y << ",";
    istr << "\"width\":" << faces[i].width << ",";
    istr << "\"height\":" << faces[i].height << ",";
    istr << "\"points\":[ ";
    SeetaPointF *facials = points[i];
    for (int j = 0; j < landmarks; ++j) {
      istr << "{\"x\":" << facials[j].x << ",";
      istr << "\"y\":" << facials[j].y << "},";
    }
    istr.seekp(-1, istr.cur); // backspace
    istr << "]},";
  }
  istr.seekp(-1, istr.cur); // backspace
  istr << "]}";
  return istr.str();
}

} // anonymous namespce

class PointDetectorWrap::DetectionWorker : public Nan::AsyncWorker {
public:
  DetectionWorker(Nan::Callback *callback, FaceDetectorWrap *holder1, PointDetectorWrap *holder2, ImageWrap *data)
    : Nan::AsyncWorker(callback), count(0), detector(holder1), pointer(holder2), image(data) {}
  ~DetectionWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute() {
    faces = detector->holder->Detect(*image, &count);
    points = new SeetaPointF*[count];
    for (int i = 0; i < count; ++i, ++faces) {
      points[i] = pointer->holder->Detect(*image, *faces);
    }
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback() {
    Nan::HandleScope scope;

    v8::Local<v8::String> json = Nan::New(bowlingJson(faces, points, count, pointer->holder->LandmarkNum()).c_str()).ToLocalChecked();
    Nan::JSON NanJSON;
    Nan::MaybeLocal<v8::Value> result = NanJSON.Parse(json);
    if (!result.IsEmpty()) {
      v8::Local<v8::Value> val = result.ToLocalChecked();
      v8::Local<v8::Value> argv[] = { Nan::Null(), val };
      callback->Call(2, argv, async_resource);
    }
    // delete points;
  }

private:
  int count;
  FaceDetectorWrap *detector;
  PointDetectorWrap *pointer;
  SeetaPointF **points;
  SeetaRect *faces;
  ImageWrap *image;
};

PointDetectorWrap::PointDetectorWrap(std::string path)
  : holder(new seeta::PointDetector2(path.c_str())) {}

PointDetectorWrap::~PointDetectorWrap() {
  delete holder;
}

Nan::Persistent<v8::Function> PointDetectorWrap::constructor;

void PointDetectorWrap::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("PointDetector").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(3);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "__detect__", Detect);
  Nan::SetPrototypeMethod(tpl, "__detect_sync__", DetectSync);
  Nan::SetPrototypeMethod(tpl, "landmarks", Landmarks);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("PointDetector").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(PointDetectorWrap::New) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new PointDetectorWrap(...)`
    Nan::Utf8String path(info[0]->ToString());
    PointDetectorWrap* it = new PointDetectorWrap(std::string(*path));
    it->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    // Invoked as plain function `PointDetectorWrap(...)`, turn into construct call.
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { info[0] };
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
  }
}

// Asynchronous access to the `Detect()` function
NAN_METHOD(PointDetectorWrap::Detect) {
  PointDetectorWrap* it = ObjectWrap::Unwrap<PointDetectorWrap>(info.Holder());
  // Nan::Utf8String path(info[0]->ToString());
  ImageWrap* image = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
  FaceDetectorWrap* detector = ObjectWrap::Unwrap<FaceDetectorWrap>(info[1]->ToObject());
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());

  Nan::AsyncQueueWorker(new DetectionWorker(callback, detector, it, image));
}

// Synchronous access to the `DetectSync()` function
NAN_METHOD(PointDetectorWrap::DetectSync) {
  PointDetectorWrap* it = ObjectWrap::Unwrap<PointDetectorWrap>(info.Holder());
  // Nan::Utf8String path(info[0]->ToString());
  ImageWrap* image = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
  FaceDetectorWrap* detector = ObjectWrap::Unwrap<FaceDetectorWrap>(info[1]->ToObject());
  int count = 0;

  SeetaRect* faces = detector->holder->Detect(*image, &count);
  SeetaPointF** points = new SeetaPointF*[count];
  for (int i = 0; i < count; ++i) {
    // std::cout << "x: " << faces->x << std::endl;
    // std::cout << "y: " << faces->y << std::endl;
    // std::cout << "width: " << faces->width << std::endl;
    // std::cout << "height: " << faces->height << std::endl;
    SeetaPointF *facial = it->holder->Detect(*image, faces[i]);
    points[i] = facial;
  }
  // std::cout << "&faces: " << faces << std::endl;
  v8::Local<v8::String> json = Nan::New(
    bowlingJson(faces, points, count, it->holder->LandmarkNum()).c_str()
  ).ToLocalChecked();
  Nan::JSON NanJSON;
  Nan::MaybeLocal<v8::Value> result = NanJSON.Parse(json);
  if (!result.IsEmpty()) {
    v8::Local<v8::Value> val = result.ToLocalChecked();
    info.GetReturnValue().Set(val);
  }
  // delete points;
}

NAN_METHOD(PointDetectorWrap::Landmarks) {
  PointDetectorWrap* it = ObjectWrap::Unwrap<PointDetectorWrap>(info.Holder());
  info.GetReturnValue().Set(Nan::New(it->holder->LandmarkNum()));
}
