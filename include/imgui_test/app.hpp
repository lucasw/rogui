#include <imgui.h>
#include <imgui_test/gl.h>
#include <opencv2/imgproc.hpp>

namespace imgui_test
{

class App
{
public:
  App();

  void draw();

  std::string msg_;
  std::string image_filename_;
  bool droppedFile(const std::string name);

  cv::Mat image_;
  int width_ = 16;
  int height_ = 16;
  float zoom_ = 8.0;

  float draw_col_[3] = {0.5, 0.5, 0.5};
  ImVec2 pos_;
  ImVec2 size_;
  ImGuiWindowFlags window_flags_ = ImGuiWindowFlags_None;

  GLuint texture_id_;
};

}  // namespace imgui_test
