#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include <node.h>

#include "image_v8.h"
#include "facedetector_v8.h"
#include "facerecognizer_v8.h"
#include "pointdetector_v8.h"

namespace {

std::string bowlingJson(float *similars, int max) {
  std::ostringstream istr;
  istr << "[ ";
  for (int i = 0; i < max; ++i) {
    istr << similars[i] << ",";
  }
  istr.seekp(-1, istr.cur); // backspace
  istr << "]";
  return istr.str();
}

} // anonymous namespce

/**
 * FaceRecognizerWrap::LazyLoader
 */
class FaceRecognizerWrap::LazyLoader : public Nan::AsyncWorker {
public:
  LazyLoader(Nan::Callback *callback, FaceRecognizerWrap *holder)
    : Nan::AsyncWorker(callback), recognizer(holder) {}
  ~LazyLoader() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute() {
    recognizer->holder = new seeta::FaceRecognizer2(recognizer->model.c_str());
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback() {
    recognizer->isOperational = true;
    v8::Local<v8::Value> argv[] = { Nan::Null() };
    callback->Call(2, argv, async_resource);
  }

private:
  FaceRecognizerWrap *recognizer;
  std::string model;
};

/**
 * FaceRecognizerWrap::CompareWorker
 */
class FaceRecognizerWrap::CompareWorker : public Nan::AsyncWorker {
public:
  CompareWorker(Nan::Callback *callback, FaceRecognizerWrap *holder, std::vector<std::map<ImageWrap*, SeetaPointF*>> *params)
    : Nan::AsyncWorker(callback), similar(0), recognizer(holder), faces(params) {
  }
  ~CompareWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute() {
    std::map<ImageWrap*, SeetaPointF*>::iterator iter;
    // ========================================================================
    // parse image1
    std::map<ImageWrap*, SeetaPointF*> &bounding1 = (*faces)[0];
    iter = bounding1.begin();
    seeta::cv::ImageData* image1 = iter->first;  SeetaPointF* facial1 = iter->second;
    // parse image2
    // ========================================================================
    std::map<ImageWrap*, SeetaPointF*> &bounding2 = (*faces)[1];
    iter = bounding2.begin();
    seeta::cv::ImageData* image2 = iter->first;  SeetaPointF* facial2 = iter->second;
    // ========================================================================
    similar = recognizer->holder->Compare(*image1, facial1, *image2, facial2);
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback() {
    v8::Local<v8::Value> argv[] = { Nan::Null(), Nan::New(similar) };
    callback->Call(2, argv, async_resource);
  }

private:
  float similar;
  FaceRecognizerWrap *recognizer;
  std::vector<std::map<ImageWrap*, SeetaPointF*>> *faces;
};

/**
 * FaceRecognizerWrap::RecognitionWorker
 */
class FaceRecognizerWrap::RecognitionWorker : public Nan::AsyncWorker {
public:
  RecognitionWorker(Nan::Callback *callback, FaceDetectorWrap *holder1, PointDetectorWrap *holder2, FaceRecognizerWrap *holder3, ImageWrap *data)
    : Nan::AsyncWorker(callback), detector(holder1), pointer(holder2), recognizer(holder3), image(data) {
      if (recognizer->holder->MaxRegisterIndex() < 1) {
        Nan::ThrowError("No member in databse. Regist first");
        return;
      }
    }

  ~RecognitionWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute() {
    int count;
    SeetaRect *face = detector->holder->Detect(*image, &count);
    SeetaPointF *facial = pointer->holder->Detect(*image, *face);
    similars = recognizer->holder->RecognizeEx(*image, facial);
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback() {
    v8::Local<v8::String> json = Nan::New(
      bowlingJson(similars, recognizer->holder->MaxRegisterIndex()).c_str()
    ).ToLocalChecked();
    Nan::JSON NanJSON;
    Nan::MaybeLocal<v8::Value> result = NanJSON.Parse(json);
    if (!result.IsEmpty()) {
      v8::Local<v8::Value> val = result.ToLocalChecked();
      v8::Local<v8::Value> argv[] = { Nan::Null(), val };
      callback->Call(2, argv, async_resource);
    }
  }

private:
  float* similars;
  FaceDetectorWrap *detector;
  PointDetectorWrap *pointer;
  FaceRecognizerWrap *recognizer;
  ImageWrap *image;
};

FaceRecognizerWrap::FaceRecognizerWrap(std::string path, bool lazy)
  : isOperational(false), model(path) {
    if (!lazy) {
      this->holder = new seeta::FaceRecognizer2(model.c_str());
      isOperational = true;
    }
  }

FaceRecognizerWrap::~FaceRecognizerWrap() {
  delete holder;
}

Nan::Persistent<v8::Function> FaceRecognizerWrap::constructor;

void FaceRecognizerWrap::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("FaceRecognizer").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(7);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "clear", Clear);
  Nan::SetPrototypeMethod(tpl, "isOperational", IsOperational);
  Nan::SetPrototypeMethod(tpl, "register", Register);
  Nan::SetPrototypeMethod(tpl, "__compare__", Compare);
  Nan::SetPrototypeMethod(tpl, "__compare_sync__", CompareSync);
  Nan::SetPrototypeMethod(tpl, "__recognize__", Recognize);
  Nan::SetPrototypeMethod(tpl, "__recognize_sync__", RecognizeSync);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("FaceRecognizer").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(FaceRecognizerWrap::New) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new FaceRecognizerWrap(...)`
    Nan::Utf8String path(info[0]->ToString());
    bool lazy = Nan::To<bool>(info[1]).FromJust();
    Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());
    FaceRecognizerWrap* it = new FaceRecognizerWrap(std::string(*path), lazy);
    if (lazy) {
      Nan::AsyncQueueWorker(new LazyLoader(callback, it));
    }
    it->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    // Invoked as plain function `FaceRecognizerWrap(...)`, turn into construct call.
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { info[0] };
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

NAN_METHOD(FaceRecognizerWrap::Clear) {
  FaceRecognizerWrap* it = ObjectWrap::Unwrap<FaceRecognizerWrap>(info.Holder());
  it->holder->Clear();
}

// Asynchronous access to the `Compare()` function
NAN_METHOD(FaceRecognizerWrap::Compare) {
  FaceRecognizerWrap* it = ObjectWrap::Unwrap<FaceRecognizerWrap>(info.Holder());
  ImageWrap* image1 = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
  ImageWrap* image2 = ObjectWrap::Unwrap<ImageWrap>(info[1]->ToObject());
  FaceDetectorWrap* detector = ObjectWrap::Unwrap<FaceDetectorWrap>(info[2]->ToObject());
  PointDetectorWrap* pointer = ObjectWrap::Unwrap<PointDetectorWrap>(info[3]->ToObject());
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[4]).ToLocalChecked());
  // ========================================================================
  std::vector<std::map<ImageWrap*, SeetaPointF*>> params;
  int count;
  // ========================================================================
  // parse image1
  SeetaRect* faces1 = detector->holder->Detect(*image1, &count);
  SeetaPointF* facial1 = pointer->holder->Detect(*image1, *faces1);
  std::map<ImageWrap*, SeetaPointF*> bounding1;
  bounding1.insert(std::pair<ImageWrap*, SeetaPointF*>(image1, facial1));
  // parse image2
  SeetaRect* faces2 = detector->holder->Detect(*image2, &count);
  SeetaPointF* facial2 = pointer->holder->Detect(*image2, *faces2);
  std::map<ImageWrap*, SeetaPointF*> bounding2;
  bounding2.insert(std::pair<ImageWrap*, SeetaPointF*>(image2, facial2));
  // ========================================================================
  params.push_back(bounding1);  params.push_back(bounding2);
  Nan::AsyncQueueWorker(new CompareWorker(callback, it, &params));
}

// Synchronous access to the `CompareSync()` function
NAN_METHOD(FaceRecognizerWrap::CompareSync) {
  FaceRecognizerWrap* it = ObjectWrap::Unwrap<FaceRecognizerWrap>(info.Holder());
  ImageWrap* image1 = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
  ImageWrap* image2 = ObjectWrap::Unwrap<ImageWrap>(info[1]->ToObject());
  FaceDetectorWrap* detector = ObjectWrap::Unwrap<FaceDetectorWrap>(info[2]->ToObject());
  PointDetectorWrap* pointer = ObjectWrap::Unwrap<PointDetectorWrap>(info[3]->ToObject());
  int count;
  // ========================================================================
  // parse image1
  SeetaRect* face1 = detector->holder->Detect(*image1, &count);
  SeetaPointF* facial1 = pointer->holder->Detect(*image1, *face1);
  // parse image2
  SeetaRect* face2 = detector->holder->Detect(*image2, &count);
  SeetaPointF* facial2 = pointer->holder->Detect(*image2, *face2);
  // ========================================================================
  float similar = it->holder->Compare(*image1, facial1, *image2, facial2);
  info.GetReturnValue().Set(Nan::New(similar));
}

NAN_METHOD(FaceRecognizerWrap::IsOperational) {
  FaceRecognizerWrap* it = ObjectWrap::Unwrap<FaceRecognizerWrap>(info.Holder());
  info.GetReturnValue().Set(Nan::New(it->isOperational));
}

// Asynchronous access to the `Recognize()` function
NAN_METHOD(FaceRecognizerWrap::Recognize) {
  FaceRecognizerWrap* it = ObjectWrap::Unwrap<FaceRecognizerWrap>(info.Holder());
  ImageWrap* image = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
  FaceDetectorWrap* detector = ObjectWrap::Unwrap<FaceDetectorWrap>(info[1]->ToObject());
  PointDetectorWrap* pointer = ObjectWrap::Unwrap<PointDetectorWrap>(info[2]->ToObject());
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[3]).ToLocalChecked());
  // ========================================================================
  Nan::AsyncQueueWorker(new RecognitionWorker(callback, detector, pointer, it, image));
}

// Synchronous access to the `RecognizeSync()` function
NAN_METHOD(FaceRecognizerWrap::RecognizeSync) {
  FaceRecognizerWrap* it = ObjectWrap::Unwrap<FaceRecognizerWrap>(info.Holder());
  ImageWrap* image = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
  FaceDetectorWrap* detector = ObjectWrap::Unwrap<FaceDetectorWrap>(info[1]->ToObject());
  PointDetectorWrap* pointer = ObjectWrap::Unwrap<PointDetectorWrap>(info[2]->ToObject());
  int count;
  // ========================================================================
  if (it->holder->MaxRegisterIndex() < 1) {
    Nan::ThrowError("No member in databse. Regist first");
    return;
  }
  // ======================= Detect and align ===============================
  SeetaRect* face = detector->holder->Detect(*image, &count);
  SeetaPointF* facial = pointer->holder->Detect(*image, *face);
  // ========================================================================
  float* similars = it->holder->RecognizeEx(*image, facial);
  v8::Local<v8::String> json = Nan::New(
    bowlingJson(similars, it->holder->MaxRegisterIndex()).c_str()
  ).ToLocalChecked();
  Nan::JSON NanJSON;
  Nan::MaybeLocal<v8::Value> result = NanJSON.Parse(json);
  if (!result.IsEmpty()) {
    v8::Local<v8::Value> val = result.ToLocalChecked();
    info.GetReturnValue().Set(val);
  }
}

// Synchronous access to the `Register()` function
NAN_METHOD(FaceRecognizerWrap::Register) {
  FaceRecognizerWrap* it = ObjectWrap::Unwrap<FaceRecognizerWrap>(info.Holder());
  ImageWrap* image = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
  FaceDetectorWrap* detector = ObjectWrap::Unwrap<FaceDetectorWrap>(info[1]->ToObject());
  PointDetectorWrap* pointer = ObjectWrap::Unwrap<PointDetectorWrap>(info[2]->ToObject());
  int count;

  SeetaRect *face = detector->holder->Detect(*image, &count);
  SeetaPointF *facial = pointer->holder->Detect(*image, *face);
  int index = it->holder->Register(*image, facial);
  info.GetReturnValue().Set(Nan::New(index));
}
