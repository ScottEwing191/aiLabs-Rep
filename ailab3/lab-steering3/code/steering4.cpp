#include "vec.hpp"
#include "draw-triangle-pro.hpp"
#include "raylib-cpp.hpp"
#include <vector>
#include <cmath>  // std::atan, std::fmod, std::abs
#include <random> // std::random_device, std::mt19937
#include <algorithm> // std::clamp
#include <variant> // std::variant, std::visit

using Vector = ai::Vector3;  // use x and z in the 2D case

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
  friend SteeringOutput operator*(const float lhs, const SteeringOutput &y) {
    return {lhs*y.linear_, lhs*y.angular_};
  }
};

class Kinematic
{
public:
  Vector position_;
  float orientation_;
  Vector velocity_;
  float rotation_;

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
//    if (velocity_.length() > 0)
//      orientation_ = std::atan2(-velocity_.x, velocity_.z);

    velocity_ *= (1 - drag * time);
    rotation_ *= (1 - drag * time);

    if (velocity_.length() > maxSpeed)
    {
      velocity_.normalise();
      velocity_ *= maxSpeed;
    }
  }
};


class Ship
{
public:
  Ship(const float z, const float x, const float ori, const raylib::Color col)
    : k_{{x,0,z},ori,{0,0,0},0}, col_{col} { }

  Kinematic k_;
  raylib::Color col_;

  void draw(int screenwidth, int screenheight)
  {
    const float w = 10, len = 30; // ship width and length
    const ai::Vector2 l{0, -w}, r{0, w}, nose{len, 0};
    ai::Vector2 pos{k_.position_.z, k_.position_.x};
    float ori = -k_.orientation_ * RAD2DEG; // negate: anticlockwise rot

    ai::DrawTrianglePro(pos, l, r, nose, ori, col_);
  }
};

// Dynamic Seek (page 96)
class Seek
{
public:
  Kinematic& character_;
  Kinematic& target_;

  float maxAcceleration_;

  /* // A constructor isn't needed, but adding it will also not hurt
  Seek(Kinematic &c, Kinematic &t, float maxAcceleration)
    : character_{c}, target_{t}, maxAcceleration{maxAcceleration_}
  {
  }*/

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

class Align {
public:
	Kinematic& character_;
	Kinematic& target_;

	float maxAngularAcceleration_ = 1.0f;
	float maxRotation_ = 3.0f;

	float targetRadius_;				// the radius for arriving at the target
	float slowRadius_;				// the radius for beginning to slow down
	float timeToTarget_ = 0.1f;		// the time over which to achieve target speed

	SteeringOutput GetSteering() {
		SteeringOutput result{ {0,0,0},0 };

		//--Get the direction to the target
		float rotation = target_.orientation_ - character_.orientation_;

		//--Map the result to the (-360,360) interval
		//rotation = MapToRange(rotation);
		rotation = std::abs(rotation) > PI ? rotation - 2 * PI : rotation;
		float rotationSize = abs(rotation);

		//--Check if we are there, return no steering
		if (rotationSize < targetRadius_) {
			//result.angular = -result.angular;
			return result;
		}

		//--If we are outside the slow radius, then use max rotation
		float targetRotation{ 0 };
		if (rotation >= slowRadius_) {
			targetRotation = maxRotation_;
		}
		//--Otherwise calculate scaled rotation
		else {
			targetRotation = maxRotation_ * rotationSize / slowRadius_;
		}

		//--The final target rotation combines speed(already in the variable) and direction
		targetRotation *= rotation / rotationSize;

		//--Acceleration tries to get to the target rotation
		result.angular_ = targetRotation - character_.rotation_;
		result.angular_ /= timeToTarget_;

		//--Check if acceleration is too great
		float angularAcceleration = abs(result.angular_);

		if (angularAcceleration > maxAngularAcceleration_)
		{
			result.angular_ /= angularAcceleration;		// makes it 1
			result.angular_ *= maxAngularAcceleration_;
		}
		result.linear_ = 0;
		return result;
	}

	float MapToRange(float rotation) {
		float modRotation = fmod(rotation, 360);
		return modRotation;
		//return rotation * DEG2RAD;
	}


};

class Face : public Align {
public:
	Kinematic& target;

	SteeringOutput getSteering(){
		//--Calculate the target to delegate to align
		//--Work out the direction to target
		Vector direction = target.position_ - character_.position_;

		//--Check for zero direection, and make no change if so.
		if (direction.length() == 0)
		{
			//return target;
		}
	}
};

//--Wander
class Wander {
public:

};

int main(int argc, char *argv[])
{
  int w{1024}, h{768};
  raylib::Window window(w, h, "Game AI: Assignment 1");

  SetTargetFPS(60);

  Ship hunter{w/2.0f + 50, h/2.0f, 0, RED};
  Ship prey{w/2.0f + 250, h/2.0f + 300, 270*DEG2RAD, BLUE};

  float target_radius{5};
  float slow_radius{60};
  const float max_accel{200};
  const float max_ang_accel{10};
  const float max_speed{220};
  const float drag_factor{0.5};

  Seek seek{hunter.k_, prey.k_, 1000};

  //--Sounds Test
  raylib::AudioDevice audioDevice;
  raylib::Sound sound("../resources/weird.wav");        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


  while (!window.ShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      const auto mpos = GetMousePosition();
    }

    prey.draw(w,h);
    hunter.draw(w,h);

    EndDrawing();

    auto steer = seek.getSteering();
    hunter.k_.update(steer, max_speed, drag_factor, GetFrameTime());
    //prey.k_.update(steer2, max_speed-60, drag_factor, GetFrameTime());
  }

  return 0;
}
