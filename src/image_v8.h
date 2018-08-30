#ifndef NODE_SEETA_IMAGE_V8_H_
#define NODE_SEETA_IMAGE_V8_H_

#include <nan.h>

// TODO: fix seetaface2 behaviour like that...
#include <seeta/Struct_cv.h>

class ImageWrap : public seeta::cv::ImageData, public Nan::ObjectWrap {
public:
  static void Init(v8::Local<v8::Object> exports);

  explicit ImageWrap(std::string path);
  explicit ImageWrap(const std::vector<unsigned char> &image, const int w, const int h,  const int chs);
  ~ImageWrap();

private:
  static NAN_METHOD(New);
  static NAN_METHOD(Width);
  static NAN_METHOD(Height);
  static NAN_METHOD(Channels);
  static NAN_METHOD(ToBuffer);

  static Nan::Persistent<v8::Function> constructor;
};

typedef ImageWrap Image;

#endif  // NODE_SEETA_IMAGE_V8_H_
