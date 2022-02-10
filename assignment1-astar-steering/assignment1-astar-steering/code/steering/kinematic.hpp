#ifndef _KINEMATIC_HPP_
#define _KINEMATIC_HPP_

#include <cmath> // std::fmod

class Kinematic
{
public:
  Vector position_;
  float orientation_;
  Vector velocity_;
  float rotation_;

  Kinematic(const Vector& pos = {}, const float& ori = {},
            const Vector& vel = {}, const float& rot = {}) :
    position_{pos}, orientation_{ori}, velocity_{vel}, rotation_{rot} {}

  // integration of the linear and angular accelerations
  void update(const SteeringOutput& steering,
              const float maxSpeed,
              float drag,
              const float time) // delta time
  {
    //Newton-Euler 1 simplification:
    position_ += velocity_ * time;
    orientation_ += rotation_ * time;
    orientation_ = std::fmod(orientation_, 2*PI); // (-2pi,2pi) - not crucial?

    velocity_ += steering.linear_ * time;
    rotation_ += steering.angular_* time;

    post_process(drag, maxSpeed, time);
  }

  void post_process(const float drag, const float maxSpeed, const float time)
  {
    velocity_ *= (1 - drag * time);
    rotation_ *= (1 - drag * time);

    if (velocity_.length() > maxSpeed)
    {
      velocity_.normalise();
      velocity_ *= maxSpeed;
    }
  }
};

#endif // _KINEMATIC_HPP_
