#include "VisionEngine.h"

#include <iostream>
#include <stdexcept>

using namespace HalconCpp;

VisionEngine::VisionEngine() : threshold_(33) {}

VisionEngine::~VisionEngine()
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

void VisionEngine::ClearState()
{
  if (ho_Images_.IsInitialized()) ho_Images_.Clear();
  if (ho_ImageRed_.IsInitialized()) ho_ImageRed_.Clear();
  if (ho_ImageGreen_.IsInitialized()) ho_ImageGreen_.Clear();
  if (ho_ImageBlue_.IsInitialized()) ho_ImageBlue_.Clear();
  if (ho_ImageSub1_.IsInitialized()) ho_ImageSub1_.Clear();
  if (ho_ImageSubFinal_.IsInitialized()) ho_ImageSubFinal_.Clear();
  if (ho_Regions_.IsInitialized()) ho_Regions_.Clear();
  if (ho_RegionOpening_.IsInitialized()) ho_RegionOpening_.Clear();
  if (ho_RegionFillUp_.IsInitialized()) ho_RegionFillUp_.Clear();
  if (ho_ConnectedRegions_.IsInitialized()) ho_ConnectedRegions_.Clear();
  if (ho_SelectedRegions_.IsInitialized()) ho_SelectedRegions_.Clear();
  if (ho_RegionUnion_.IsInitialized()) ho_RegionUnion_.Clear();
  if (ho_DistanceImage_.IsInitialized()) ho_DistanceImage_.Clear();
  if (ho_DistanceImageUint2_.IsInitialized()) ho_DistanceImageUint2_.Clear();
  if (ho_DistanceImageInvert_.IsInitialized()) ho_DistanceImageInvert_.Clear();
  if (ho_Watersheds_.IsInitialized()) ho_Watersheds_.Clear();
  if (ho_FinalCaps_.IsInitialized()) ho_FinalCaps_.Clear();

  hv_Width_.Clear();
  hv_Height_.Clear();
  hv_Number_.Clear();
}

void VisionEngine::SetThreshold(int value)
{
  if (value < 0 || value > 255)
  {
    throw std::out_of_range("VisionEngine::SetThreshold expects 0..255");
  }
  threshold_ = value;
}

int VisionEngine::DetectCaps(std::string imagePath)
{
  try
  {
    // Ensure repeated calls do not keep previous iconic/control handles alive.
    ClearState();

    ReadImage(&ho_Images_, imagePath.c_str());
    GetImageSize(ho_Images_, &hv_Width_, &hv_Height_);

    Decompose3(ho_Images_, &ho_ImageRed_, &ho_ImageGreen_, &ho_ImageBlue_);

    SubImage(ho_ImageBlue_, ho_ImageRed_, &ho_ImageSub1_, 1, 0);
    SubImage(ho_ImageSub1_, ho_ImageGreen_, &ho_ImageSubFinal_, 1, 0);

    Threshold(ho_ImageSubFinal_, &ho_Regions_, threshold_, 255);
    Connection(ho_Regions_, &ho_ConnectedRegions_);
    OpeningCircle(ho_Regions_, &ho_RegionOpening_, 3.5);
    FillUp(ho_RegionOpening_, &ho_RegionFillUp_);
    Connection(ho_RegionFillUp_, &ho_ConnectedRegions_);

    SelectShape(ho_ConnectedRegions_,
                &ho_SelectedRegions_,
                (HTuple("area").Append("circularity")),
                "and",
                (HTuple(100).Append(0.2)),
                (HTuple(99999999).Append(1.0)));

    Union1(ho_SelectedRegions_, &ho_RegionUnion_);

    DistanceTransform(ho_RegionUnion_,
                      &ho_DistanceImage_,
                      "euclidean",
                      "true",
                      hv_Width_,
                      hv_Height_);

    ConvertImageType(ho_DistanceImage_, &ho_DistanceImageUint2_, "uint2");
    InvertImage(ho_DistanceImageUint2_, &ho_DistanceImageInvert_);
    WatershedsThreshold(ho_DistanceImageInvert_, &ho_Watersheds_, 100);
    Intersection(ho_Watersheds_, ho_RegionUnion_, &ho_FinalCaps_);

    CountObj(ho_FinalCaps_, &hv_Number_);

    return static_cast<int>(hv_Number_.I());
  }
  catch (const HalconCpp::HException& e)
  {
    std::cerr << "HALCON exception in VisionEngine::DetectCaps\n"
              << "  imagePath   : " << imagePath << "\n"
              << "  errorCode   : " << e.ErrorCode() << "\n"
              << "  procedure   : " << e.ProcName().Text() << "\n"
              << "  message     : " << e.ErrorMessage().Text() << std::endl;
    ClearState();
    return -1;
  }
  catch (const std::exception& e)
  {
    std::cerr << "std::exception in VisionEngine::DetectCaps\n"
              << "  imagePath   : " << imagePath << "\n"
              << "  message     : " << e.what() << std::endl;
    ClearState();
    return -1;
  }
  catch (...)
  {
    std::cerr << "Unknown exception in VisionEngine::DetectCaps\n"
              << "  imagePath   : " << imagePath << std::endl;
    ClearState();
    return -1;
  }
}
