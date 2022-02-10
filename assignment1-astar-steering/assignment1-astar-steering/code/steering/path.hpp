#ifndef _PATH_HPP_
#define _PATH_HPP_

#include <vector>
#include <limits>

class Path
{
  std::vector<Vector> wps_;
  float total_length_{};

public:

  Path(const std::vector<Vector>& wps = {}) { set_waypoints(wps); }

  void set_waypoints(const std::vector<Vector>& wps) {
    wps_ = wps;
    total_length_ = 0;
    for (int i = 1; i < wps_.size(); ++i)
      total_length_ += (wps_[i] - wps_[i-1]).length();
  }

  float getParam(const Vector& position, const float& lastParam) const
  {
    Vector v{};
    float min_distance{std::numeric_limits<float>::max()};
    float cur_length_accum{}, param{};

    for (int i = 1; i < wps_.size(); ++i)
    {
      v = closest_point_on_segment(wps_[i-1], wps_[i], position);
      const float d = (v - position).length();
      if (d < min_distance)
      {
        min_distance = d;
        float f = cur_length_accum + (v - wps_[i-1]).length();
        param = f / total_length_;
      }

      cur_length_accum += (wps_[i] - wps_[i-1]).length();
    }

    return param;
  }

  Vector getPosition(float param) const
  {
    Vector ret{};

    float param_length = total_length_ * param;

    for (int i = 1; i < wps_.size(); ++i)
    {
      const Vector v = wps_[i] - wps_[i-1];
      const float len = v.length();
      if (len > param_length || i == (wps_.size() - 1)) {
        ret = wps_[i-1] + (param_length / len) * v;
        break;
      }

      param_length -= len;
    }

    return ret;
  }

  void add_waypoint(const float z, const float x)
  {
    wps_.push_back({x,0,z});
    int sz = wps_.size();
    if (sz > 1) {
      total_length_ += (wps_[sz-1] - wps_[sz-2]).length();
    }
  }

  // useful for debugging
  void draw()
  {
    int i = 1;
    for (; i < wps_.size(); ++i) {
      DrawLine(wps_[i-1].z, wps_[i-1].x, wps_[i].z, wps_[i].x, GRAY);
      DrawCircle(wps_[i-1].z,wps_[i-1].x, 4, BLACK);
    }
    if (!wps_.empty())
      DrawCircle(wps_[i-1].z,wps_[i-1].x, 4, BLACK);
  }
};

#endif // _PATH_HPP_
