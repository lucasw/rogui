#ifndef ROGUI_ROGUI_HPP
#define ROGUI_ROGUI_HPP

#include <SDL.h>
#include <functional>
#include <imgui.h>
#include <rogui/character.hpp>
#include <rogui/gl.h>
#include <list>
#include <map>
#include <memory>
// #include <opencv2/imgproc.hpp>
#include <vector>

namespace rogui
{

class Map;

class Player : public Character
{
public:
  Player(const std::string& name);

#if 0
  virtual bool move(const int dx, const int dy);
  virtual void draw(const ImVec2 window_offset, const float scale);
#endif

  void handleKey(const SDL_Keycode& key);
  std::map<SDL_Keycode, std::function<void ()> > key_actions_;
};

class Rogui
{
public:
  Rogui(const ImVec2 size);

#if 0
  bool droppedFile(const std::string name);
#endif

  void update(const std::vector<int>& key_presses);

  void draw();
  void drawImage();

  float zoom_ = 3.0;

  std::shared_ptr<Map> map_;
  std::shared_ptr<Player> player_;

  std::string msg_ = "Rogui ready";
  ImVec2 pos_;
  ImVec2 size_;
  ImGuiWindowFlags window_flags_ = ImGuiWindowFlags_None;

  GLuint texture_id_;
};

}  // namespace rogui

#endif  // ROGUI_ROGUI_HPP
