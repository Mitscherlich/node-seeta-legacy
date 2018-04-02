#include <string>
#include <vector>

#include "face_detection.h"

#include "seeta_v8.h"
#include "face_detect.h"

using namespace std;
using namespace seeta;
using namespace node;
using namespace v8;
using namespace Nan;


string Detect(ImageData &image, const char *modelPath)
{
  FaceDetection detector(modelPath);

  vector<FaceInfo> faces = detector.Detect(image);
  int count = faces.size();

  string result = "{\"faces\":[ ";  // @fix it: trim space

  for (int i = 0; i < count; i++) {
    result += seeta_v8::ParseFaceInfo(faces[i]);
    result += ",";
  }

  result.erase(result.end() - 1);
  result += "]}";

  return result;
}

class DetectionWorker : public AsyncWorker {
public:
  DetectionWorker(Callback *callback,
    const char *modelPath_, const char *imagePath_) : AsyncWorker(callback), modelPath(modelPath_), imagePath(imagePath_)
  {
    seeta_v8::LoadImage(image, imagePath.c_str());  // load png file
  }

  void Execute()
  {
    result = Detect(image, modelPath.c_str());  // get face detection result
  }

  void HandleOKCallback()
  {
    Local<Value> argv[] = { Null(), New<String>(result.c_str()).ToLocalChecked() };

    callback->Call(2, argv, async_resource);
  }

private:
  ImageData image;  // to be detected struct 'ImageData'
    
  string result;    // face detection result string
  string modelPath;  // model file path
  string imagePath;  // png file path
};

NAN_METHOD(DetectAsync)
{
  String::Utf8Value imagePath(info[0]->ToString());
  String::Utf8Value modelPath(info[1]->ToString());

  // bind v8 callback function with async callback
  Callback *callback = new Callback(To<Function>(info[2]).ToLocalChecked());

  AsyncQueueWorker(new DetectionWorker(callback, *modelPath, *imagePath));
}
