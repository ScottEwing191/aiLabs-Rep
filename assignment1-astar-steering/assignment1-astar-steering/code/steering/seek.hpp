#ifndef _SEEK_HPP_
#define _SEEK_HPP_

// Dynamic Seek (page 96)
class Seek
{
public:
  const Kinematic& character_;
  /*const*/ Kinematic& target_; // parent class may modify (e.g. FollowPath)

  const float maxAcceleration_;

  Seek(const Kinematic& c, Kinematic& t, const float ma)
    : character_{c}, target_{t}, maxAcceleration_{ma} { }

  SteeringOutput getSteering() const
  {
    SteeringOutput result;

    result.linear_ = target_.position_ - character_.position_;

    result.linear_.normalise();
    result.linear_ *= maxAcceleration_;

    result.angular_ = 0;
    return result;
  }
};

#endif // _SEEK_HPP_
