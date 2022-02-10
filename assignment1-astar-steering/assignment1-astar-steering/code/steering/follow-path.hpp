#ifndef _FOLLOW_PATH_HPP_
#define _FOLLOW_PATH_HPP_

#include <algorithm> // std::min
#include <cmath>     // std::atan2

class FollowPath : public Arrive
{
public:

  Path path_;
  const float pathOffset_;  // 0-1
  float currentParam_{};    // 0-1 (initialise in the ctor?)

  FollowPath(const Kinematic& c, Kinematic& t,
             const Path& p, const float po, const float ma, const float ms)
    : Arrive{c, t, ma, ms}, path_{p}, pathOffset_{po} {}

  SteeringOutput getSteering() /*const*/
  {
    currentParam_ = path_.getParam(character_.position_, currentParam_);

    float targetParam = currentParam_ + pathOffset_;
    targetParam = std::min(targetParam, 1.0f);

    target_.position_ = path_.getPosition(targetParam);

    // Set the target's orientation as well as its position
    Vector v = target_.position_ - character_.position_;
    if (v.length() > 0)
      target_.orientation_ = std::atan2(-v.x, v.z);

    DrawCircleLines(target_.position_.z,target_.position_.x, 8, RED);

    return Arrive::getSteering();
  }
};

#endif // _FOLLOW_PATH_HPP_
