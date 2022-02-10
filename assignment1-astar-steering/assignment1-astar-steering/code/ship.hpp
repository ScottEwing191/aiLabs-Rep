#ifndef _SHIP_HPP_
#define _SHIP_HPP_

#include <cmath> // std::fmod

class Ship : public Kinematic
{
public:
  Ship(const float z, const float x, const float ori, const raylib::Color col)
    : Kinematic{{x,0,z},ori}, col_{col} { }

  raylib::Color col_;

  void draw()
  {
    const float w = 10, len = 30; // ship width and length
    const ai::Vector2 l{0, -w}, r{0, w}, nose{len, 0};
    ai::Vector2 pos{position_.z, position_.x}; // Get a 2D coord for drawing
    float ori = -orientation_ * RAD2DEG; // negate: anticlockwise rot

    // wrap
    const int screenwidth = GetScreenWidth();
    const int screenheight = GetScreenHeight();
    pos.x = std::fmod(pos.x, static_cast<float>(screenwidth));
    pos.y = std::fmod(pos.y, static_cast<float>(screenheight));
    pos.x = pos.x < 0 ? pos.x + screenwidth : pos.x;
    pos.y = pos.y < 0 ? pos.y + screenheight : pos.y;

    ai::DrawTrianglePro(pos, l, r, nose, ori, col_);
  }
};

#endif // _SHIP_HPP_
