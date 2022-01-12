#include "draw-triangle-pro.hpp"
#include "raylib-cpp.hpp"

// C++ documentation is at https://robloach.github.io/raylib-cpp/index.html
// ...and a cheatsheet at: https://www.raylib.com/cheatsheet/cheatsheet.html

// 1. Add a target for the green ship using a point collected from the mouse position when the left button is pressed.
// 2. Direct the ship towards the target.

struct tri {
  raylib::Vector2 l_, r_, nose_;
  raylib::Vector2 pos_;
  float rotation_;
  raylib::Color col_;

  void draw()
  {
    ai::DrawTrianglePro(pos_, l_, r_, nose_, rotation_, col_);
  }
};

int main(int argc, char *argv[])
{
  float sw{1024}, sh{768};
  raylib::Window window(sw, sh, "Steering 2");

  SetTargetFPS(60);

  tri ship{{-10,0}, {10,0}, {0,-30}, {sw/2,sh/2}, 0, RED};

  while (!window.ShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    ship.draw();

    ship.rotation_ += 2;

    EndDrawing();
  }

  return 0;
}
