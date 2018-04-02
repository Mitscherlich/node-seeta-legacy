#include <string>
#include <vector>

#include "face_detection.h"
#include "face_alignment.h"

#include "seeta_v8.h"
#include "face_align.h"

using namespace std;
using namespace seeta;
using namespace node;
using namespace v8;
using namespace Nan;


string Align(ImageData &image, const char *fdModelPath, const char *faModelPath)
{
  FaceDetection detector(fdModelPath);
  detector.SetMinFaceSize(40);
  detector.SetScoreThresh(2.f);
  detector.SetImagePyramidScaleFactor(0.8f);
  detector.SetWindowStep(4, 4);

  FaceAlignment aligner(faModelPath);

  vector<FaceInfo> faces = detector.Detect(image);
  int count = faces.size();
  string result = "{\"faces\":[";

  for (int i = 0; i < count; ++i)
  {
    // Detect 5 facial landmarks
    FacialLandmark points[5];
    aligner.PointDetectLandmarks(image, faces[i], points);
    result += "{\"points\":[";  // @fix it: trim space

    for (int j = 0; j < 5; j++)
    {
      result += seeta_v8::ParseFacialLandmark(points[j]);
      result += ",";
    }

    result.erase(result.end() - 1);
    result += "]},";
  }

  result.erase(result.end() - 1);
  result += "]}";

  return result;
}

class AlignmentWorker : public AsyncWorker {
public:
  AlignmentWorker(Callback *callback,
    const char *faModelPath_, const char *fdModelPath_, const char *imagePath_) : AsyncWorker(callback), faModelPath(faModelPath_), fdModelPath(fdModelPath_), imagePath(imagePath_)
  {
    seeta_v8::LoadImage(image, imagePath.c_str());  // load png file
  }

  void Execute()
  {
    result = Align(image, fdModelPath.c_str(), faModelPath.c_str());  // get face detection result
  }

  void HandleOKCallback()
  {
    Local<Value> argv[] = { Null(), New<String>(result.c_str()).ToLocalChecked() };

    callback->Call(2, argv, async_resource);
  }

private:
  ImageData image;  // to be detected struct 'ImageData'

  string result;    // face detection result string
  string faModelPath;  // fa model file path
  string fdModelPath;  // fd model file path
  string imagePath;  // png file path
};

NAN_METHOD(AlignAsync)
{
  String::Utf8Value imagePath(info[0]->ToString());
  String::Utf8Value fdModelPath(info[1]->ToString());
  String::Utf8Value faModelPath(info[2]->ToString());

  // bind v8 callback function with async callback
  Callback *callback = new Callback(To<Function>(info[3]).ToLocalChecked());

  AsyncQueueWorker(new AlignmentWorker(callback, *faModelPath, *fdModelPath, *imagePath));
}
