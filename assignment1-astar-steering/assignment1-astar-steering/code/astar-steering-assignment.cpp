#include "astar-steering-assignment.hpp"

int main(int argc, char *argv[])
{
  const int w{1024}, h{768}, half_w{w/2}, half_h{h/2}, gap{200};
  raylib::Window window(w, h, "A* Steering Assignment");

  raylib::AudioDevice audiodevice;
  auto file = "../../../raylib-cpp-4.0.0/examples/audio/resources/weird.wav";
  raylib::Sound fx(file);

  SetTargetFPS(60);

  Ship ship{10, half_h, 0, RED};

  const float max_accel{2000}, max_ang_accel{50}, max_rot{3}, max_speed{220};
  const float drag_factor{0.5};

  Kinematic path_offset_target{{100,0,100}, 180 * DEG2RAD};
  FollowPath fp{ship, path_offset_target, {}, 0.1, max_accel, max_speed};
  Align align{ship, path_offset_target, max_ang_accel, max_rot, 0.01, 0.1};
  BlendedSteering blend{{{fp,0.5},{align,0.5}}, max_accel, max_ang_accel};

  Graph g;
  add_node(g, 'A', {half_h,       0, half_w - gap});
  add_node(g, 'B', {half_h,       0, half_w      });
  add_node(g, 'C', {half_h - gap, 0, half_w      });
  add_node(g, 'D', {half_h + gap, 0, half_w      });
  add_node(g, 'E', {half_h + gap, 0, half_w + gap});
  add_node(g, 'F', {half_h,       0, half_w + gap});
  add_node(g, 'G', {half_h - gap, 0, half_w + (2*gap) });
  add_double_edge(g, 'A', 'B');
  add_double_edge(g, 'B', 'C');
  add_double_edge(g, 'B', 'D');
  add_double_edge(g, 'C', 'A');
  add_double_edge(g, 'D', 'E');
  add_double_edge(g, 'D', 'A');
  add_double_edge(g, 'E', 'B');
  add_double_edge(g, 'B', 'F');
  add_double_edge(g, 'C', 'F');
  add_double_edge(g, 'C', 'G');
  add_double_edge(g, 'F', 'G');

  std::vector<Vector> v{node_info['A'], node_info['B'], node_info['F'], node_info['G']};
  FollowPath& pr = std::get<FollowPath>(blend.behaviours_[0].behaviour_);
  pr.path_.set_waypoints(v); // n.b.

  while (!window.ShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    ship.draw();
    pr.path_.draw();
    draw_graph(g);

    EndDrawing();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      const ai::Vector2 mpos = GetMousePosition();
      //fx.Play();
      ship.position_.z = mpos.x;
      ship.position_.x = mpos.y;
    }

    auto steer = blend.getSteering();
    ship.update(steer, max_speed, drag_factor, GetFrameTime());
  }

  return 0;
}
