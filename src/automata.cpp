#include <iostream>
#include <imgui_test/automata.hpp>
#include <imgui_test/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace imgui_test
{

Person::Person(const size_t& x, const size_t& y, const std::string& nation) :
    x_(x),
    y_(y),
    nation_(nation)
{
}

void Person::notVeryRandomNewDir()
{
  x_move_max_ = rand() % 20 - 10;
  y_move_max_ = rand() % 20 - 10;
}

bool Person::update(cv::Mat& map)
{
  if (map.empty()) {
    return false;
  }
  ++tick_count_;
  if (tick_count_ % ticks_to_move_ != 0) {
    return false;
  }
  // TODO(lucasw) handle swimming

  bool moved = false;

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
        moved = true;
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
        moved = true;
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
  return moved;
}

bool Person::spawn()
{
  ++spawn_count_;
  if (spawn_count_ % spawn_max_ == spawn_max_ - 1) {
    return true;
  }
  return false;
}

//////////////////////////////////////////////////////////////////////
Land::Land(const std::string& path)
{
  std::cout << "new Land\n";
  cv::Mat map = cv::imread(path, cv::IMREAD_GRAYSCALE);
  if (map.empty()) {
    throw std::runtime_error("couldn't load image from disk " + path);
  }

  cv::resize(map, map_, cv::Size(320, 200), cv::INTER_NEAREST);

  const auto width = map_.cols;
  const auto height = map_.rows;

  people_on_map_.resize(width * height);

  std::cout << width << " " << height << "\n";

  nation_colors_["red"] = cv::Vec3b(0, 0, 255);
  nation_colors_["blue"] = cv::Vec3b(255, 0, 0);

  addPerson(width / 2 - 50, height / 2, "red");
  addPerson(width / 2 + 50, height / 2, "blue");
}

void Land::update()
{
  if (map_.empty()) {
    return;
  }

  size_t num_people = 0;
  for(auto& people_pair : peoples_) {
    num_people += people_pair.second.size();
  }

  for(auto& people_pair : peoples_) {
    const std::string& nation = people_pair.first;
    std::vector<std::shared_ptr<Person> > new_people;
    for(auto& person : people_pair.second) {
      const int old_x = person->x_;
      const int old_y = person->y_;
      const bool moved = person->update(map_);
      if (moved) {
        // remove from old_x, old_y, add to current x_ y_
        removePersonFromMap(person, old_x, old_y);
        addPersonToMap(person);
      }

      if ((num_people < people_limit_) && person->spawn()) {
        auto new_person = std::make_shared<Person>(person->x_, person->y_, nation);
        new_person->spawn_max_ += rand() % 23;
        addPersonToMap(new_person);
        new_people.push_back(new_person);
      }
    }
    if (new_people.size() > 0) {
      people_pair.second.insert(people_pair.second.end(), new_people.begin(), new_people.end());
    }
  }
}

void Land::draw()
{
  cv::cvtColor(map_, image_, cv::COLOR_GRAY2RGB);
  for (int y = 0; y < map_.rows; ++y) {
    for (int x = 0; x < map_.cols; ++x) {
      const size_t ind = y * map_.cols + x;
      if (people_on_map_[ind].size() == 0) {
        continue;
      }

      std::map<std::string, size_t> nums;
      cv::Vec3b color(255, 255, 255);

      for (auto it = people_on_map_[ind].begin(); it != people_on_map_[ind].end(); ++it) {
        const std::string& nation = (*it)->nation_;
        nums[nation]++;
        // color -= nation_colors_[nation] * 0.1;
      }
      // image_.at<cv::Vec3b>(y, x) = color;  // image_.at<cv::Vec3b>(y, x) - color;
      if (nums["blue"] > nums["red"]) {
        image_.at<cv::Vec3b>(y, x) = nation_colors_["blue"];
      } else {
        image_.at<cv::Vec3b>(y, x) = nation_colors_["red"];
      }
    }
  }
#if 0
  for (const auto& people_pair : peoples_) {
    for (const auto& person : people_pair.second) {
      const auto x = person.x_;
      const auto y = person.y_;
      image_.at<cv::Vec3b>(y, x) = person.color_;  // image_.at<cv::Vec3b>(y, x) - color;
    }
  }
#endif
}

void Land::addPerson(size_t x, size_t y, const std::string& nation)
{
  if (map_.empty()) return;

  auto person = std::make_shared<Person>(x, y, nation);
  peoples_[nation].push_back(person);
  addPersonToMap(person);
}

void Land::addPersonToMap(std::shared_ptr<Person> person)
{
  const size_t ind = person->y_ * map_.cols + person->x_;
  people_on_map_[ind].push_back(person);
}

void Land::removePersonFromMap(std::shared_ptr<Person> person,
    const size_t old_x, const size_t old_y)
{
  const size_t ind = old_y * map_.cols + old_x;
  people_on_map_[ind].remove(person);
}


void Land::resetPeople()
{
  for(auto& people_pair : peoples_) {
    people_pair.second.clear();
  }
}

//////////////////////////////////////////////////////////

Automata::Automata(const ImVec2 size) : size_(size)
{
  std::cout << "new Automata\n";
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
        msg_ = "making new person " + std::to_string(x) + " " + std::to_string(y);
        const std::string nation = "blue";
        land_->addPerson(x, y, nation);
      }
    }
  }
}

void Automata::draw()
{
  ImGui::SetNextWindowPos(pos_);
  ImGui::SetNextWindowSize(ImVec2(size_.x * 0.25, size_.y));

  bool is_open = true;

  ImGui::Begin("##controls", &is_open, window_flags_);
  if (is_open) {
    if (ImGui::Button("reset")) {
      land_->resetPeople();
    }

    // ImGui::SliderInt("ticks between updates", &land_.ticks_to_move_, 1, 100);
  }
  ImGui::Text("%s", msg_.c_str());
  if (land_) {
    for(const auto& people_pair : land_->peoples_) {
      ImGui::Text("%s %lu peoples", people_pair.first.c_str(), people_pair.second.size());
    }
    ImGui::Text("image %d x %d", land_->image_.cols, land_->image_.rows);
    ImGui::Text("map %d x %d", land_->map_.cols, land_->map_.rows);
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
}

}  // namespace imgui_test
