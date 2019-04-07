#include <SDL.h>
#include <functional>
#include <imgui.h>
#include <rogui/gl.h>
#include <list>
#include <map>
#include <memory>
// #include <opencv2/imgproc.hpp>
#include <vector>

namespace rogui
{

struct Cell
{
  Cell(const std::string& name) : name_(name)
  {
  }

  void draw(const float x, const float y, const float w, const float h)
  {
    // TODO(lucasw) or make this parameter?
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // TODO(lucasw) later draw a small graphic
    const float offset = 1.0;
    draw_list->AddRectFilled(ImVec2(x + offset, y + offset),
        // ImVec2(x + w - offset * 2.0, y + h - offset * 2.0), col32_);
        ImVec2(x + w - offset, y + h - offset), col32_);
  }

  const std::string name_;
  ImU32 col32_;
};

class Player
{
public:
  Player(const std::string& name);

  void handleKey(const SDL_Keycode& key);

  void move(const int dx, const int dy);

  void draw(const ImVec2 window_offset, const float scale);

  std::map<SDL_Keycode, std::function<void ()> > key_actions_;

  // const
  char sym_ = '@';

  size_t x_;
  size_t y_;

  const std::string name_;
};

class Map
{
public:
  Map(const size_t width, const size_t height);

  void drawCell(const float screen_x, const float screen_y, const float scale,
    const uint8_t cell_type);

  void draw(const int x, const int y, const size_t width, const size_t height,
    const float scale);

  std::vector<uint8_t> grid_;
  std::map<uint8_t, Cell> cells_;

  // TODO(lucasw) list of entities
  std::shared_ptr<Player> player_;

  const size_t width_;
  const size_t height_;
};

class Rogui
{
public:
  Rogui(const ImVec2 size);

#if 0
  bool droppedFile(const std::string name);
#endif

  void update(const std::vector<int>& key_presses);

  void draw();
  void drawImage();

  float zoom_ = 3.0;

  std::shared_ptr<Map> map_;
  std::shared_ptr<Player> player_;

  std::string msg_ = "Rogui ready";
  ImVec2 pos_;
  ImVec2 size_;
  ImGuiWindowFlags window_flags_ = ImGuiWindowFlags_None;

  GLuint texture_id_;
};

}  // namespace rogui
