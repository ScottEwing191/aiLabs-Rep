#include "raylib-cpp.hpp"
#include <vector>

// C++ Documentation is at https://robloach.github.io/raylib-cpp/index.html
// ... and https://www.raylib.com/cheatsheet/cheatsheet.html

class shape
{
public:

  enum struct shape_type { circle, square };

  shape(shape_type s, raylib::Vector2 p, raylib::Color c, float sz)
    : shape_{s}, posn_{p}, colour_{c}, size_{sz} {}

  shape_type shape_;
  raylib::Vector2 posn_;
  raylib::Color colour_;
  float size_;
};

int main(int argc, char *argv[])
{
  raylib::Color text_colour(GREEN);
  raylib::Window window(800, 600, "Steering Lab 1");
  std::vector<shape> my_shapes;

  raylib::Color col{255, 161, 0}; // ORANGE
  shape s1{shape::shape_type::circle, {0,0}, col, 40};
  my_shapes.push_back(s1);

  SetTargetFPS(60);

  while (!window.ShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      my_shapes.back().posn_ = GetMousePosition();
    }

    for (auto s : my_shapes) {
      if (s.shape_ == shape::shape_type::circle)
        s.posn_.DrawCircle(s.size_, s.colour_);
      else
        s.posn_.DrawRectangle({s.size_,s.size_}, s.colour_);
    }

    EndDrawing();
  }

  return 0;
}
