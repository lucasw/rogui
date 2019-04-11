#ifndef ROGUI_MAP_HPP
#define ROGUI_MAP_HPP

#include <functional>
#include <imgui.h>
#include <list>
#include <map>
#include <memory>
// #include <opencv2/imgproc.hpp>
#include <vector>

namespace rogui
{

class Character;

enum CELL_TYPES
{
  IMPASSABLE = 0,
  STONE,
  FLOOR
};

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

class Map;

// TODO(lucasw) don't want shared_ptr constructor/copy overhead in functions
// getting called often.
void generateInit(std::shared_ptr<Map> map);
void generateRandom(std::shared_ptr<Map> map);
void generateDucci(std::shared_ptr<Map> map);

void populateMap(std::shared_ptr<Map> map);

class Player;

class Map
{
public:
  Map(const size_t width, const size_t height);

  bool passable(const int& x, const int& y);

  void drawCell(const float screen_x, const float screen_y, const float scale,
    const uint8_t cell_type);

  void draw(const int x, const int y, const size_t width, const size_t height,
    const float scale);

  std::vector<uint8_t> grid_;
  std::map<uint8_t, Cell> cells_;

  std::shared_ptr<Player> player_;
  std::list<std::shared_ptr<Character> > characters_;

  const size_t width_;
  const size_t height_;
};

}  // namespace rogui

#endif  // ROGUI_MAP_HPP
