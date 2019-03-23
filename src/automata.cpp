#include <iostream>
#include <imgui_test/automata.hpp>
#include <imgui_test/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace imgui_test
{

Person::Person(const size_t& x, const size_t& y) :
    x_(x),
    y_(y)
{
}

void Person::notVeryRandomNewDir()
{
  x_move_max_ = rand() % 20 - 10;
  y_move_max_ = rand() % 20 - 10;
}

void Person::update(cv::Mat& map)
{
  if (map.empty()) {
    return;
  }
  ++tick_count_;
  if (tick_count_ % ticks_to_move_ != 0) {
    return;
  }
  // TODO(lucasw) handle swimming

  ++x_move_count_;
  ++y_move_count_;
  size_t nx = x_, ny = y_;
  if (y_move_count_ > std::abs(y_move_max_)) {
    y_move_count_ = 0;
    if (y_move_max_ > 0) {
      ++ny;
    } else if (y_move_max_ < 0) {
      --ny;
    }
    if (ny < map.rows) {
      const auto val = map.at<uint8_t>(ny, x_);
      if (val > 0) {
        y_ = ny;
      } else {
        notVeryRandomNewDir();
      }
    } else {
      notVeryRandomNewDir();
    }
  }
  if (x_move_count_ > std::abs(x_move_max_)) {
    x_move_count_ = 0;
    if (x_move_max_ > 0) {
      ++nx;
    } else if (x_move_max_ < 0) {
      --nx;
    }

    if (nx < map.cols) {
      const auto val = map.at<uint8_t>(y_, nx);
      if (val > 0) {
        x_ = nx;
      } else {
        notVeryRandomNewDir();
      }
    } else {
      notVeryRandomNewDir();
    }
  }

  if (move_count_ % change_dir_max_ == 0) {
    notVeryRandomNewDir();
  }

  ++move_count_;
}

bool Person::spawn()
{
  ++spawn_count_;
  if (spawn_count_ % spawn_max_ == spawn_max_ - 1) {
    return true;
  }
  return false;
}

Land::Land(const std::string& path)
{
  cv::Mat map = cv::imread(path, cv::IMREAD_GRAYSCALE);
  if (map.empty()) {
    throw std::runtime_error("couldn't load image from disk " + path);
  }

  cv::resize(map, map_, cv::Size(320, 200), cv::INTER_NEAREST);

  const auto width = map_.cols;
  const auto height = map_.rows;

  std::cout << width << " " << height << "\n";

  people_.emplace_back(Person(width/2, height/2));
}

void Land::update()
{
  if (map_.empty()) {
    return;
  }
  std::vector<Person> new_people;
  for(auto& person : people_) {
    person.update(map_);
    if ((people_.size() < people_limit_) && person.spawn()) {
      new_people.emplace_back(Person(person.x_, person.y_));
      new_people[new_people.size() - 1].spawn_max_ += rand() % 23;
    }
  }

  if (new_people.size() > 0) {
    std::cout << people_.size() << " new people " << new_people.size() << "\n";
    people_.insert(people_.end(), new_people.begin(), new_people.end());
  }

}

void Land::draw()
{
  const auto color = cv::Vec3b(255, 0, 0);
  cv::cvtColor(map_, image_, cv::COLOR_GRAY2RGB);
  for(auto& person : people_) {
    image_.at<cv::Vec3b>(person.y_, person.x_) = color;
  }
}

//////////////////////////////////////////////////////////

Automata::Automata(const ImVec2 size) : size_(size)
{
  window_flags_ = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_HorizontalScrollbar;
}

bool Automata::droppedFile(const std::string name)
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

void Automata::update()
{
  if (!land_) {
    return;
  }
  // TODO(lucasw) skip amount slider
  land_->update();
}

void Automata::drawImage()
{
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
  bool dirty = false;

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

#if 0
  if (!any_scrollbar_active) {
    ImVec2 image_pos = (io.MousePos - screen_pos1) / zoom_;
    int x = image_pos.x;
    int y = image_pos.y;
  }
#endif
}

void Automata::draw()
{
  ImGui::SetNextWindowPos(pos_);
  ImGui::SetNextWindowSize(ImVec2(size_.x, size_.y));

  bool is_open;
  ImGui::Begin("automata", &is_open, window_flags_);
  #if 1
  if (is_open) {
    if ((land_) && (!land_->map_.empty())) {
      msg_ = "updating land and people";
      // TODO(lucasw) if updated
      land_->draw();
      glTexFromMat(land_->image_, texture_id_);
      drawImage();
      cv::imshow("map", land_->map_);
    }
  }
  #endif
  ImGui::Text("%s", msg_.c_str());
  ImGui::End();
}

}  // namespace imgui_test
