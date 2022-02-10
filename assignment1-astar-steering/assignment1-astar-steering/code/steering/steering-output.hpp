#ifndef _STEERING_OUTPUT_HPP_
#define _STEERING_OUTPUT_HPP_

// These two values each represent an acceleration.
// i.e. they effect changes in velocity (linear and angular).
class SteeringOutput
{
public:
  Vector linear_;
  float angular_;

  SteeringOutput &operator+=(const SteeringOutput &rhs)
  {
    linear_ += rhs.linear_;
    angular_ += rhs.angular_;
    return *this;
  }
  friend auto operator*(const float lhs, const SteeringOutput &y) {
    return SteeringOutput{lhs*y.linear_, lhs*y.angular_};
  }
};

#endif // _STEERING_OUTPUT_HPP_
