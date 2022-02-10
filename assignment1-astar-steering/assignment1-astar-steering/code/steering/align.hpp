#ifndef _ALIGN_HPP_
#define _ALIGN_HPP_

#include <cmath> // std::abs

class Align
{
public:
  const Kinematic& character_;
  const Kinematic& target_;

  const float maxAngularAcceleration_;
  const float maxRotation_;

  const float targetRadius_;
  const float slowRadius_;

  const float timeToTarget_ = 0.1f;

  SteeringOutput getSteering() const
  {
    SteeringOutput result{}; // a zero value

    float rotation = target_.orientation_ - character_.orientation_;

    // mapToRange(rotation); (-pi,pi)
    rotation = std::abs(rotation) > PI ? rotation-2*PI : rotation;
    float rotationSize = std::abs(rotation);
    float targetRotation;

    if (rotationSize < targetRadius_)
      return result;

    if (rotationSize > slowRadius_)
      targetRotation = maxRotation_;
    else
      targetRotation = maxRotation_ * rotationSize / slowRadius_;

    targetRotation *= rotation / rotationSize;

    result.angular_ = targetRotation - character_.rotation_;
    result.angular_ /= timeToTarget_;

    float angularAcceleration = std::abs(result.angular_);
    if (angularAcceleration > maxAngularAcceleration_)
    {
      result.angular_ /= angularAcceleration;
      result.angular_ *= maxAngularAcceleration_;
    }

    result.linear_ = 0;
    return result;
  }
};

#endif // _ALIGN_HPP_
