#ifndef _ARRIVE_HPP_
#define _ARRIVE_HPP_

// Dynamic Arrive (page 99)
class Arrive
{
public:
  const Kinematic &character_;
  /*const*/ Kinematic &target_;

  float maxAcceleration_;
  float maxSpeed_; // used below, but not as a hard limit; that comes elsewhere

  float targetRadius_ = 5;
  float slowRadius_   = 60;

  float timeToTarget_ = 0.1f;

  Arrive(const Kinematic& c, Kinematic& t, const float ma, const float ms)
    : character_{c}, target_{t}, maxAcceleration_{ma}, maxSpeed_{ms} { }

  SteeringOutput getSteering() const
  {
    SteeringOutput result{}; // a zero value

    Vector direction = target_.position_ - character_.position_;
    float distance = direction.length();
    float targetSpeed;

    if (distance < targetRadius_)
      return result;

    if (distance > slowRadius_)
      targetSpeed = maxSpeed_;
    else
      targetSpeed = maxSpeed_ * distance / slowRadius_;

    Vector targetVelocity = direction;
    targetVelocity.normalise();
    targetVelocity *= targetSpeed;

    result.linear_ = targetVelocity - character_.velocity_;
    result.linear_ /= timeToTarget_;

    if (result.linear_.length() > maxAcceleration_)
    {
      result.linear_.normalise();
      result.linear_ *= maxAcceleration_;
    }

    result.angular_ = 0;
    return result;
  }
};

#endif // _ARRIVE_HPP_
