#include <imgui_test/app.hpp>
#include <opencv2/highgui.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace imgui_test
{

bool glTexFromMat(cv::Mat& image, GLuint& texture_id)
{
  if (image.empty()) {
    // TODO(lucasw) or make the texture 0x0 or 1x1 gray.
    return false;
  }

  glBindTexture(GL_TEXTURE_2D, texture_id);

  // Do these know which texture to use because of the above bind?
  // TODO(lucasw) make these configurable live
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Set texture clamping method - GL_CLAMP isn't defined
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // use fast 4-byte alignment (default anyway) if possible
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  // glPixelStorei(GL_UNPACK_ALIGNMENT, (image.step & 3) ? 1 : 4);

  // copy the data to the graphics memory
  // TODO(lucasw) actually look at the image encoding type and
  // have a big switch statement here
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
               image.cols, image.rows,
               0, GL_BGR, GL_UNSIGNED_BYTE, image.ptr());

  // set length of one complete row in data (doesn't need to equal image.cols)
  // glPixelStorei(GL_UNPACK_ROW_LENGTH, image.step / image_.elemSize());

  glBindTexture(GL_TEXTURE_2D, 0);

  return true;
}

App::App()
{
  pos_.x = 0;
  pos_.y = 0;

  size_.x = 300;
  size_.y = 720;

  window_flags_ = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_HorizontalScrollbar;  // | ImGuiWindowFlags_NoDecoration;

  image_ = cv::Mat(cv::Size(width_, height_), CV_8UC3);
  glTexFromMat(image_, texture_id_);
}

void App::draw()
{
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  ImGui::SetNextWindowPos(pos_);
  ImGui::SetNextWindowSize(ImVec2(size_.x * 0.25, size_.y));
  bool is_open = true;
  bool dirty = false;
  ImGui::Begin("app test", &is_open, window_flags_);
  if (is_open) {
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
      dirty = true;
    }

    ImGui::ColorEdit3("color 1", draw_col_);

    // ImGui::NextColumn();
    // ImGui::Columns(1);
    ImGui::Text("%s", image_filename_.c_str());
    ImGui::Text("%s", msg_.c_str());
  }
  ImGui::End();

  ImGui::SetNextWindowPos(ImVec2(pos_.x + size_.x * 0.25, pos_.y));
  ImGui::SetNextWindowSize(ImVec2(size_.x * 0.75, size_.y));
  ImGui::Begin("app image", &is_open, window_flags_);
  if (is_open) {
    ImVec2 image_size;
    image_size.x = image_.cols * zoom_;
    image_size.y = image_.rows * zoom_;

    ImVec2 screen_pos1 = ImGui::GetCursorScreenPos();
    ImGui::Image((void*)(intptr_t)texture_id_, image_size);

    {
      ImVec2 image_pos = (io.MousePos - screen_pos1) / zoom_;
      int x = image_pos.x;
      int y = image_pos.y;
      cv::Vec3b col(draw_col_[2] * 255, draw_col_[1] * 255, draw_col_[0] * 255);
      bool clicked = ImGui::IsMouseDown(0);
      if ((x >= 0) && (y >= 0) && (x < image_.cols) && (y < image_.rows)) {
        if (clicked) {
          ImGui::Text("draw");
          image_.at<cv::Vec3b>(y, x) = col;
          dirty = true;
        }
        col = image_.at<cv::Vec3b>(y, x);
      }
      ImGui::Text("%d %d, %d %d %d, %d", x, y, col[0], col[1], col[2], clicked);
    }
  }
  ImGui::End();
  if (dirty) {
    // image_ = cv::Mat(cv::Size(width_, height_), CV_8UC3);
    glTexFromMat(image_, texture_id_);
  }

}

bool App::droppedFile(const std::string name)
{
  cv::Mat image = cv::imread(name, cv::IMREAD_COLOR);
  if (!image.empty()) {
    image_ = image;
    width_ = image_.cols;
    height_ = image_.rows;
    glTexFromMat(image_, texture_id_);
    image_filename_ = name;
    msg_ = "loaded dropped image '" + name + "'";
  } else {
    msg_ = "Could not load image '" + name + "'";
  }
}

}  // namespace imgui_test
