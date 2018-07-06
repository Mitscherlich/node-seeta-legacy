#ifndef NODE_SEETA_FACE_DETECTOR_V8_H_
#define NODE_SEETA_FACE_DETECTOR_V8_H_

#include <node.h>
#include <node_object_wrap.h>

#include "seeta/FaceDetector2.h"

namespace v8 {
  class FaceDetector : public node::ObjectWrap {
  public:
    static void init(Local<Object> exports);

  private:
    explicit FaceDetector(const char *model);
    ~FaceDetector();

    static Persistent<Function> constructor;

    seeta::FaceDetector2 *holder;

    static void create(const FunctionCallbackInfo<Value> &args);
    static void detect(const FunctionCallbackInfo<Value> &args);
  };
}

#endif // NODE_SEETA_FACE_DETECTOR_V8_H_
