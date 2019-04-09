#include <iostream>
#include <rogui/map.hpp>
#include <rogui/rogui.hpp>
#include <rogui/utility.hpp>
#if 0
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#endif

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace rogui
{

Map::Map(const size_t width, const size_t height) :
  width_(width),
  height_(height)
{
  std::cout << "new Map " << width_ << " " << height_ << " " << width_ * height_ << "\n";

  grid_.resize(width_ * height_);

  // TODO(lucasw) load all the cell types from a text file
  Cell impassable("impassable");
  impassable.col32_ = ImColor(ImVec4(0.2, 0.2, 0.2, 1.0));
  cells_.insert(std::make_pair(0, impassable));

  Cell stone("stone");
  stone.col32_ = ImColor(ImVec4(0.4, 0.4, 0.4, 1.0));
  cells_.insert(std::make_pair(1, stone));

  // TODO(lucasw) later have a 3D block conception of the map,
  // for now have different floor vs. wall materials
  Cell floor("floor");
  floor.col32_ = ImColor(ImVec4(0.6, 0.5, 0.3, 1.0));
  cells_.insert(std::make_pair(2, floor));

  for (size_t y = 1; y < height_ - 1; ++y) {
    for (size_t x = 1; x < width_ - 1; ++x) {
      const size_t ind = y * width_ + x;
      grid_[ind] = 1;
    }
  }

  // make one large room with random stones in it
  // later TODO(lucasw) load from file or generate randomly elsewhere
  for (size_t y = 5; y < height_ - 5; ++y) {
    for (size_t x = 5; x < width_ - 5; ++x) {
      const size_t ind = y * width_ + x;
      if (rand() % 10 == 0) {
        continue;
      }
      grid_[ind] = 2;
    }
  }
}

bool Map::passable(const int& x, const int& y)
{
  if (x < 0) {
    return false;
  }
  if (y < 0) {
    return false;
  }
  if (x >= static_cast<int>(width_)) {
    return false;
  }
  if (y >= static_cast<int>(height_)) {
    return false;
  }
  const size_t ind = y * width_ + x;
  if (grid_[ind] == 2) {
    return true;
  }

  return false;
}

void Map::drawCell(const float screen_x, const float screen_y, const float scale,
    const uint8_t cell_type)
{
  if (cells_.count(cell_type) == 0) {
    return;
  }

  cells_.at(cell_type).draw(screen_x, screen_y, scale, scale);
}

void Map::draw(const int x, const int y, const size_t width, const size_t height,
    const float scale)
{
  const size_t start_x = (x > 0) ? x : 0;
  const size_t start_y = (y > 0) ? y : 0;
  if (x + static_cast<int>(width) <= 0) {
    return;
  }
  if (y + static_cast<int>(height) <= 0) {
    return;
  }

  ImVec2 pt = ImGui::GetCursorScreenPos();

  for (size_t v_y = start_y; v_y < start_y + height && v_y < height_; ++v_y) {
    for (size_t v_x = start_x; v_x < start_x + width && v_x < width_; ++v_x) {
      const size_t ind = v_y * width_ + v_x;
      if (ind >= grid_.size()) {
        throw std::runtime_error("grid unexpectedly small: " +
            std::to_string(ind) + " >= " + std::to_string(grid_.size()));
      }
      const float screen_x = pt.x + (v_x - x) * scale;
      const float screen_y = pt.y + (v_y - y) * scale;

      drawCell(screen_x, screen_y, scale, grid_[ind]);
    }
  }

  player_->draw(pt, scale);
}

}  // namespace rogui
