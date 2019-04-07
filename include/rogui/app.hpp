#include <imgui.h>
#include <rogui/gl.h>
#include <opencv2/imgproc.hpp>

namespace rogui
{

class App
{
public:
  App(const ImVec2 size);

  void draw();

  std::string msg_;
  std::string image_filename_;
  bool droppedFile(const std::string name);

  void drawControls();
  void drawImage();
  void drawPrimitives();

  cv::Mat image_;
  float mouse_wheel_ = 0.0;
  bool mouse_over_image_ = false;
  int width_ = 16;
  int height_ = 16;
  float zoom_ = 8.0;

  // float draw_col_[3] = {0.5, 0.5, 0.5};
  ImVec4 draw_col_ = ImVec4(0.5, 0.5, 0.5, 1.0);
  ImVec2 pos_;
  ImVec2 size_;
  ImGuiWindowFlags window_flags_ = ImGuiWindowFlags_None;

  GLuint texture_id_;
};

}  // namespace rogui
