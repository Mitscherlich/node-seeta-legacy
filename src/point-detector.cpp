#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>

#include "seeta/Struct_cv.h"

#include "point-detector_v8.h"

namespace v8 {
  Persistent<Function> PointDetector::constructor;

  PointDetector::PointDetector(const char *model)
  : holder(new seeta::PointDetector2(model)) {
  }

  PointDetector::~PointDetector() {
    if (holder) delete holder;
  }

  void PointDetector::init(Local<Object> exports) {
    Isolate *isolate = exports->GetIsolate();

    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, create);
    tpl->SetClassName(String::NewFromUtf8(isolate, "PointDetector"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(tpl, "detect", detect);

    // TODO: declare malloc mem space when program exit.
    // node::AtExit(cleanup);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "PointDetector"),
                tpl->GetFunction());
  }

  void PointDetector::create(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();

    if (args.Length() < 1) {
      // Throw an Error that is passed back to JavaScript
      isolate->ThrowException(Exception::Error(
          String::NewFromUtf8(isolate, "Too few arguments: Except 1 but got 0. 'modelPath' is needed in arguments.")));
      return;
    }

    if (!args[0]->IsString()) {
      isolate->ThrowException(Exception::TypeError(
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

      PointDetector *holder = new PointDetector(modelPath_.c_str());
      holder->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    } else {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Do not use as a normal function.")));
    }
  }

  void PointDetector::detect(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();

    if (args.Length() < 2) {
      // Throw an Error that is passed back to JavaScript
      isolate->ThrowException(Exception::Error(
          String::NewFromUtf8(isolate, "Too few arguments: Except 2. 'imagePath' and 'faceRect' is needed in arguments.")));
      return;
    }

    if (!args[0]->IsString()) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Except 'imagePath' should be a 'string'.")));
      return;
    }

    if (!args[1]->IsObject()) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Except 'faceRect' should be an 'object'.")));
      return;
    }

    Local<String> str = Local<String>::Cast(args[0]);
    String::Utf8Value utfValue(str);
    std::string imagePath = std::string(*utfValue);

    Local<Object> faceRect = Local<Object>::Cast(args[1]);
    int x = faceRect->Get(String::NewFromUtf8(isolate, "x"))->NumberValue();
    int y = faceRect->Get(String::NewFromUtf8(isolate, "y"))->NumberValue();
    int width = faceRect->Get(String::NewFromUtf8(isolate, "width"))->NumberValue();
    int height = faceRect->Get(String::NewFromUtf8(isolate, "height"))->NumberValue();
    SeetaRect face;
    face.x = x;
    face.y = y;
    face.width = width;
    face.height = height;

    cv::Mat mat = cv::imread(imagePath.c_str());
    seeta::cv::ImageData image = mat;

    Local<Object> obj = Object::New(isolate);
    Local<Object> data = Object::New(isolate);
    Local<Array> points = Array::New(isolate);

    PointDetector *pointer = ObjectWrap::Unwrap<PointDetector>(args.Holder());
    SeetaPointF *points_ = pointer->holder->Detect(image, face);
    for (int i = 0; i < pointer->holder->LandmarkNum(); ++i) {
      Local<Object> point = Object::New(isolate);
      point->Set(String::NewFromUtf8(isolate, "x"), Integer::New(isolate, points_[i].x));
      point->Set(String::NewFromUtf8(isolate, "y"), Integer::New(isolate, points_[i].y));
      points->Set(i, point);
    }

    data->Set(String::NewFromUtf8(isolate, "points"), points);

    obj->Set(String::NewFromUtf8(isolate, "data"), data);

    args.GetReturnValue().Set(obj);
  }
}
