#pragma once

#include <cstdint>
#include <msclr/marshal_cppstd.h>
#include <stdexcept>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

#include "../BottleScanner.h"

namespace BottleScannerWinFormsCli
{
using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

struct NativeRuntime
{
  BottleScanner scanner;
  HalconCpp::HTuple window_handle;
  bool window_opened = false;
};

public ref class MainForm : public Form
{
public:
  MainForm()
  {
    native_ = new NativeRuntime();
    // Keep GUI behavior aligned with the tuned console profile.
    native_->scanner.SetHueRange(115, 210);
    native_->scanner.SetBaseSLowThreshold(80);
    native_->scanner.SetVLowThreshold(45);
    native_->scanner.SetAdaptiveThresholdEnabled(false);
    native_->scanner.SetPreFilterMinArea(1200);
    native_->scanner.SetMinArea(650);
    native_->scanner.SetMinCircularity(0.20);
    BuildUi();
  }

  ~MainForm()
  {
    this->!MainForm();
  }

protected:
  !MainForm()
  {
    if (native_ != nullptr)
    {
      try
      {
        if (native_->window_opened)
        {
          HalconCpp::CloseWindow(native_->window_handle);
          native_->window_opened = false;
        }
      }
      catch (...)
      {
      }
      delete native_;
      native_ = nullptr;
    }
  }

private:
  NativeRuntime* native_;

  SplitContainer^ split_root_;
  Panel^ panel_halcon_;
  Panel^ right_panel_;
  Label^ label_path_;
  TextBox^ textbox_path_;
  Button^ button_browse_;
  Button^ button_detect_;
  Label^ label_result_;
  Label^ label_param_title_;
  Label^ label_h_low_;
  Label^ label_h_high_;
  Label^ label_s_low_;
  Label^ label_v_low_;
  Label^ label_min_area_;
  Label^ label_min_circularity_;
  TrackBar^ track_h_low_;
  TrackBar^ track_h_high_;
  TrackBar^ track_s_low_;
  TrackBar^ track_v_low_;
  TrackBar^ track_min_area_;
  TrackBar^ track_min_circularity_;
  Button^ button_open_report_dir_;
  OpenFileDialog^ open_file_dialog_;

  void BuildUi()
  {
    Text = "BottleScanner";
    Width = 1320;
    Height = 860;
    MinimumSize = Drawing::Size(1100, 700);
    StartPosition = FormStartPosition::CenterScreen;
    BackColor = Color::FromArgb(242, 246, 252);
    Font = gcnew Drawing::Font("Microsoft YaHei UI", 9.5f, FontStyle::Regular, GraphicsUnit::Point);

    split_root_ = gcnew SplitContainer();
    split_root_->Dock = DockStyle::Fill;
    split_root_->SplitterWidth = 8;
    split_root_->Panel2Collapsed = false;
    split_root_->FixedPanel = FixedPanel::Panel2;
    split_root_->IsSplitterFixed = false;
    Controls->Add(split_root_);

    panel_halcon_ = gcnew Panel();
    panel_halcon_->Dock = DockStyle::Fill;
    panel_halcon_->BackColor = Color::Black;
    panel_halcon_->Padding = System::Windows::Forms::Padding(2);
    split_root_->Panel1->Controls->Add(panel_halcon_);

    right_panel_ = gcnew Panel();
    right_panel_->Dock = DockStyle::Fill;
    right_panel_->AutoScroll = true;
    right_panel_->BackColor = Color::FromArgb(251, 253, 255);
    split_root_->Panel2->Controls->Add(right_panel_);

    label_path_ = gcnew Label();
    label_path_->Text = "图片路径";
    label_path_->Location = Point(16, 20);
    label_path_->AutoSize = true;
    label_path_->ForeColor = Color::FromArgb(52, 64, 84);
    right_panel_->Controls->Add(label_path_);

    textbox_path_ = gcnew TextBox();
    textbox_path_->Location = Point(16, 46);
    textbox_path_->Width = 316;
    textbox_path_->BorderStyle = BorderStyle::FixedSingle;
    textbox_path_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                       AnchorStyles::Right);
    right_panel_->Controls->Add(textbox_path_);

    button_browse_ = gcnew Button();
    button_browse_->Text = "浏览";
    button_browse_->Location = Point(16, 82);
    button_browse_->Width = 100;
    button_browse_->FlatStyle = FlatStyle::Flat;
    button_browse_->BackColor = Color::White;
    button_browse_->ForeColor = Color::FromArgb(31, 41, 55);
    button_browse_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left);
    button_browse_->Click += gcnew EventHandler(this, &MainForm::OnBrowseClicked);
    right_panel_->Controls->Add(button_browse_);

    button_detect_ = gcnew Button();
    button_detect_->Text = "开始检测";
    button_detect_->Location = Point(124, 82);
    button_detect_->Width = 208;
    button_detect_->FlatStyle = FlatStyle::Flat;
    button_detect_->BackColor = Color::FromArgb(22, 119, 255);
    button_detect_->ForeColor = Color::White;
    button_detect_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                        AnchorStyles::Right);
    button_detect_->Click += gcnew EventHandler(this, &MainForm::OnDetectClicked);
    right_panel_->Controls->Add(button_detect_);

    label_result_ = gcnew Label();
    label_result_->Text = "结果：待检测";
    label_result_->Location = Point(16, 132);
    label_result_->Width = 316;
    label_result_->Height = 220;
    label_result_->BackColor = Color::White;
    label_result_->BorderStyle = BorderStyle::FixedSingle;
    label_result_->Padding = System::Windows::Forms::Padding(10);
    label_result_->AutoSize = false;
    label_result_->TextAlign = ContentAlignment::TopLeft;
    label_result_->ForeColor = Color::FromArgb(17, 24, 39);
    label_result_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                       AnchorStyles::Right);
    right_panel_->Controls->Add(label_result_);

    label_param_title_ = gcnew Label();
    label_param_title_->Text = "参数调节（实时生效）";
    label_param_title_->Location = Point(16, 368);
    label_param_title_->AutoSize = false;
    label_param_title_->Width = 316;
    label_param_title_->Height = 24;
    label_param_title_->ForeColor = Color::FromArgb(52, 64, 84);
    label_param_title_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                            AnchorStyles::Right);
    right_panel_->Controls->Add(label_param_title_);

    label_h_low_ = gcnew Label();
    label_h_low_->Location = Point(16, 396);
    label_h_low_->Width = 316;
    label_h_low_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                      AnchorStyles::Right);
    right_panel_->Controls->Add(label_h_low_);

    track_h_low_ = gcnew TrackBar();
    track_h_low_->Location = Point(16, 416);
    track_h_low_->Width = 316;
    track_h_low_->Minimum = 0;
    track_h_low_->Maximum = 255;
    track_h_low_->TickFrequency = 10;
    track_h_low_->SmallChange = 1;
    track_h_low_->LargeChange = 5;
    track_h_low_->Value = 115;
    track_h_low_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                      AnchorStyles::Right);
    track_h_low_->Scroll += gcnew EventHandler(this, &MainForm::OnParamScroll);
    right_panel_->Controls->Add(track_h_low_);

    label_h_high_ = gcnew Label();
    label_h_high_->Location = Point(16, 462);
    label_h_high_->Width = 316;
    label_h_high_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                       AnchorStyles::Right);
    right_panel_->Controls->Add(label_h_high_);

    track_h_high_ = gcnew TrackBar();
    track_h_high_->Location = Point(16, 482);
    track_h_high_->Width = 316;
    track_h_high_->Minimum = 0;
    track_h_high_->Maximum = 255;
    track_h_high_->TickFrequency = 10;
    track_h_high_->SmallChange = 1;
    track_h_high_->LargeChange = 5;
    track_h_high_->Value = 210;
    track_h_high_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                       AnchorStyles::Right);
    track_h_high_->Scroll += gcnew EventHandler(this, &MainForm::OnParamScroll);
    right_panel_->Controls->Add(track_h_high_);

    label_s_low_ = gcnew Label();
    label_s_low_->Location = Point(16, 528);
    label_s_low_->Width = 316;
    label_s_low_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                      AnchorStyles::Right);
    right_panel_->Controls->Add(label_s_low_);

    track_s_low_ = gcnew TrackBar();
    track_s_low_->Location = Point(16, 548);
    track_s_low_->Width = 316;
    track_s_low_->Minimum = 0;
    track_s_low_->Maximum = 255;
    track_s_low_->TickFrequency = 10;
    track_s_low_->SmallChange = 1;
    track_s_low_->LargeChange = 5;
    track_s_low_->Value = 80;
    track_s_low_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                      AnchorStyles::Right);
    track_s_low_->Scroll += gcnew EventHandler(this, &MainForm::OnParamScroll);
    right_panel_->Controls->Add(track_s_low_);

    label_v_low_ = gcnew Label();
    label_v_low_->Location = Point(16, 594);
    label_v_low_->Width = 316;
    label_v_low_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                      AnchorStyles::Right);
    right_panel_->Controls->Add(label_v_low_);

    track_v_low_ = gcnew TrackBar();
    track_v_low_->Location = Point(16, 614);
    track_v_low_->Width = 316;
    track_v_low_->Minimum = 0;
    track_v_low_->Maximum = 255;
    track_v_low_->TickFrequency = 10;
    track_v_low_->SmallChange = 1;
    track_v_low_->LargeChange = 5;
    track_v_low_->Value = 45;
    track_v_low_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                      AnchorStyles::Right);
    track_v_low_->Scroll += gcnew EventHandler(this, &MainForm::OnParamScroll);
    right_panel_->Controls->Add(track_v_low_);

    label_min_area_ = gcnew Label();
    label_min_area_->Location = Point(16, 660);
    label_min_area_->Width = 316;
    label_min_area_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                         AnchorStyles::Right);
    right_panel_->Controls->Add(label_min_area_);

    track_min_area_ = gcnew TrackBar();
    track_min_area_->Location = Point(16, 680);
    track_min_area_->Width = 316;
    track_min_area_->Minimum = 0;
    track_min_area_->Maximum = 2000;
    track_min_area_->TickFrequency = 100;
    track_min_area_->SmallChange = 10;
    track_min_area_->LargeChange = 50;
    track_min_area_->Value = 650;
    track_min_area_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Left |
                                                         AnchorStyles::Right);
    track_min_area_->Scroll += gcnew EventHandler(this, &MainForm::OnParamScroll);
    right_panel_->Controls->Add(track_min_area_);

    label_min_circularity_ = gcnew Label();
    label_min_circularity_->Location = Point(16, 726);
    label_min_circularity_->Width = 316;
    label_min_circularity_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top |
                                                                AnchorStyles::Left |
                                                                AnchorStyles::Right);
    right_panel_->Controls->Add(label_min_circularity_);

    track_min_circularity_ = gcnew TrackBar();
    track_min_circularity_->Location = Point(16, 746);
    track_min_circularity_->Width = 316;
    track_min_circularity_->Minimum = 0;
    track_min_circularity_->Maximum = 100;
    track_min_circularity_->TickFrequency = 5;
    track_min_circularity_->SmallChange = 1;
    track_min_circularity_->LargeChange = 5;
    track_min_circularity_->Value = 20;
    track_min_circularity_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top |
                                                                AnchorStyles::Left |
                                                                AnchorStyles::Right);
    track_min_circularity_->Scroll += gcnew EventHandler(this, &MainForm::OnParamScroll);
    right_panel_->Controls->Add(track_min_circularity_);

    button_open_report_dir_ = gcnew Button();
    button_open_report_dir_->Text = "打开报表目录";
    button_open_report_dir_->Location = Point(16, 806);
    button_open_report_dir_->Width = 316;
    button_open_report_dir_->Height = 34;
    button_open_report_dir_->FlatStyle = FlatStyle::Flat;
    button_open_report_dir_->BackColor = Color::White;
    button_open_report_dir_->ForeColor = Color::FromArgb(31, 41, 55);
    button_open_report_dir_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top |
                                                                 AnchorStyles::Left |
                                                                 AnchorStyles::Right);
    button_open_report_dir_->Click += gcnew EventHandler(this, &MainForm::OnOpenReportDirClicked);
    right_panel_->Controls->Add(button_open_report_dir_);

    open_file_dialog_ = gcnew OpenFileDialog();
    open_file_dialog_->Filter =
        "Image Files|*.png;*.jpg;*.jpeg;*.bmp;*.tif;*.tiff|All Files|*.*";

    Load += gcnew EventHandler(this, &MainForm::OnFormLoad);
    Shown += gcnew EventHandler(this, &MainForm::OnFormShown);
    SizeChanged += gcnew EventHandler(this, &MainForm::OnFormSizeChanged);
    panel_halcon_->Resize += gcnew EventHandler(this, &MainForm::OnHalconPanelResize);
    right_panel_->Resize += gcnew EventHandler(this, &MainForm::OnRightPanelResize);
    LayoutRightPanelControls();
    UpdateParamLabels();
  }

  void OnFormLoad(Object^, EventArgs^)
  {
    panel_halcon_->CreateControl();
    EnsureHalconWindow();
  }

  void OnFormShown(Object^, EventArgs^)
  {
    AdjustLayoutForCurrentSize();
    EnsureHalconWindow();
    if (native_->window_opened)
    {
      HalconCpp::SetWindowParam(native_->window_handle, "background_color", "black");
      HalconCpp::ClearWindow(native_->window_handle);
      HalconCpp::SetColor(native_->window_handle, "white");
      HalconCpp::SetTposition(native_->window_handle, 20, 20);
      HalconCpp::WriteString(native_->window_handle, "请选择图片后点击 开始检测");
    }
  }

  void OnFormSizeChanged(Object^, EventArgs^)
  {
    AdjustLayoutForCurrentSize();
    LayoutRightPanelControls();
  }

  void OnBrowseClicked(Object^, EventArgs^)
  {
    if (open_file_dialog_->ShowDialog() == System::Windows::Forms::DialogResult::OK)
    {
      textbox_path_->Text = open_file_dialog_->FileName;
      label_result_->Text = "结果：已选择图片，点击开始检测";
    }
  }

  void OnDetectClicked(Object^, EventArgs^)
  {
    label_result_->Text = "结果：正在检测...";
    label_result_->Refresh();
    Application::DoEvents();

    String^ path_managed = textbox_path_->Text->Trim();
    if (String::IsNullOrWhiteSpace(path_managed))
    {
      MessageBox::Show("请选择图片路径。", "提示", MessageBoxButtons::OK, MessageBoxIcon::Warning);
      return;
    }
    if (!System::IO::File::Exists(path_managed))
    {
      MessageBox::Show("图片路径不存在。", "提示", MessageBoxButtons::OK, MessageBoxIcon::Warning);
      return;
    }

    try
    {
      SyncScannerParamsFromUi();
      EnsureHalconWindow();
      if (!native_->window_opened)
      {
        label_result_->Text = "结果：HALCON 窗口未初始化";
        MessageBox::Show("HALCON 显示窗口未初始化成功。", "错误", MessageBoxButtons::OK, MessageBoxIcon::Error);
        return;
      }

      const std::string image_path = msclr::interop::marshal_as<std::string>(path_managed);
      const BottleScanner::InspectionResult result = native_->scanner.DoInspection(image_path);
      if (!result.success)
      {
        label_result_->Text = String::Format("结果：检测失败\n耗时：{0} ms\n错误：{1}",
                                             result.elapsed_ms,
                                             gcnew String(result.error_message.c_str()));
        return;
      }

      HalconCpp::HObject display_obj;
      HalconCpp::GenEmptyObj(&display_obj);
      HalconCpp::HTuple hv_display_raw_count;
      HalconCpp::CountObj(result.regions, &hv_display_raw_count);
      for (int i = 1; i <= static_cast<int>(hv_display_raw_count.I()); ++i)
      {
        HalconCpp::HObject one_region;
        HalconCpp::HTuple area;
        HalconCpp::HTuple row;
        HalconCpp::HTuple col;
        HalconCpp::SelectObj(result.regions, &one_region, i);
        HalconCpp::AreaCenter(one_region, &area, &row, &col);
        if (area.Length() > 0 && area[0].D() > 0.0)
        {
          HalconCpp::ConcatObj(display_obj, one_region, &display_obj);
        }
      }

      HalconCpp::HTuple final_count;
      HalconCpp::CountObj(display_obj, &final_count);

      HalconCpp::HTuple hv_w;
      HalconCpp::HTuple hv_h;
      HalconCpp::GetImageSize(result.image, &hv_w, &hv_h);
      HalconCpp::SetPart(native_->window_handle, 0, 0, hv_h - 1, hv_w - 1);
      HalconCpp::ClearWindow(native_->window_handle);
      HalconCpp::DispObj(result.image, native_->window_handle);
      HalconCpp::SetColor(native_->window_handle, "green");
      HalconCpp::SetLineWidth(native_->window_handle, 2);
      HalconCpp::SetDraw(native_->window_handle, "margin");
      HalconCpp::DispObj(display_obj, native_->window_handle);

      label_result_->Text =
          String::Format("结果：识别到 {0} 个蓝色瓶盖\n耗时：{1} ms\nH:[{2},{3}] S>= {4} V>= {5}\nArea>= {6} Circ>= {7:F2}",
                         static_cast<int>(final_count.I()),
                         result.elapsed_ms,
                         track_h_low_->Value,
                         track_h_high_->Value,
                         track_s_low_->Value,
                         track_v_low_->Value,
                         track_min_area_->Value,
                         track_min_circularity_->Value / 100.0);

      AppendDetectionLog(static_cast<int>(final_count.I()),
                         result.elapsed_ms,
                         result.adaptive_s_low_threshold);
    }
    catch (System::Exception^ ex)
    {
      label_result_->Text = "结果：.NET 异常";
      MessageBox::Show(ex->Message, ".NET 异常", MessageBoxButtons::OK, MessageBoxIcon::Error);
    }
    catch (const HalconCpp::HException& ex)
    {
      label_result_->Text = "结果：HALCON 异常";
      MessageBox::Show(gcnew String(ex.ErrorMessage().Text()),
                       "HALCON 异常",
                       MessageBoxButtons::OK,
                       MessageBoxIcon::Error);
    }
    catch (const std::exception& ex)
    {
      label_result_->Text = "结果：标准异常";
      MessageBox::Show(gcnew String(ex.what()),
                       "标准异常",
                       MessageBoxButtons::OK,
                       MessageBoxIcon::Error);
    }
    catch (...)
    {
      MessageBox::Show("未知异常", "错误", MessageBoxButtons::OK, MessageBoxIcon::Error);
    }
  }

  void EnsureHalconWindow()
  {
    if (native_->window_opened) return;
    if (!panel_halcon_->IsHandleCreated)
    {
      panel_halcon_->CreateControl();
      if (!panel_halcon_->IsHandleCreated)
      {
        auto unused = panel_halcon_->Handle;
        (void)unused;
      }
    }
    if (!panel_halcon_->IsHandleCreated) return;

    const int width = Math::Max(1, panel_halcon_->ClientSize.Width);
    const int height = Math::Max(1, panel_halcon_->ClientSize.Height);
    const auto father_value =
        static_cast<long long>(reinterpret_cast<std::intptr_t>(panel_halcon_->Handle.ToPointer()));
    HalconCpp::HTuple father(father_value);

    HalconCpp::OpenWindow(0,
                          0,
                          width,
                          height,
                          father,
                          "visible",
                          "",
                          &native_->window_handle);
    native_->window_opened = true;
    HalconCpp::SetWindowParam(native_->window_handle, "background_color", "black");
    HalconCpp::ClearWindow(native_->window_handle);
  }

  void OnHalconPanelResize(Object^, EventArgs^)
  {
    if (!native_->window_opened) return;
    const int width = Math::Max(1, panel_halcon_->ClientSize.Width);
    const int height = Math::Max(1, panel_halcon_->ClientSize.Height);
    HalconCpp::SetWindowExtents(native_->window_handle, 0, 0, width, height);
  }

  void OnRightPanelResize(Object^, EventArgs^)
  {
    LayoutRightPanelControls();
  }

  void AdjustLayoutForCurrentSize()
  {
    if (split_root_ == nullptr) return;
    if (split_root_->ClientSize.Width <= 0) return;

    split_root_->Panel2Collapsed = false;
    const int total_width = split_root_->ClientSize.Width;
    const int splitter_width = Math::Max(1, split_root_->SplitterWidth);
    const int right_width = 420;

    int panel1_min = 500;
    int panel2_min = 400;
    if (total_width < panel1_min + panel2_min + splitter_width)
    {
      panel1_min = Math::Max(120, (total_width - splitter_width) / 2);
      panel2_min = Math::Max(120, total_width - splitter_width - panel1_min);
    }

    split_root_->Panel1MinSize = panel1_min;
    split_root_->Panel2MinSize = panel2_min;

    const int min_distance = split_root_->Panel1MinSize;
    const int max_distance =
        Math::Max(min_distance, total_width - split_root_->Panel2MinSize - splitter_width);
    const int preferred_distance = total_width - right_width - splitter_width;
    split_root_->SplitterDistance =
        Math::Min(max_distance, Math::Max(min_distance, preferred_distance));
  }

  void LayoutRightPanelControls()
  {
    if (right_panel_ == nullptr) return;
    if (textbox_path_ == nullptr || button_browse_ == nullptr || button_detect_ == nullptr ||
        label_result_ == nullptr || label_h_low_ == nullptr || track_h_low_ == nullptr ||
        label_h_high_ == nullptr || track_h_high_ == nullptr || label_s_low_ == nullptr ||
        track_s_low_ == nullptr || label_v_low_ == nullptr || track_v_low_ == nullptr ||
        label_min_area_ == nullptr || track_min_area_ == nullptr ||
        label_min_circularity_ == nullptr || track_min_circularity_ == nullptr ||
        button_open_report_dir_ == nullptr)
    {
      return;
    }

    const int left = 16;
    const int right = 16;
    int width = right_panel_->ClientSize.Width - left - right;
    // Reserve scrollbar space conservatively to avoid text clipping when scroll appears.
    width -= (SystemInformation::VerticalScrollBarWidth + 6);
    width = Math::Max(240, width);

    textbox_path_->Width = width;
    label_result_->Width = width;
    label_param_title_->Width = width;
    label_h_low_->Width = width;
    track_h_low_->Width = width;
    label_h_high_->Width = width;
    track_h_high_->Width = width;
    label_s_low_->Width = width;
    track_s_low_->Width = width;
    label_v_low_->Width = width;
    track_v_low_->Width = width;
    label_min_area_->Width = width;
    track_min_area_->Width = width;
    label_min_circularity_->Width = width;
    track_min_circularity_->Width = width;
    button_open_report_dir_->Width = width;

    button_browse_->Location = Point(left, button_browse_->Location.Y);
    button_browse_->Width = 92;
    button_detect_->Location = Point(button_browse_->Right + 8, button_detect_->Location.Y);
    button_detect_->Width = Math::Max(120, left + width - button_detect_->Location.X);

  }

  void OnParamScroll(Object^ sender, EventArgs^)
  {
    if (sender == track_h_low_ && track_h_low_->Value > track_h_high_->Value)
    {
      track_h_high_->Value = track_h_low_->Value;
    }
    if (sender == track_h_high_ && track_h_high_->Value < track_h_low_->Value)
    {
      track_h_low_->Value = track_h_high_->Value;
    }
    UpdateParamLabels();
  }

  void UpdateParamLabels()
  {
    label_h_low_->Text = String::Format("H 下限: {0}", track_h_low_->Value);
    label_h_high_->Text = String::Format("H 上限: {0}", track_h_high_->Value);
    label_s_low_->Text = String::Format("S 下限: {0}", track_s_low_->Value);
    label_v_low_->Text = String::Format("V 下限: {0}", track_v_low_->Value);
    label_min_area_->Text = String::Format("最小面积: {0}", track_min_area_->Value);
    label_min_circularity_->Text =
        String::Format("最小圆度: {0:F2}", track_min_circularity_->Value / 100.0);
  }

  void SyncScannerParamsFromUi()
  {
    native_->scanner.SetHueRange(track_h_low_->Value, track_h_high_->Value);
    native_->scanner.SetBaseSLowThreshold(track_s_low_->Value);
    native_->scanner.SetVLowThreshold(track_v_low_->Value);
    native_->scanner.SetMinArea(track_min_area_->Value);
    native_->scanner.SetMinCircularity(track_min_circularity_->Value / 100.0);
  }

  String^ GetLogDirectoryPath()
  {
    return System::IO::Path::Combine(Application::StartupPath, "Log");
  }

  String^ GetTodayCsvPath()
  {
    String^ file_name = DateTime::Now.ToString("yyyy-MM-dd") + ".csv";
    return System::IO::Path::Combine(GetLogDirectoryPath(), file_name);
  }

  void AppendDetectionLog(int count, long long cost_time_ms, int current_s)
  {
    String^ log_dir = GetLogDirectoryPath();
    if (!System::IO::Directory::Exists(log_dir))
    {
      System::IO::Directory::CreateDirectory(log_dir);
    }

    String^ csv_path = GetTodayCsvPath();
    if (!System::IO::File::Exists(csv_path))
    {
      System::IO::File::AppendAllText(csv_path,
                                      "时间,数量,耗时(ms),自适应S阈值\r\n",
                                      System::Text::Encoding::UTF8);
    }

    String^ line = String::Format("{0},{1},{2},{3}\r\n",
                                  DateTime::Now.ToString("yyyy-MM-dd HH:mm:ss"),
                                  count,
                                  cost_time_ms,
                                  current_s);
    System::IO::File::AppendAllText(csv_path, line, System::Text::Encoding::UTF8);
  }

  void OnOpenReportDirClicked(Object^, EventArgs^)
  {
    String^ log_dir = GetLogDirectoryPath();
    if (!System::IO::Directory::Exists(log_dir))
    {
      System::IO::Directory::CreateDirectory(log_dir);
    }

    System::Diagnostics::ProcessStartInfo^ psi = gcnew System::Diagnostics::ProcessStartInfo();
    psi->FileName = log_dir;
    psi->UseShellExecute = true;
    System::Diagnostics::Process::Start(psi);
  }
};
}  // namespace BottleScannerWinFormsCli
