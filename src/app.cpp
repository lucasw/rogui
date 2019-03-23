#include <imgui_test/app.hpp>
#include <imgui_test/utility.hpp>
#include <opencv2/highgui.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace imgui_test
{

App::App(const ImVec2 size) : size_(size)
{
  pos_.x = 0;
  pos_.y = 0;

  window_flags_ = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_HorizontalScrollbar;  // | ImGuiWindowFlags_NoDecoration;

  image_ = cv::Mat(cv::Size(width_, height_), CV_8UC3);
  glTexFromMat(image_, texture_id_);
}

void App::drawControls()
{
  // ImGui::Columns(2);
  float tmp = width_;
  const bool width_changed = ImGui::SliderFloat("width", &tmp, 1.0, 512.0, "%1.0f", 2.0);
  width_ = tmp;
  tmp = height_;
  const bool height_changed = ImGui::SliderFloat("height", &tmp, 1.0, 512.0, "%1.0f", 2.0);
  height_ = tmp;
  const bool zoom_changed = ImGui::SliderFloat("zoom", &zoom_, 0.1, 64.0, "%1.1f", 2.0);
  (void)zoom_changed;

  if (width_changed || height_changed) {
    cv::resize(image_, image_, cv::Size(width_, height_), 0.0, 0.0, cv::INTER_NEAREST);
    // dirty = true;
    glTexFromMat(image_, texture_id_);
  }

  ImGui::ColorEdit4("draw color", &draw_col_.x);

  // ImGui::NextColumn();
  // ImGui::Columns(1);
  ImGui::Text("%s", image_filename_.c_str());
  ImGui::Text("%s", msg_.c_str());
}

void App::drawImage()
{
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImVec2 image_size;
  image_size.x = image_.cols * zoom_;
  image_size.y = image_.rows * zoom_;

  ImVec2 win_sz = ImGui::GetWindowSize();
  bool dirty = false;

  float region_width = ImGui::GetWindowContentRegionWidth();
  // const ImVec2 uv0(-10.0, -10.0);  // = win_sz * 0.5 - image_size * 0.5;
  // const ImVec2 uv1(10.0, 10.0);  //  = win_sz * 0.5 + image_size * 0.5;
  // ImGui::Text("%f %f, %f %f", win_sz.x, win_sz.y, pos.x, pos.y);
  if (image_size.x < region_width) {
    ImGui::SetCursorPosX((region_width - image_size.x) * 0.5);
  }
  // float region_height = ImGui::GetWindowContentRegionHeight();
  float region_height = win_sz.y;
  if (image_size.y < region_height) {
    ImGui::SetCursorPosY((region_height - image_size.y) * 0.5);
  }
  ImVec2 screen_pos1 = ImGui::GetCursorScreenPos();
  ImGui::Image((void*)(intptr_t)texture_id_, image_size);  // , uv0, uv1);

  ImGuiWindow* window = ImGui::GetCurrentWindow();
  ImGuiID active_id = ImGui::GetActiveID();
  const bool any_scrollbar_active = active_id &&
      (active_id == ImGui::GetScrollbarID(window, ImGuiAxis_X) ||
       active_id == ImGui::GetScrollbarID(window, ImGuiAxis_Y));

  if (!any_scrollbar_active) {
    ImVec2 image_pos = (io.MousePos - screen_pos1) / zoom_;
    int x = image_pos.x;
    int y = image_pos.y;
    cv::Vec3b col(draw_col_.z * 255, draw_col_.y * 255, draw_col_.x * 255);
    bool clicked = ImGui::IsMouseDown(0);
    if ((x >= 0) && (y >= 0) && (x < image_.cols) && (y < image_.rows)) {
      // mouse_over_image = true;
      if (clicked) {
        image_.at<cv::Vec3b>(y, x) = col;
        dirty = true;
      }
      col = image_.at<cv::Vec3b>(y, x);
    }
    ImGui::Text("%d %d, %d %d %d, %d", x, y, col[0], col[1], col[2], clicked);
  }

  if (dirty) {
    // image_ = cv::Mat(cv::Size(width_, height_), CV_8UC3);
    glTexFromMat(image_, texture_id_);
  }

}

void App::drawPrimitives()
{
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  // draw primitives
  ImVec2 pt = ImGui::GetCursorScreenPos();
  const ImU32 col32 = ImColor(draw_col_);
  // const float spacing = 30.0;
  const float sz = 4.0;
  // can't draw too many
  // imgui_test: /home/lwalter/own/imgui_test/imgui/imgui.cpp:3937: void AddDrawListToDrawData(ImVector<ImDrawList*>*, ImDrawList*): Assertion `draw_list->_VtxCurrentIdx < (1 << 16) && "Too many vertices in ImDrawList using 16-bit indices. Read comment above"' failed.
  for (int i = 0; i < 100; ++i) {
    for (int j = 0; j < 100; ++j) {
      const float cx = pt.x + j * sz * 1.5;
      const float cy = pt.y + i * sz * 1.5;
      // draw_list->AddCircle(ImVec2(cx, cy), 7.0, col32, 4, 4.0);
      draw_list->AddRectFilled(ImVec2(cx, cy), ImVec2(cx + sz, cy + sz), col32);
    }
  }
}

void App::draw()
{
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  bool is_open = true;

  {
    ImGui::SetNextWindowPos(pos_);
    ImGui::SetNextWindowSize(ImVec2(size_.x * 0.25, size_.y));
    ImGui::Begin("app test", &is_open, window_flags_);
    if (is_open) {
      drawControls();
    }
    ImGui::End();
  }

  // bool mouse_over_image = false;
  ImGui::SetNextWindowPos(ImVec2(pos_.x + size_.x * 0.25, pos_.y));
  ImGui::SetNextWindowSize(ImVec2(size_.x * 0.75, size_.y));
  ImGui::Begin("app image", &is_open, window_flags_);

  if (is_open) {
    if (ImGui::BeginTabBar("##TabBar")) {
      if (ImGui::BeginTabItem("Image")) {
        drawImage();
        ImGui::EndTabItem();
      }  // Image Tab

      if (ImGui::BeginTabItem("Draw")) {
        drawPrimitives();
        ImGui::EndTabItem();
      }  // Draw primitives
      ImGui::EndTabBar();
    }
  }  // if open

#if 0
  // if (mouse_over_image)
  {
    auto wheel_diff = io.MouseWheel;
    zoom_ += wheel_diff * 0.5;
    // ImGui::Text("zoom %f %f %f", zoom_, wheel_diff, io.MouseWheel);
  }
  mouse_wheel_ = io.MouseWheel;

  mouse_over_image_ = mouse_over_image;
#endif
  ImGui::End();
}

bool App::droppedFile(const std::string name)
{
  if (name == "") return false;
  cv::Mat image = cv::imread(name, cv::IMREAD_COLOR);
  if (image.empty()) {
    msg_ = "Could not load image '" + name + "'";
    return false;
  }
  image_ = image;
  width_ = image_.cols;
  height_ = image_.rows;
  glTexFromMat(image_, texture_id_);
  image_filename_ = name;
  msg_ = "loaded dropped image '" + name + "'";
  return true;
}

}  // namespace imgui_test
