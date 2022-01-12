#include "vec.hpp"
#include "draw-triangle-pro.hpp"
#include "raylib-cpp.hpp"

// The scene drawn shows positive x downwards and positive z to the right:
// ....> Z (+ve)
// .
// .
// .
// X (+ve)

class SteeringOutput
{
public:
};

class Kinematic
{
public:
};

int main(int argc, char *argv[])
{
  float sw{1024}, sh{768};
  raylib::Window window(sw, sh, "Steering 3");

  SetTargetFPS(60);

  while (!window.ShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      const auto mpos = GetMousePosition();
    }

    ai::DrawTrianglePro({sw/2,sh/2}, {0,-10}, {0,10}, {30,0}, 0, BLUE);

    EndDrawing();
  }

  return 0;
}
