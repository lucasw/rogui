#include <iostream>
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
}

//////////////////////////////////////////////////////////////////////////
Rogui::Rogui(const ImVec2 size) : size_(size)
{
  std::cout << "new Rogui\n";
  window_flags_ = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_HorizontalScrollbar;

  map_ = std::make_shared<Map>(30, 30);
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

void Rogui::update()
{
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