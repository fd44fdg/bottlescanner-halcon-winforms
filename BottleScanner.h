#ifndef BOTTLE_SCANNER_H
#define BOTTLE_SCANNER_H

#include <string>

#include "halconcpp/HalconCpp.h"

class BottleScanner
{
public:
  struct InspectionResult
  {
    int count = -1;
    long long elapsed_ms = 0;
    int adaptive_s_low_threshold = -1;
    bool success = false;
    std::string error_message;
    HalconCpp::HObject image;
    HalconCpp::HObject regions;
  };

  BottleScanner();
  ~BottleScanner();

  BottleScanner(const BottleScanner&) = delete;
  BottleScanner& operator=(const BottleScanner&) = delete;

  void SetAdaptiveThresholdEnabled(bool enabled);
  void SetBaseSLowThreshold(int value);
  void SetHueRange(int low, int high);
  void SetVLowThreshold(int value);
  void SetPreFilterMinArea(double value);
  void SetMinArea(double value);
  void SetMinCircularity(double value);

  InspectionResult DoInspection(const std::string& imagePath);
  int DetectCaps(const std::string& imagePath);

private:
  void ClearState();
  int ComputeAdaptiveSLowThreshold();

  static constexpr double kMinArea = 800.0;
  static constexpr double kMinCircularity = 0.2;
  static constexpr int kDefaultBaseSLowThreshold = 80;
  static constexpr int kDefaultHueLow = 115;
  static constexpr int kDefaultHueHigh = 210;
  static constexpr int kDefaultVLowThreshold = 45;
  static constexpr double kDefaultPreFilterMinArea = 1200.0;

  bool adaptive_threshold_enabled_;
  int base_s_low_threshold_;
  int hue_low_;
  int hue_high_;
  int v_low_threshold_;
  double pre_filter_min_area_;
  double min_area_;
  double min_circularity_;
  int adaptive_s_low_threshold_;

  // Iconic objects
  HalconCpp::HObject ho_Image_;
  HalconCpp::HObject ho_ImageRed_;
  HalconCpp::HObject ho_ImageGreen_;
  HalconCpp::HObject ho_ImageBlue_;
  HalconCpp::HObject ho_ImageH_;
  HalconCpp::HObject ho_ImageS_;
  HalconCpp::HObject ho_ImageV_;
  HalconCpp::HObject ho_FullRegion_;
  HalconCpp::HObject ho_RegionH_;
  HalconCpp::HObject ho_RegionS_;
  HalconCpp::HObject ho_RegionV_;
  HalconCpp::HObject ho_BlueCandidates_;
  HalconCpp::HObject ho_ConnectedRegions_;
  HalconCpp::HObject ho_RegionFillUp_;
  HalconCpp::HObject ho_RegionOpening_;
  HalconCpp::HObject ho_SelectedRegions_;
  HalconCpp::HObject ho_RegionUnion_;
  HalconCpp::HObject ho_DistanceImage_;
  HalconCpp::HObject ho_DistanceImageUint2_;
  HalconCpp::HObject ho_DistanceImageInvert_;
  HalconCpp::HObject ho_Watersheds_;
  HalconCpp::HObject ho_FinalCaps_;
  HalconCpp::HObject ho_RealCaps_;

  // Control tuples
  HalconCpp::HTuple hv_Width_;
  HalconCpp::HTuple hv_Height_;
  HalconCpp::HTuple hv_MeanV_;
  HalconCpp::HTuple hv_DeviationV_;
  HalconCpp::HTuple hv_Number_;
};

#endif
