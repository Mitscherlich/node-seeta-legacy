#ifndef SEETA_V8_H_
#define SEETA_V8_H_

#include <nan.h>

#include "common.h"


namespace seeta_v8 {

const char *ParseFaceInfo(seeta::FaceInfo &face);

void LoadImage(seeta::ImageData &imageData, const char *imagePath);

}

#endif // SEETA_V8_H_
