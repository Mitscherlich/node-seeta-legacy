#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>

#include "seeta/Struct_cv.h"

#include "face-detector_v8.h"

namespace v8 {
  Persistent<Function> FaceDetector::constructor;

  FaceDetector::FaceDetector(const char *model): holder(new seeta::FaceDetector2(model)) {
  }

  FaceDetector::~FaceDetector() {
    if (holder) delete holder;
  }

  void FaceDetector::init(Local<Object> exports) {
    Isolate *isolate = exports->GetIsolate();

    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, create);
    tpl->SetClassName(String::NewFromUtf8(isolate, "FaceDetector"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(tpl, "detect", detect);

    // TODO: declare malloc mem space when program exit.
    // node::AtExit(cleanup);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "FaceDetector"),
                tpl->GetFunction());
  }

  void FaceDetector::create(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();

    if (args.Length() < 1) {
      // Throw an Error that is passed back to JavaScript
      isolate->ThrowException(Exception::Error(
          String::NewFromUtf8(isolate, "Too few arguments: Except 1 but got 0. 'modelPath' is needed in arguments.")));
      return;
    }

    if (!args[0]->IsString()) {
      isolate->ThrowException(Exception::Error(
          String::NewFromUtf8(isolate, "Except 'modelPath' should be a 'string'.")));
      return;
    }

    if (args.IsConstructCall()) {
      Local<String> str = Local<String>::Cast(args[0]);
      String::Utf8Value utfValue(str);
      std::string modelPath_ = std::string(*utfValue);

      std::ifstream fin(modelPath_.c_str());
      if (!fin) {
        std::cout << "Model file not exist." << std::endl
                  << "You can download prebuild bindata here: https://pan.baidu.com/s/1HJj8PEnv3SOu6ZxVpAHPXg" << std::endl;
        isolate->ThrowException(Exception::Error(
          String::NewFromUtf8(isolate, ("Cannot locate model file in path: " + modelPath_).c_str())));
        return;
      }

      FaceDetector *detector = new FaceDetector(modelPath_.c_str());
      detector->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    } else {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Do not use as a normal function.")));
    }
  }

  void FaceDetector::detect(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();

    if (args.Length() < 1) {
      // Throw an Error that is passed back to JavaScript
      isolate->ThrowException(Exception::Error(
          String::NewFromUtf8(isolate, "Too few arguments: Except 1 but got 0. 'imagePath' is needed in arguments.")));
      return;
    }

    if (!args[0]->IsString()) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Except 'imagePath' should be a 'string'.")));
      return;
    }

    Local<String> str = Local<String>::Cast(args[0]);
    String::Utf8Value utfValue(str);
    std::string imagePath = std::string(*utfValue);

    cv::Mat mat = cv::imread(imagePath.c_str());
    seeta::cv::ImageData image = mat;
    int num;  // save the number of detected faces

    Local<Object> obj = Object::New(isolate);
    Local<Object> data = Object::New(isolate);
    Local<Array> faces = Array::New(isolate);

    FaceDetector *detector = ObjectWrap::Unwrap<FaceDetector>(args.Holder());
    SeetaRect *face = detector->holder->Detect(image, &num);

    obj->Set(String::NewFromUtf8(isolate, "count"), Integer::New(isolate, num));

    for (int i = 0; i < num; ++i, ++face) {
      Local<Object> face_ = Object::New(isolate);
      face_->Set(String::NewFromUtf8(isolate, "x"), Integer::New(isolate, face->x));
      face_->Set(String::NewFromUtf8(isolate, "y"), Integer::New(isolate, face->y));
      face_->Set(String::NewFromUtf8(isolate, "width"), Integer::New(isolate, face->width));
      face_->Set(String::NewFromUtf8(isolate, "height"), Integer::New(isolate, face->height));
      faces->Set(i, face_);
    }

    data->Set(String::NewFromUtf8(isolate, "faces"), faces);

    obj->Set(String::NewFromUtf8(isolate, "data"), data);

    args.GetReturnValue().Set(obj);
  }
}
