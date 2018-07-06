#ifndef NODE_SEETA_FACE_RECOGNIZER_V8_H_
#define NODE_SEETA_FACE_RECOGNIZER_V8_H_

#include <node.h>
#include <node_object_wrap.h>

#include "seeta/FaceRecognizer.h"

namespace v8 {
  class FaceRecognizer : public node::ObjectWrap {
  public:
    static void init(Local<Object> exports);

  private:
    explicit FaceRecognizer(const char *model);
    ~FaceRecognizer();

    static Persistent<Function> constructor;

    seeta::FaceRecognizer2 *holder;

    static void create(const FunctionCallbackInfo<Value> &args);
    static void compare(const FunctionCallbackInfo<Value> &args);
    static void registerImage(const FunctionCallbackInfo<Value> &args);
    static void recognition(const FunctionCallbackInfo<Value> &args);
    static void clear(const FunctionCallbackInfo<Value> &args);
  };
}

#endif // NODE_SEETA_FACE_RECOGNIZER_V8_H_
