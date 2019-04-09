
#include <imgui.h>
#include <rogui/character.hpp>
#include <rogui/map.hpp>

namespace rogui
{

bool Character::move(const int dx, const int dy)
{
  const int tmp_x = x_ + dx;
  const int tmp_y = y_ + dy;
  // std::cout << dx << " " << dy << "\n";
  if (auto map = map_.lock()) {
    // TODO(lucasw) later passability will depend on player traits
    if (map->passable(tmp_x, tmp_y)) {
      x_ = tmp_x;
      y_ = tmp_y;
      return true;
    }
  }
  return false;
}

void Character::draw(const ImVec2 window_offset, const float scale)
{
  // ImDrawList* draw_list = ImGui::GetWindowDrawList();

  const float screen_x = window_offset.x + x_ * scale;
  const float screen_y = window_offset.y + y_ * scale;
  // TODO(lucasw) change font size based on scale - but do it once
  // at beginning of map draw and then restore default.
  ImGui::SetCursorScreenPos(ImVec2(screen_x + scale * 0.3, screen_y + scale * 0.1));
  // TODO(lucasw) text color
  ImGui::Text("%c", sym_);
}


}  // rogui
