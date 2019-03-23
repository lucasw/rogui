#include <imgui.h>
#include <imgui_test/gl.h>
#include <list>
#include <map>
#include <memory>
#include <opencv2/imgproc.hpp>

namespace imgui_test
{

class Person
{
public:
  Person(const size_t& x, const size_t& y, const std::string& nation);

  void notVeryRandomNewDir();
  bool update(cv::Mat& map);
  bool spawn();

  size_t move_count_ = 0;
  int x_move_count_ = 0;
  int x_move_max_ = 4;
  int y_move_count_ = 0;
  int y_move_max_ = 0;

  size_t tick_count_ = 0;
  size_t ticks_to_move_ = 3;
  size_t x_;
  size_t y_;

  size_t new_x_;
  size_t new_y_;

  size_t change_dir_max_ = 41;
  size_t change_dir_count_ = 0;

  size_t spawn_count_ = 0;
  size_t spawn_max_ = 173;

  std::string nation_ = "";

  cv::Vec3b color_ = cv::Vec3b(255, 0, 0);
};

class Land
{
public:
  Land(const std::string& path);
  void update();
  void draw();

  void addPerson(size_t x, size_t y, const std::string& nation);
  void removePersonFromMap(std::shared_ptr<Person> person);
  void addPersonToMap(std::shared_ptr<Person> person);
  void resetPeople();

  // std::map<std::string, std::vector< std::shared_ptr<Person> > > peoples_;
  std::map<std::string, cv::Vec3b> nation_colors_;
  std::map<std::string, size_t> num_peoples_;

  cv::Mat image_;
  cv::Mat map_;

  // the indices into here are position on map
  std::vector<std::list<std::shared_ptr<Person> > > people_on_map_;

  const size_t people_limit_ = 110000;
};

class Automata
{
public:
  Automata(const ImVec2 size);

  bool droppedFile(const std::string name);

  void update();
  void draw();
  void drawImage();

  std::shared_ptr<Land> land_;

  float zoom_ = 3.0;

  std::string msg_ = "Automata ready";
  ImVec2 pos_;
  ImVec2 size_;
  ImGuiWindowFlags window_flags_ = ImGuiWindowFlags_None;
  GLuint texture_id_;
};

}  // namespace imgui_test
