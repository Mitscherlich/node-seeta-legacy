#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>

#include "seeta/Struct_cv.h"

#include "face-recognizer_v8.h"

namespace v8 {
  Persistent<Function> FaceRecognizer::constructor;

  FaceRecognizer::FaceRecognizer(const char *model): holder(new seeta::FaceRecognizer2(model)) {
  }

  FaceRecognizer::~FaceRecognizer() {
    if (holder) delete holder;
  }

  void FaceRecognizer::init(Local<Object> exports) {
    Isolate *isolate = exports->GetIsolate();

    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, create);
    tpl->SetClassName(String::NewFromUtf8(isolate, "FaceRecognizer"));
    tpl->InstanceTemplate()->SetInternalFieldCount(4);

    NODE_SET_PROTOTYPE_METHOD(tpl, "register", registerImage);
    NODE_SET_PROTOTYPE_METHOD(tpl, "compare", compare);
    NODE_SET_PROTOTYPE_METHOD(tpl, "recognition", recognition);
    NODE_SET_PROTOTYPE_METHOD(tpl, "clear", clear);

    // TODO: declare malloc mem space when program exit.
    // node::AtExit(cleanup);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "FaceRecognizer"),
                tpl->GetFunction());
  }

  void FaceRecognizer::create(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();

    if (args.Length() < 1) {
      // Throw an Error that is passed back to JavaScript
      isolate->ThrowException(Exception::TypeError(
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
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, ("Cannot locate model file in path: " + modelPath_).c_str())));
        return;
      }

      FaceRecognizer *detector = new FaceRecognizer(modelPath_.c_str());
      detector->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    } else {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Do not use as a normal function.")));
    }
  }

  /**
   * \brief compare similar between two images
   * \param args javascript function arguments
   * */
  void FaceRecognizer::registerImage(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();

    if (args.Length() < 2) {
      // Throw an Error that is passed back to JavaScript
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Too few arguments: Except 2 but got less.")));
      return;
    }

    if (!args[0]->IsString()) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Except 'imagePath' should be a 'string'.")));
      return;
    }

    if (!args[1]->IsArray()) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Except 'points' should be an 'Array'.")));
      return;
    }

    Local<String> str = Local<String>::Cast(args[0]);
    String::Utf8Value utfValue(str);
    std::string imagePath = std::string(*utfValue);

    Local<Array> array = Local<Array>::Cast(args[1]);
    const int length = array->Length();
    if (length < 1) {
      args.GetReturnValue().Set(Integer::New(isolate, -1));
      return;
    }
    SeetaPointF* points = new SeetaPointF[length];
    for (int i = 0; i < length; i++) {
      points[i].x = Local<Object>::Cast(array->Get(i))->Get(String::NewFromUtf8(isolate, "x"))->NumberValue();
      points[i].y = Local<Object>::Cast(array->Get(i))->Get(String::NewFromUtf8(isolate, "y"))->NumberValue();
    }
    
    cv::Mat mat = cv::imread(imagePath.c_str());
    if (mat.empty()) {
      args.GetReturnValue().Set(Integer::New(isolate, -1));
      return;
    }
    seeta::cv::ImageData image = mat;
    FaceRecognizer *recognizer = ObjectWrap::Unwrap<FaceRecognizer>(args.Holder());
    int index = recognizer->holder->Register(image, points);
    args.GetReturnValue().Set(Integer::New(isolate, index));
  }

  void FaceRecognizer::compare(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    // parse function argument
    if (args.Length() < 2) {
      // Throw an Error that is passed back to JavaScript
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Too few arguments: Except 2 but got less.")));
      return;
    }

    if (!args[0]->IsObject()) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Except 'image1' should be a 'Object'.")));
      return;
    }

    if (!args[1]->IsObject()) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Except 'image2' should be an 'Object'.")));
      return;
    }
    // ========================================================================
    // arg[0] is image1
    Local<Object> arg0 = Local<Object>::Cast(args[0]);
    // parse image path
    Local<Value> path1 = arg0->Get(String::NewFromUtf8(isolate, "path"));
    // if (!path1.IsString()) {
    //   isolate->ThrowException(Exception::TypeError(
    //       String::NewFromUtf8(isolate, "Except 'image1.path' should be an 'Object'.")));
    //   return;
    // }
    Local<String> str1 = Local<String>::Cast(path1);
    String::Utf8Value utfValue1(str1);
    std::string imagePath1 = std::string(*utfValue1);
    // parse facial points
    Local<Array> array1 = Local<Array>::Cast(arg0->Get(String::NewFromUtf8(isolate, "points")));
    const int length1 = array1->Length();
    if (length1 < 1) {
      isolate->ThrowException(Exception::Error(
          String::NewFromUtf8(isolate, "There is not facial points in image1.")));
      return;
    }
    SeetaPointF* points1 = new SeetaPointF[length1];
    for (int i = 0; i < length1; i++) {
      points1[i].x = Local<Object>::Cast(array1->Get(i))->Get(String::NewFromUtf8(isolate, "x"))->NumberValue();
      points1[i].y = Local<Object>::Cast(array1->Get(i))->Get(String::NewFromUtf8(isolate, "y"))->NumberValue();
    }
    // parse image data
    cv::Mat mat1 = cv::imread(imagePath1.c_str());
    seeta::cv::ImageData image1 = mat1;
    // ========================================================================
    // arg[1] is image2
    Local<Object> arg1 = Local<Object>::Cast(args[1]);
    // parse image path
    Local<Value> path2 = arg1->Get(String::NewFromUtf8(isolate, "path"));
    // if (!path2.IsString()) {
    //   isolate->ThrowException(Exception::TypeError(
    //       String::NewFromUtf8(isolate, "Except 'image2.path' should be an 'Object'.")));
    //   return;
    // }
    Local<String> str2 = Local<String>::Cast(path2);
    String::Utf8Value utfValue2(str2);
    std::string imagePath2 = std::string(*utfValue2);
    // parse facial points
    Local<Array> array2 = Local<Array>::Cast(arg1->Get(String::NewFromUtf8(isolate, "points")));
    const int length2 = array2->Length();
    if (length2 < 1) {
      isolate->ThrowException(Exception::Error(
          String::NewFromUtf8(isolate, "There is not facial points in image2.")));
      return;
    }
    SeetaPointF* points2 = new SeetaPointF[length2];
    for (int i = 0; i < length2; i++) {
      points2[i].x = Local<Object>::Cast(array2->Get(i))->Get(String::NewFromUtf8(isolate, "x"))->NumberValue();
      points2[i].y = Local<Object>::Cast(array2->Get(i))->Get(String::NewFromUtf8(isolate, "y"))->NumberValue();
    }
    // parse image data
    cv::Mat mat2 = cv::imread(imagePath2.c_str());
    seeta::cv::ImageData image2 = mat2;
    // ========================================================================
    // compare two image
    FaceRecognizer *recognizer = ObjectWrap::Unwrap<FaceRecognizer>(args.Holder());
    float similar = recognizer->holder->Compare(image1, points1, image2, points2);
    args.GetReturnValue().Set(Number::New(isolate, similar));
  }

  /**
   * \brief recognize face and get all faces' similars
   * \param args javascript function arguments
   * */
  void FaceRecognizer::recognition(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    // parse function argument
    if (args.Length() < 2) {
      // Throw an Error that is passed back to JavaScript
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Too few arguments: Except 2 but got less.")));
      return;
    }

    if (!args[0]->IsString()) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Except 'imagePath' should be a 'string'.")));
      return;
    }

    if (!args[1]->IsArray()) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Except 'points' should be an 'Array'.")));
      return;
    }
    // ========================================================================
    // arg[0] is image path
    // parse image path
    Local<String> str = Local<String>::Cast(args[0]);
    String::Utf8Value utfValue(str);
    std::string imagePath = std::string(*utfValue);
    // parse image data
    cv::Mat mat = cv::imread(imagePath.c_str());
    seeta::cv::ImageData image = mat;
    // ========================================================================
    // arg[1] is image path
    // parse facial points
    Local<Array> array = Local<Array>::Cast(args[1]);
    const int length = array->Length();
    if (length < 1) {
      isolate->ThrowException(Exception::Error(
          String::NewFromUtf8(isolate, "There is not facial points in image.")));
      return;
    }
    SeetaPointF* points = new SeetaPointF[length];
    for (int i = 0; i < length; i++) {
      points[i].x = Local<Object>::Cast(array->Get(i))->Get(String::NewFromUtf8(isolate, "x"))->NumberValue();
      points[i].y = Local<Object>::Cast(array->Get(i))->Get(String::NewFromUtf8(isolate, "y"))->NumberValue();
    }
    // ========================================================================
    // recognize face and get all faces' similars
    FaceRecognizer *recognizer = ObjectWrap::Unwrap<FaceRecognizer>(args.Holder());
    if (recognizer->holder->MaxRegisterIndex() < 1) {
      isolate->ThrowException(Exception::Error(
          String::NewFromUtf8(isolate, "There is no image in group. You should register it first.")));
      return;
    }
    float *similar = recognizer->holder->RecognizeEx(image, points);
    Local<Object> obj = Object::New(isolate);
    Local<Array> data = Array::New(isolate);
    for (int i = 0; i < recognizer->holder->MaxRegisterIndex(); ++i) {
      data->Set(i, Number::New(isolate, similar[i]));
    }
    obj->Set(String::NewFromUtf8(isolate, "data"), data);
    args.GetReturnValue().Set(obj);
  }

  /**
   * \brief clear recognizer's face database
   * \param args javascript function arguments
   * */
  void FaceRecognizer::clear(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    // ========================================================================
    // clear face database
    FaceRecognizer *recognizer = ObjectWrap::Unwrap<FaceRecognizer>(args.Holder());
    recognizer->holder->Clear();
  }
}
