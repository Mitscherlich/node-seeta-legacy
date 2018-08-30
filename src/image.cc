#include <opencv2/opencv.hpp>

#include "image_v8.h"

ImageWrap::ImageWrap(std::string path): seeta::cv::ImageData(cv::imread(path.c_str())) {}

ImageWrap::~ImageWrap() {}

Nan::Persistent<v8::Function> ImageWrap::constructor;

void ImageWrap::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Image").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(4);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "__width__", Width);
  Nan::SetPrototypeMethod(tpl, "__height__", Height);
  Nan::SetPrototypeMethod(tpl, "__channels__", Channels);
  Nan::SetPrototypeMethod(tpl, "toBuffer", ToBuffer);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("Image").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(ImageWrap::New) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new Image(...)`
    Nan::Utf8String path(info[0]->ToString());
    ImageWrap* it = new ImageWrap(std::string(*path));
    it->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    // Invoked as plain function `Image(...)`, turn into construct call.
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { info[0] };
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
  }
}

NAN_METHOD(ImageWrap::Width) {
  ImageWrap* it = ObjectWrap::Unwrap<ImageWrap>(info.Holder());
  info.GetReturnValue().Set(Nan::New(it->width));
}

NAN_METHOD(ImageWrap::Height) {
  ImageWrap* it = ObjectWrap::Unwrap<ImageWrap>(info.Holder());
  info.GetReturnValue().Set(Nan::New(it->height));
}

NAN_METHOD(ImageWrap::Channels) {
  ImageWrap* it = ObjectWrap::Unwrap<ImageWrap>(info.Holder());
  info.GetReturnValue().Set(Nan::New(it->channels));
}

NAN_METHOD(ImageWrap::ToBuffer) {
  ImageWrap* it = ObjectWrap::Unwrap<ImageWrap>(info.Holder());
  Nan::MaybeLocal<v8::Object> buffer = Nan::CopyBuffer((const char *) it->data, it->width * it->height * it->channels);
  info.GetReturnValue().Set(buffer.ToLocalChecked());
}
