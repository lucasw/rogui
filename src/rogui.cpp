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

Player::Player(const std::string& name) :
  name_(name)
{
  // TODO(lucasw) eventually these need to be context specific, what menu is being used
  key_actions_.insert(std::make_pair(SDLK_h, std::bind(&Player::move, this, -1, 0)));
  key_actions_.insert(std::make_pair(SDLK_j, std::bind(&Player::move, this, 0, 1)));
  key_actions_.insert(std::make_pair(SDLK_k, std::bind(&Player::move, this, 0, -1)));
  key_actions_.insert(std::make_pair(SDLK_l, std::bind(&Player::move, this, 1, 0)));

  key_actions_.insert(std::make_pair(SDLK_y, std::bind(&Player::move, this, -1, -1)));
  key_actions_.insert(std::make_pair(SDLK_u, std::bind(&Player::move, this, 1, -1)));
  key_actions_.insert(std::make_pair(SDLK_b, std::bind(&Player::move, this, -1, 1)));
  key_actions_.insert(std::make_pair(SDLK_n, std::bind(&Player::move, this, 1, 1)));
}

void Player::handleKey(const SDL_Keycode& key)
{
  if (key_actions_.count(key) > 0) {
    key_actions_.at(key)();
  }
}

bool Player::move(const int dx, const int dy)
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

void Player::draw(const ImVec2 window_offset, const float scale)
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

//////////////////////////////////////////////////////////////////////////
Rogui::Rogui(const ImVec2 size) : size_(size)
{
  std::cout << "new Rogui\n";
  window_flags_ = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_HorizontalScrollbar;

  map_ = std::make_shared<Map>(90, 40);
  generateInit(map_);
  generateRandom(map_);

  player_ = std::make_shared<Player>("player");
  player_->sym_ = '@';
  player_->map_ = map_;
  player_->x_ = map_->width_ / 2;
  player_->y_ = map_->height_ / 2;
  map_->player_ = player_;

  msg_.reserve(80 * 20);
}

#if 0
bool Rogui::droppedFile(const std::string name)
{
  if (name == "") return false;

  try {
    land_ = std::make_shared<Land>(name);
  } catch (std::runtime_error& ex) {
    msg_ = std::string(ex.what());
    return false;
  }
  msg_ = "loaded new map " + name;
  return true;
}
#endif

void Rogui::update(const std::vector<SDL_Keycode>& key_presses)
{
  for (const auto& key : key_presses) {
    player_->handleKey(key);
  }
}

void Rogui::drawImage()
{
#if 0
  if (!land_) {
    return;
  }
  if (land_->image_.empty()) return;
  cv::Mat image = land_->image_;

  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImVec2 image_size;
  image_size.x = image.cols * zoom_;
  image_size.y = image.rows * zoom_;

  ImVec2 win_sz = ImGui::GetWindowSize();
  // bool dirty = false;

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
    bool clicked = ImGui::IsMouseDown(0);
    if ((x >= 0) && (y >= 0) && (x < land_->image_.cols) && (y < land_->image_.rows)) {
      // mouse_over_image = true;
      if (clicked) {
        // make new Person
        msg_ = "making new person " + std::to_string(x) + " " + std::to_string(y) + " "
            + spawn_types_[spawn_ind_];
        const std::string nation = spawn_types_[spawn_ind_];
        land_->addPerson(x, y, nation);
      }
    }
  }
  #endif
}

void Rogui::draw()
{
  bool is_open = true;
  // draw the map
  {
    ImGui::SetNextWindowPos(pos_);
    const float map_x = size_.x * 0.75;
    const float map_y = size_.y * 0.75;
    ImGui::SetNextWindowSize(ImVec2(map_x, map_y));
    ImGui::Begin("##map view", &is_open, window_flags_);
    // TODO(lucasw) get this from gui
    const float scale = 16.0;
    const int grid_x = 0;
    const int grid_y = 0;
    const size_t grid_width = map_x / scale;
    const size_t grid_height = map_y / scale;
    map_->draw(grid_x, grid_y, grid_width, grid_height, scale);
    ImGui::End();
  }
  // draw the controls and status to the right

  // draw the console on the bottom

#if 0

  ImGui::Begin("##controls", &is_open, window_flags_);
  if (is_open) {
    if (ImGui::Button("reset")) {
      land_->resetPeople();
    }

    // ImGui::SliderInt("ticks between updates", &land_.ticks_to_move_, 1, 100);
  }
  ImGui::Text("%s", msg_.c_str());
  if (land_) {
    for(const auto& people_pair : land_->num_peoples_) {
      ImGui::Text("%s %lu peoples", people_pair.first.c_str(), people_pair.second);
    }
    ImGui::Text("image %d x %d", land_->image_.cols, land_->image_.rows);
    ImGui::Text("map %d x %d", land_->map_.cols, land_->map_.rows);

    ImGui::Combo("spawn type", &spawn_ind_, "Red\0Blue\0");
  }
  ImGui::End();

  ImGui::SetNextWindowPos(ImVec2(pos_.x + size_.x * 0.25, pos_.y));
  ImGui::SetNextWindowSize(ImVec2(size_.x * 0.75, size_.y));

  ImGui::Begin("##automata", &is_open, window_flags_);
  if (is_open) {
    if ((land_) && (!land_->map_.empty())) {
      msg_ = "updating land and people";
      // TODO(lucasw) if updated
      land_->draw();
      glTexFromMat(land_->image_, texture_id_);
      drawImage();
    }
  }
  ImGui::End();
#endif
}

}  // namespace rogui
