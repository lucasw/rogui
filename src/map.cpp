#include <iostream>
#include <rogui/character.hpp>
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

void generateInit(std::shared_ptr<Map> map)
{
  std::cout << "Map generation\n";
  if (map == nullptr) {
    throw std::runtime_error("null map passed to generator");
  }

  // TODO(lucasw) load all the cell types from a text file
  Cell impassable("impassable");
  impassable.col32_ = ImColor(ImVec4(0.2, 0.2, 0.2, 1.0));
  map->cells_.insert(std::make_pair(IMPASSABLE, impassable));

  Cell stone("stone");
  stone.col32_ = ImColor(ImVec4(0.4, 0.4, 0.4, 1.0));
  map->cells_.insert(std::make_pair(STONE, stone));

  // TODO(lucasw) later have a 3D block conception of the map,
  // for now have different floor vs. wall materials
  Cell floor("floor");
  floor.col32_ = ImColor(ImVec4(0.6, 0.5, 0.3, 1.0));
  map->cells_.insert(std::make_pair(FLOOR, floor));

  const size_t& wd = map->width_;
  const size_t& ht = map->height_;

  for (size_t y = 1; y < ht - 1; ++y) {
    for (size_t x = 1; x < wd - 1; ++x) {
      const size_t ind = y * wd + x;
      map->grid_[ind] = IMPASSABLE;
    }
  }

  // make one large room with random stones in it
  // later TODO(lucasw) load from file or generate randomly elsewhere
  for (size_t y = 1; y < ht - 1; ++y) {
    for (size_t x = 1; x < wd - 1; ++x) {
      const size_t ind = y * wd + x;
      map->grid_[ind] = STONE;
    }
  }
}

void generateRandom(std::shared_ptr<Map> map)
{
  std::cout << "Map generate random\n";
  if (map == nullptr) {
    throw std::runtime_error("null map passed to generator");
  }

  const size_t& wd = map->width_;
  const size_t& ht = map->height_;

  // make one large room with random stones in it
  // later TODO(lucasw) load from file or generate randomly elsewhere
  const size_t border = 2;
  for (size_t y = border; y < ht - border; ++y) {
    for (size_t x = border; x < wd - border; ++x) {
      const size_t ind = y * wd + x;
      if (rand() % 10 == 0) {
        continue;
      }
      map->grid_[ind] = FLOOR;
    }
  }
}

void generateDucci(std::shared_ptr<Map> map)
{
  std::cout << "Map generate ducci\n";
  if (map == nullptr) {
    throw std::runtime_error("null map passed to generator");
  }

#if 0
  const size_t& wd = map->width_;
  const size_t& ht = map->height_;
#endif

  std::vector<std::array<int32_t, 3> > ducci;
  const size_t num = 48;
  ducci.reserve(num);
  // ducci.push_back({0, 653, 1854, 4063});
  ducci.push_back({2, 4126087, 4126085});

  for (size_t i = 1; i < num; ++i) {
    const auto& ducci1 = ducci[i - 1];
    auto ducci2(ducci1);
    for (size_t ind = 0; ind < ducci2.size(); ++ind) {
      const size_t next_ind = (i + 1) % ducci1.size();
      ducci2[ind] = std::abs(ducci1[next_ind] - ducci1[ind]);
    }
    ducci.push_back(ducci2);
  }

  size_t count = 0;
  for (const auto& d : ducci) {
    for (const auto& num : d) {
      for (size_t i = 0; i < sizeof(num) * CHAR_BIT; ++i) {
        if (count >= map->grid_.size()) {
          break;
        }
        // auto grid_loc  = count / (wd - 2)
        // const auto& grid_val = map->grid_[count];
        // if (grid_val != IMPASSABLE) {
        if ((num & (0x1 << i)) == 0) {
          map->grid_[count] = FLOOR;
        }
        ++count;
      }
      std::cout << num << " ";
    }
    std::cout << "\n";
  }
}

Map::Map(const size_t width, const size_t height) :
  width_(width),
  height_(height)
{
  std::cout << "new Map " << width_ << " " << height_ << " " << width_ * height_ << "\n";
  grid_.resize(width_ * height_);
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
