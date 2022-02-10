#ifndef _BLENDED_STEERING_HPP_
#define _BLENDED_STEERING_HPP_

#include <vector>  // std::vector
#include <variant> // std::variant, std::visit
#include <cmath>   // std::abs

class BlendedSteering
{
public:
  class BehaviourAndWeight
  {
  public:
    using SteeringBehaviour = std::variant<FollowPath,Align>;
    SteeringBehaviour behaviour_;
    float weight_;
  };

  std::vector<BehaviourAndWeight> behaviours_;

  const float maxAcceleration_;
  const float maxAngularAcceleration_;

  SteeringOutput getSteering() /*const*/
  {
    SteeringOutput result{};

    for (auto &b : behaviours_)
      result +=
        b.weight_ *
        std::visit([](auto &b) { return b.getSteering(); }, b.behaviour_);

    if (result.linear_.length() > maxAcceleration_)
    {
      result.linear_.normalise();
      result.linear_ *= maxAcceleration_;
    }

    float angularAcceleration = std::abs(result.angular_);
    if (angularAcceleration > maxAngularAcceleration_)
    {
      result.angular_ /= angularAcceleration;
      result.angular_ *= maxAngularAcceleration_;
    }

    return result;
  }
};

#endif // _BLENDED_STEERING_HPP_
