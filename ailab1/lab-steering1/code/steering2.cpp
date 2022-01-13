#include <iostream>
#include "draw-triangle-pro.hpp"
#include "raylib-cpp.hpp"
#include "steering2.h"


// C++ documentation is at https://robloach.github.io/raylib-cpp/index.html
// ...and a cheatsheet at: https://www.raylib.com/cheatsheet/cheatsheet.html

// 1. Add a target for the green ship using a point collected from the mouse position when the left button is pressed.
// 2. Direct the ship towards the target.

struct tri {
  raylib::Vector2 l_, r_, nose_;
  raylib::Vector2 pos_;
  float rotation_;
  raylib::Color col_;
  raylib::Vector2 target{ 0,0 };

  void draw()
  {
    ai::DrawTrianglePro(pos_, l_, r_, nose_, rotation_, col_);
  }
};

float GetNewRotation(raylib::Vector2 currentPosition, raylib::Vector2 targetPosition) {
     raylib::Vector2 direction = targetPosition.Subtract(currentPosition);
     float angle = direction.Angle(direction.Zero());
     //std::cout << "Direction: (" << direction.x << ", " <<direction.y << std::endl;
     //std::cout << "Angle: (" << angle -90 << std::endl;
     return angle - 90;
}

bool NotAtTarget(raylib::Vector2 position, raylib::Vector2 target) {
    return true;
}

//Which version of MoveTowardsTarget Method is better C++ Code
raylib::Vector2 MoveTowardsTarget(raylib::Vector2 position, raylib::Vector2 target) {
    raylib::Vector2 dir = target.Subtract(position);
    dir = dir.Normalize();
    std::cout << "Length: (" << dir.Length() << std::endl;
    return position += dir * GetFrameTime() * 150;
}

raylib::Vector2 MoveTowardsTarget(raylib::Vector2* position, raylib::Vector2* target) {
    raylib::Vector2 dir = target->Subtract(*position);
    dir = dir.Normalize();
    std::cout << "Length: (" << dir.Length() << std::endl;
    return *position += dir * GetFrameTime() * 150;
}

int main(int argc, char *argv[])
{
  float sw{1024}, sh{768};
  raylib::Window window(sw, sh, "Steering 2");

  SetTargetFPS(60);

  tri ship{{-10,0}, {10,0}, {0,-30}, {sw/2,sh/2}, 0, RED, {sw / 2,sh / 2} };

  //raylib::Vector2 target{ 0,0 };
  

  while (!window.ShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        ship.target = GetMousePosition();
        std::cout << "Target: (" << ship.target.GetX() <<", " << ship.target.GetY() << ")" << std::endl;
        ship.rotation_ = GetNewRotation(ship.pos_, ship.target);
    }
    
    if (NotAtTarget(ship.pos_, ship.target))
    {
        //ship.pos_ = MoveTowardsTarget(ship.pos_, ship.target);
        ship.pos_ = MoveTowardsTarget(&ship.pos_, &ship.target);

    }
    ship.draw();

    //ship.rotation_ += 2;



    EndDrawing();
  }
  return 0;
}


