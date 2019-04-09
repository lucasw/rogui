#ifndef ROGUI_CHARACTER_HPP
#define ROGUI_CHARACTER_HPP

#include <memory>
#include <string>

struct ImVec2;

namespace rogui
{

class Map;

class Character
{
public:
  Character(const std::string& name) :
    name_(name)
  {

  }

  virtual ~Character()
  {
  }


  virtual bool move(const int dx, const int dy);
  virtual void draw(const ImVec2 window_offset, const float scale);

  std::string name_;
  char sym_ = '?';
  int x_;
  int y_;
  std::weak_ptr<Map> map_;
};

}

#endif  // ROGUI_CHARACTER_HPP
