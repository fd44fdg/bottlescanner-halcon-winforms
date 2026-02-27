#ifndef VISION_ENGINE_H
#define VISION_ENGINE_H

#include <string>

#include "halconcpp/HalconCpp.h"

class VisionEngine
{
public:
  VisionEngine();
  ~VisionEngine();

  VisionEngine(const VisionEngine&) = delete;
  VisionEngine& operator=(const VisionEngine&) = delete;

  void SetThreshold(int value);
  int DetectCaps(std::string imagePath);

private:
  void ClearState();

  // Iconic objects
  HalconCpp::HObject ho_Images_;
  HalconCpp::HObject ho_ImageRed_;
  HalconCpp::HObject ho_ImageGreen_;
  HalconCpp::HObject ho_ImageBlue_;
  HalconCpp::HObject ho_ImageSub1_;
  HalconCpp::HObject ho_ImageSubFinal_;
  HalconCpp::HObject ho_Regions_;
  HalconCpp::HObject ho_RegionOpening_;
  HalconCpp::HObject ho_RegionFillUp_;
  HalconCpp::HObject ho_ConnectedRegions_;
  HalconCpp::HObject ho_SelectedRegions_;
  HalconCpp::HObject ho_RegionUnion_;
  HalconCpp::HObject ho_DistanceImage_;
  HalconCpp::HObject ho_DistanceImageUint2_;
  HalconCpp::HObject ho_DistanceImageInvert_;
  HalconCpp::HObject ho_Watersheds_;
  HalconCpp::HObject ho_FinalCaps_;

  // Control tuples
  HalconCpp::HTuple hv_Width_;
  HalconCpp::HTuple hv_Height_;
  HalconCpp::HTuple hv_Number_;

  int threshold_;
};

#endif
