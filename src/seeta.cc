#include <string>

#include "seeta_v8.h"

#include "lodepng.h"
#include "face_detect.h"
#include "face_align.h"

using namespace std;
using namespace seeta;
using namespace v8;

using Nan::GetFunction;
using Nan::New;


const char *seeta_v8::ParseFaceInfo(FaceInfo &face)
{
  char *tmpStr = new char[128];
  sprintf(tmpStr, "[%d,%d,%d,%d]", face.bbox.x, face.bbox.y, face.bbox.width, face.bbox.height);
  return tmpStr;
}

const char *seeta_v8::ParseFacialLandmark(FacialLandmark &point)
{
  char *tmpStr = new char[128];
  sprintf(tmpStr, "[%f,%f]", point.x, point.y);
  return tmpStr;
}

void seeta_v8::LoadImage(ImageData &imageData, const char *imagePath)
{
  vector<unsigned char> png;
  vector<unsigned char> grey;  // the raw pixels
  unsigned width = 0, height = 0;

  // load and decode
  lodepng::load_file(png, imagePath);
  lodepng::decode(grey, width, height, png, LCT_GREY, 8);

  imageData.data = grey.data();
  imageData.width = width;
  imageData.height = height;
  imageData.num_channels = 1;
}

NAN_MODULE_INIT(InitAll)
{
  Nan::Set(target, New<String>("detect").ToLocalChecked(), GetFunction(New<FunctionTemplate>(DetectAsync)).ToLocalChecked());
  Nan::Set(target, New<String>("align").ToLocalChecked(), GetFunction(New<FunctionTemplate>(AlignAsync)).ToLocalChecked());
}

NODE_MODULE(seeta, InitAll)
