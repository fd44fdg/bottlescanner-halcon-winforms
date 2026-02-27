#include "BottleScanner.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace HalconCpp;

BottleScanner::BottleScanner()
    : adaptive_threshold_enabled_(true),
      base_s_low_threshold_(kDefaultBaseSLowThreshold),
      hue_low_(kDefaultHueLow),
      hue_high_(kDefaultHueHigh),
      v_low_threshold_(kDefaultVLowThreshold),
      pre_filter_min_area_(kDefaultPreFilterMinArea),
      min_area_(kMinArea),
      min_circularity_(kMinCircularity),
      adaptive_s_low_threshold_(kDefaultBaseSLowThreshold) {}

BottleScanner::~BottleScanner()
{
  try
  {
    ClearState();
  }
  catch (...)
  {
    // Destructors must not throw.
  }
}

void BottleScanner::ClearState()
{
  if (ho_Image_.IsInitialized()) ho_Image_.Clear();
  if (ho_ImageRed_.IsInitialized()) ho_ImageRed_.Clear();
  if (ho_ImageGreen_.IsInitialized()) ho_ImageGreen_.Clear();
  if (ho_ImageBlue_.IsInitialized()) ho_ImageBlue_.Clear();
  if (ho_ImageH_.IsInitialized()) ho_ImageH_.Clear();
  if (ho_ImageS_.IsInitialized()) ho_ImageS_.Clear();
  if (ho_ImageV_.IsInitialized()) ho_ImageV_.Clear();
  if (ho_FullRegion_.IsInitialized()) ho_FullRegion_.Clear();
  if (ho_RegionH_.IsInitialized()) ho_RegionH_.Clear();
  if (ho_RegionS_.IsInitialized()) ho_RegionS_.Clear();
  if (ho_RegionV_.IsInitialized()) ho_RegionV_.Clear();
  if (ho_BlueCandidates_.IsInitialized()) ho_BlueCandidates_.Clear();
  if (ho_ConnectedRegions_.IsInitialized()) ho_ConnectedRegions_.Clear();
  if (ho_RegionFillUp_.IsInitialized()) ho_RegionFillUp_.Clear();
  if (ho_RegionOpening_.IsInitialized()) ho_RegionOpening_.Clear();
  if (ho_SelectedRegions_.IsInitialized()) ho_SelectedRegions_.Clear();
  if (ho_RegionUnion_.IsInitialized()) ho_RegionUnion_.Clear();
  if (ho_DistanceImage_.IsInitialized()) ho_DistanceImage_.Clear();
  if (ho_DistanceImageUint2_.IsInitialized()) ho_DistanceImageUint2_.Clear();
  if (ho_DistanceImageInvert_.IsInitialized()) ho_DistanceImageInvert_.Clear();
  if (ho_Watersheds_.IsInitialized()) ho_Watersheds_.Clear();
  if (ho_FinalCaps_.IsInitialized()) ho_FinalCaps_.Clear();
  if (ho_RealCaps_.IsInitialized()) ho_RealCaps_.Clear();

  hv_Width_.Clear();
  hv_Height_.Clear();
  hv_MeanV_.Clear();
  hv_DeviationV_.Clear();
  hv_Number_.Clear();
}

int BottleScanner::ComputeAdaptiveSLowThreshold()
{
  double mean_v = hv_MeanV_.D();
  double s_low = static_cast<double>(base_s_low_threshold_);

  if (adaptive_threshold_enabled_ && mean_v > 200.0)
  {
    s_low *= 1.10;
  }
  else if (adaptive_threshold_enabled_ && mean_v < 100.0)
  {
    s_low *= 0.95;
  }

  int threshold = static_cast<int>(std::lround(s_low));
  if (threshold < 0) threshold = 0;
  if (threshold > 255) threshold = 255;
  adaptive_s_low_threshold_ = threshold;
  return threshold;
}

void BottleScanner::SetAdaptiveThresholdEnabled(bool enabled)
{
  adaptive_threshold_enabled_ = enabled;
}

void BottleScanner::SetBaseSLowThreshold(int value)
{
  if (value < 0 || value > 255)
  {
    throw std::out_of_range("BottleScanner::SetBaseSLowThreshold expects 0..255");
  }
  base_s_low_threshold_ = value;
}

void BottleScanner::SetPreFilterMinArea(double value)
{
  if (value < 0.0)
  {
    throw std::out_of_range("BottleScanner::SetPreFilterMinArea expects >= 0");
  }
  pre_filter_min_area_ = value;
}

void BottleScanner::SetHueRange(int low, int high)
{
  if (low < 0 || high < 0 || low > 255 || high > 255 || low > high)
  {
    throw std::out_of_range("BottleScanner::SetHueRange expects 0<=low<=high<=255");
  }
  hue_low_ = low;
  hue_high_ = high;
}

void BottleScanner::SetVLowThreshold(int value)
{
  if (value < 0 || value > 255)
  {
    throw std::out_of_range("BottleScanner::SetVLowThreshold expects 0..255");
  }
  v_low_threshold_ = value;
}

void BottleScanner::SetMinArea(double value)
{
  if (value < 0.0)
  {
    throw std::out_of_range("BottleScanner::SetMinArea expects >= 0");
  }
  min_area_ = value;
}

void BottleScanner::SetMinCircularity(double value)
{
  if (value < 0.0 || value > 1.0)
  {
    throw std::out_of_range("BottleScanner::SetMinCircularity expects 0..1");
  }
  min_circularity_ = value;
}

BottleScanner::InspectionResult BottleScanner::DoInspection(const std::string& imagePath)
{
  InspectionResult result;
  const auto start = std::chrono::steady_clock::now();
  try
  {
    ClearState();

    ReadImage(&ho_Image_, imagePath.c_str());
    GetImageSize(ho_Image_, &hv_Width_, &hv_Height_);

    Decompose3(ho_Image_, &ho_ImageRed_, &ho_ImageGreen_, &ho_ImageBlue_);
    TransFromRgb(ho_ImageRed_,
                 ho_ImageGreen_,
                 ho_ImageBlue_,
                 &ho_ImageH_,
                 &ho_ImageS_,
                 &ho_ImageV_,
                 "hsv");

    GenRectangle1(&ho_FullRegion_, 0, 0, hv_Height_ - 1, hv_Width_ - 1);
    Intensity(ho_FullRegion_, ho_ImageV_, &hv_MeanV_, &hv_DeviationV_);
    const int s_low_threshold = ComputeAdaptiveSLowThreshold();

    Threshold(ho_ImageH_, &ho_RegionH_, hue_low_, hue_high_);
    Threshold(ho_ImageS_, &ho_RegionS_, s_low_threshold, 255);
    Threshold(ho_ImageV_, &ho_RegionV_, v_low_threshold_, 255);
    Intersection(ho_RegionH_, ho_RegionS_, &ho_BlueCandidates_);
    Intersection(ho_BlueCandidates_, ho_RegionV_, &ho_BlueCandidates_);
    HTuple hv_CountBlueCandidates;
    CountObj(ho_BlueCandidates_, &hv_CountBlueCandidates);

    Connection(ho_BlueCandidates_, &ho_ConnectedRegions_);
    HTuple hv_CountConnected;
    CountObj(ho_ConnectedRegions_, &hv_CountConnected);
    FillUp(ho_ConnectedRegions_, &ho_RegionFillUp_);
    OpeningCircle(ho_RegionFillUp_, &ho_RegionOpening_, 3.5);
    SelectShape(ho_RegionOpening_,
                &ho_SelectedRegions_,
                "area",
                "and",
                pre_filter_min_area_,
                99999999);
    HTuple hv_CountSelectedByArea;
    CountObj(ho_SelectedRegions_, &hv_CountSelectedByArea);

    Union1(ho_SelectedRegions_, &ho_RegionUnion_);
    DistanceTransform(ho_RegionUnion_,
                      &ho_DistanceImage_,
                      "euclidean",
                      "true",
                      hv_Width_,
                      hv_Height_);
    ConvertImageType(ho_DistanceImage_, &ho_DistanceImageUint2_, "uint2");
    InvertImage(ho_DistanceImageUint2_, &ho_DistanceImageInvert_);
    HTuple hv_CountFinalCaps;
    int used_watershed_threshold = 40;
    const long long target_count = hv_CountSelectedByArea.I();
    long long best_count = -1;
    long long best_score = (std::numeric_limits<long long>::max)();

    for (const int watershed_threshold : std::array<int, 4>{40, 20, 10, 5})
    {
      HObject ho_WatershedsCandidate;
      HObject ho_FinalCapsCandidate;
      HObject ho_RealCapsCandidate;
      HTuple hv_NumberCandidate;
      HTuple hv_CountFinalCapsCandidate;

      WatershedsThreshold(ho_DistanceImageInvert_, &ho_WatershedsCandidate, watershed_threshold);
      Intersection(ho_WatershedsCandidate, ho_RegionUnion_, &ho_FinalCapsCandidate);
      CountObj(ho_FinalCapsCandidate, &hv_CountFinalCapsCandidate);

      SelectShape(ho_FinalCapsCandidate,
                  &ho_RealCapsCandidate,
                  (HTuple("area").Append("circularity")),
                  "and",
                  (HTuple(min_area_).Append(min_circularity_)),
                  (HTuple(99999999).Append(1.0)));
      CountObj(ho_RealCapsCandidate, &hv_NumberCandidate);

      const long long candidate_count = hv_NumberCandidate.I();
      const long long score = std::llabs(candidate_count - target_count);

      bool should_pick = false;
      if (score < best_score)
      {
        should_pick = true;
      }
      else if (score == best_score)
      {
        // Tie-breaker: avoid over-segmentation if possible.
        if (best_count > target_count && candidate_count <= target_count)
        {
          should_pick = true;
        }
        else if ((candidate_count <= target_count) == (best_count <= target_count) &&
                 candidate_count > best_count)
        {
          should_pick = true;
        }
      }

      if (should_pick)
      {
        best_score = score;
        best_count = candidate_count;
        used_watershed_threshold = watershed_threshold;
        ho_Watersheds_ = ho_WatershedsCandidate;
        ho_FinalCaps_ = ho_FinalCapsCandidate;
        ho_RealCaps_ = ho_RealCapsCandidate;
        hv_CountFinalCaps = hv_CountFinalCapsCandidate;
        hv_Number_ = hv_NumberCandidate;
      }
    }

    // Build a clean display object tuple from the final candidates.
    // This removes possible empty slots and enforces region-domain consistency.
    HObject ho_DisplayCaps;
    GenEmptyObj(&ho_DisplayCaps);
    HTuple hv_RawRealCapsCount;
    CountObj(ho_RealCaps_, &hv_RawRealCapsCount);
    for (Hlong idx = 1; idx <= hv_RawRealCapsCount.I(); ++idx)
    {
      HObject ho_OneCap;
      HObject ho_OneCapInDomain;
      HTuple hv_OneArea;
      HTuple hv_OneRow;
      HTuple hv_OneCol;

      SelectObj(ho_RealCaps_, &ho_OneCap, idx);
      Intersection(ho_OneCap, ho_RegionUnion_, &ho_OneCapInDomain);
      AreaCenter(ho_OneCapInDomain, &hv_OneArea, &hv_OneRow, &hv_OneCol);
      if (hv_OneArea.Length() > 0 && hv_OneArea[0].D() > 0.0)
      {
        ConcatObj(ho_DisplayCaps, ho_OneCapInDomain, &ho_DisplayCaps);
      }
    }
    ho_RealCaps_ = ho_DisplayCaps;
    CountObj(ho_RealCaps_, &hv_Number_);

    HTuple hv_Areas;
    HTuple hv_Rows;
    HTuple hv_Cols;
    HTuple hv_Circularities;
    AreaCenter(ho_RealCaps_, &hv_Areas, &hv_Rows, &hv_Cols);
    Circularity(ho_RealCaps_, &hv_Circularities);

    const Hlong total_caps = hv_Areas.Length();
    Hlong fail_area = 0;
    Hlong fail_circularity = 0;
    Hlong pass_both = 0;
    double min_area_value = 1e18;
    double min_circularity_value = 1e18;

    for (Hlong i = 0; i < total_caps; ++i)
    {
      const double area = hv_Areas[i].D();
      const double circularity = hv_Circularities[i].D();
      const bool pass_area = area >= min_area_;
      const bool pass_circularity = circularity >= min_circularity_;
      if (!pass_area) ++fail_area;
      if (!pass_circularity) ++fail_circularity;
      if (pass_area && pass_circularity) ++pass_both;
      if (area < min_area_value) min_area_value = area;
      if (circularity < min_circularity_value) min_circularity_value = circularity;
    }

    const auto end = std::chrono::steady_clock::now();
    const auto elapsed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "[BottleScanner] mean(V)=" << hv_MeanV_.D()
              << ", adaptiveEnabled=" << (adaptive_threshold_enabled_ ? "true" : "false")
              << ", H=[" << hue_low_ << "," << hue_high_ << "]"
              << ", base S low=" << base_s_low_threshold_
              << ", adaptive S low=" << adaptive_s_low_threshold_
              << ", V low=" << v_low_threshold_
              << ", preMinArea=" << pre_filter_min_area_
              << ", minArea=" << min_area_
              << ", minCircularity=" << min_circularity_
              << ", blueCandidates=" << hv_CountBlueCandidates.I()
              << ", connected=" << hv_CountConnected.I()
              << ", selectedByArea=" << hv_CountSelectedByArea.I()
              << ", watershedTarget=" << target_count
              << ", watershedCaps=" << hv_CountFinalCaps.I()
              << ", watershedTh=" << used_watershed_threshold
              << ", finalTotal=" << total_caps
              << ", passBoth=" << pass_both
              << ", failArea=" << fail_area
              << ", failCircularity=" << fail_circularity
              << ", minAreaSeen=" << min_area_value
              << ", minCircularitySeen=" << min_circularity_value
              << ", elapsedMs=" << elapsed_ms << std::endl;

    result.count = static_cast<int>(hv_Number_.I());
    result.elapsed_ms = elapsed_ms;
    result.adaptive_s_low_threshold = adaptive_s_low_threshold_;
    result.success = true;
    result.image = ho_Image_;
    result.regions = ho_RealCaps_;
    return result;
  }
  catch (const HException& e)
  {
    const auto end = std::chrono::steady_clock::now();
    const auto elapsed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cerr << "[BottleScanner] HALCON exception\n"
              << "  imagePath   : " << imagePath << "\n"
              << "  errorCode   : " << e.ErrorCode() << "\n"
              << "  procedure   : " << e.ProcName().Text() << "\n"
              << "  message     : " << e.ErrorMessage().Text() << "\n"
              << "  elapsedMs   : " << elapsed_ms << std::endl;
    result.elapsed_ms = elapsed_ms;
    result.error_message = e.ErrorMessage().Text();
    ClearState();
    return result;
  }
  catch (const std::exception& e)
  {
    const auto end = std::chrono::steady_clock::now();
    const auto elapsed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cerr << "[BottleScanner] std::exception\n"
              << "  imagePath   : " << imagePath << "\n"
              << "  message     : " << e.what() << "\n"
              << "  elapsedMs   : " << elapsed_ms << std::endl;
    result.elapsed_ms = elapsed_ms;
    result.error_message = e.what();
    ClearState();
    return result;
  }
  catch (...)
  {
    const auto end = std::chrono::steady_clock::now();
    const auto elapsed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cerr << "[BottleScanner] Unknown exception\n"
              << "  imagePath   : " << imagePath << "\n"
              << "  elapsedMs   : " << elapsed_ms << std::endl;
    result.elapsed_ms = elapsed_ms;
    result.error_message = "Unknown exception";
    ClearState();
    return result;
  }
}

int BottleScanner::DetectCaps(const std::string& imagePath)
{
  const InspectionResult result = DoInspection(imagePath);
  return result.success ? result.count : -1;
}
