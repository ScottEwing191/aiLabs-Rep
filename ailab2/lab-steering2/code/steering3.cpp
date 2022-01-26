#include <iostream>
#include "vec.hpp"
#include "draw-triangle-pro.hpp"
#include "raylib-cpp.hpp"

using Vector = ai::Vector3;

// The scene drawn shows positive x downwards and positive z to the right:
// ....> Z (+ve)
// .
// .
// .
// X (+ve)

class SteeringOutput
{
public:
	Vector linear_;
	float angular_;
};

class Kinematic
{
public:
	Vector position_;
	float orientation_{};
	Vector velocity_{ 0,0,0 };
	float rotation_ = 0;

	/*Kinematic() {

	}*/

	void Update(const SteeringOutput steering, float maxSpeed, float time) {
		//--Update the position and orientation
		position_ += velocity_ * time;
		orientation_ += rotation_ * time;

		//--and the velocity and rotation
		velocity_ += steering.linear_ * time;
		rotation_ += steering.angular_ * time;

		//--Check for speeding and clip
		if (velocity_.length() > maxSpeed)
		{
			velocity_.normalise();
			velocity_ *= maxSpeed;
		}
	}
};

class Seek
{
public:
	//--I tried making these references but but got "E1790 - the default contructor of "Seek" cannot be referenced -- it is a deleted function"
	//--when trying to create seek object in main method. 
	Kinematic& character_;									//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Kinematic& target_;
	float maxAcceleration_;

	Seek() = default;

	Seek(Kinematic& character, Kinematic& target, float maxAcceleration) : character_{ character }, target_{ target }, maxAcceleration_{ maxAcceleration }	{ }

	SteeringOutput GetSteering() {
		SteeringOutput result;

		//--Get the direction to the target
		result.linear_ = target_.position_ - character_.position_;

		//--Give the full acceleration along this direction
		result.linear_.normalise();
		result.linear_ *= maxAcceleration_;

		result.angular_ = 0;
		return result;
	}
};

class Ship : public Kinematic {
public:
	ai::Vector2 l, r, nose;
	ai::Vector2 pos;
	raylib::Color col = RED;

	Ship(Vector position, ai::Vector2 l, ai::Vector2 r, ai::Vector2 nose, float rotation, raylib::Color col) {
		this->position_ = position;
		this->l = l;
		this->r = r;
		this->nose = nose;
		this->rotation_ = rotation;
		this->col = col;
	}

	void Draw() {
		pos.x = position_.x;
		pos.y = position_.z;
		ai::DrawTrianglePro(pos, l, r, nose, orientation_, col);
	}
};

class Arrive {
public:
	Kinematic& character_;
	Kinematic& target_;

	float maxAcceleration_;
	float maxSpeed_;

	float targetRadius_;				// the radius for arriving at the target
	float slowRadius_;				// the radius for beginning to slow down
	float timeToTarget_ = 0.1f;		// the time over which to achieve target speed

	SteeringOutput GetSteering() {
		SteeringOutput result{};

		//--Get the direction to the target
		Vector direction = target_.position_ - character_.position_;
		float distance = direction.length();

		//--Check if we are there, return no steering
		if (distance <= targetRadius_) {
			result.linear_ = { -character_.velocity_.x, 0, -character_.velocity_.z };
			return result;
		}

		//--If we are outside the slow radius then move at max speed
		float targetSpeed{};			// using name from pseudocode p.99 ln.30, doesn't really make sende to me (characterSpeed better name?)
		if (distance > slowRadius_)
			targetSpeed = maxSpeed_;
		//--Otherwise calculate scaled speed
		else
			targetSpeed = maxSpeed_ * distance / slowRadius_;

		//--The target velocity combines speed and direction
		Vector targetVelocity = direction;
		targetVelocity.normalise();
		targetVelocity *= targetSpeed;

		//--Acceleration tries to get to the speed and velocity
		result.linear_ = targetVelocity - character_.velocity_;
		result.linear_ /= timeToTarget_;

		//--Check if acceleration is too fast
		if (result.linear_.length() > maxAcceleration_)
		{
			result.linear_.normalise();
			result.linear_ *= maxAcceleration_;
		}
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
		float targetRotation{0};
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

int main(int argc, char* argv[])
{
	float sw{ 1024 }, sh{ 768 };
	raylib::Window window(sw, sh, "Steering 3");

	SetTargetFPS(60);

	//--Declare Ships
	//--also works and doesnt require contructor
	//Ship ship{ Vector{ sw / 2,0,sh / 2 }, ai::Vector2{ 0,-10 }, ai::Vector2{ 0,10 }, ai::Vector2{ 30,0 }, 0, BLUE };		

	Ship* ship = new Ship({ sw / 2,0,sh / 2 }, { 0,-10 }, { 0,10 }, { 30,0 }, 0, BLUE);
	Ship* enemy = new Ship({ sw / 2,0,sh / 2 - 200 }, { 0,-10 }, { 0,10 }, { 30,0 }, 0, RED);


	//--Declare Seek
	//Seek seekEnemy{ *ship, *enemy, 1.0f };			//not tested but should be fine and means constructor not required
	Seek* seekEnemy = new Seek(*ship, *enemy, 1.0f);
	Seek* seekShip = new Seek(*enemy, *ship, 1.0f);

	//--Declare Arrive
	//Arrive arriveEnemy = *(new Arrive(ship, enemy, 140, 200, 10, 200));		// requires contructor
	//Arrive arriveShip = *(new Arrive(enemy, ship, 70, 100, 10, 100));
	Arrive arriveEnemy{ *ship, *enemy, 140, 200, 10, 200 };						// doesnt require constructor
	Arrive arriveShip{ *enemy, *ship, 70, 100, 10, 100 };

	//				maxAngularAcceleration, maxRotation, targetRadius, slowRadius
	Align alignEnemy{ *ship, *enemy, 80.0f, 80.0f, 1.0f, 90.0f };
	//Align alignEnemy{ *ship, *enemy, 10.0f, 1.0f, 0.01f, 0.1f };


	while (!window.ShouldClose()) // Detect window close button or ESC key
	{
		BeginDrawing();

		ClearBackground(RAYWHITE);

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			const auto mpos = GetMousePosition();
			Vector mposV3{ mpos.x, 0, mpos.y };
			enemy->position_ = mposV3;
			enemy->orientation_ = rand() % 360;
		}

		enemy->Draw();
		ship->Draw();
		//ai::DrawTrianglePro({ sw / 2,sh / 2 }, { 0,-10 }, { 0,10 }, { 30,0 }, 0, BLUE);

		EndDrawing();

		//ship->Update(arriveEnemy.GetSteering(), 200.0f, GetFrameTime());
		ship->Update(alignEnemy.GetSteering(), 100.0f, GetFrameTime());

		
		//enemy->Update(arriveShip.GetSteering(), 100.0f, GetFrameTime());


	}

	return 0;
}
