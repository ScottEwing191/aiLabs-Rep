#include <iostream>
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

	SteeringOutput& operator+=(const SteeringOutput& rhs)
	{
		linear_ += rhs.linear_;
		angular_ += rhs.angular_;
		return *this;
	}
	friend SteeringOutput operator*(const float lhs, const SteeringOutput& y) {
		return { lhs * y.linear_, lhs * y.angular_ };
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
		orientation_ = std::fmod(orientation_, 2 * PI); // (-2pi,2pi) - not crucial?

		velocity_ += steering.linear_ * time;
		rotation_ += steering.angular_ * time;

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
		: k_{ {x,0,z},ori,{0,0,0},0 }, col_{ col } { }

	Kinematic k_;
	raylib::Color col_;

	void draw(int screenwidth, int screenheight)
	{
		const float w = 10, len = 30; // ship width and length
		const ai::Vector2 l{ 0, -w }, r{ 0, w }, nose{ len, 0 };

		//wrapPositions(k_.position_, screenwidth, screenheight);

		ai::Vector2 pos{ k_.position_.z, k_.position_.x };
		float ori = -k_.orientation_ * RAD2DEG; // negate: anticlockwise rot
		
												// wrap
		pos.x = std::fmod(pos.x, static_cast<float>(screenwidth));
		pos.y = std::fmod(pos.y, static_cast<float>(screenheight));
		pos.x = pos.x < 0 ? pos.x + screenwidth : pos.x;
		pos.y = pos.y < 0 ? pos.y + screenheight : pos.y;

		ai::DrawTrianglePro(pos, l, r, nose, ori, col_);
	}

	void wrapPositions(Vector position, int w, int h) {
		//--Height (X value)
		float xRemainder = std::fmod(k_.position_.x, h);
		if (xRemainder > 0)
			k_.position_.x = xRemainder;
		else if (k_.position_.x < 0)
			k_.position_.x = h;
		//--Width (Z value)
		float zRemainder = std::fmod(k_.position_.z, w);
		if (zRemainder > 0)
			k_.position_.z = zRemainder;
		else if (k_.position_.z < 0)
			k_.position_.z = w;
	}

	void goToRandomPosition(int screenWidth, int screenHeight) {
		k_.position_.x = GetRandomValue(0, screenHeight);
		k_.position_.z = GetRandomValue(0, screenWidth);
	}
};

//This is just used to display the target for the wander algorithm
class TargetShip
{
public:
	TargetShip(const float z, const float x, const float ori, const raylib::Color col)
		: k_{ {x,0,z},ori,{0,0,0},0 }, col_{ col } { }

	Kinematic k_;
	raylib::Color col_;

	void draw(int screenwidth, int screenheight)
	{
		const float w = 10, len = 30; // ship width and length
		const ai::Vector2 l{ 0, -w }, r{ 0, w }, nose{ len, 0 };
		ai::Vector2 pos{ k_.position_.z, k_.position_.x };
		float ori = -k_.orientation_ * RAD2DEG; // negate: anticlockwise rot
		ai::DrawTrianglePro(pos, l, r, nose, ori, col_);
	}
};

class SteeringBehaviour {
public:
	virtual SteeringOutput getSteering() {
		SteeringOutput result{};
		return result;
	}
};

// Dynamic Seek (page 96)
class Seek : public SteeringBehaviour 
{
public:
	Kinematic& character_;
	Kinematic& target_;

	float maxAcceleration_;

	// A constructor isn't needed, but adding it will also not hurt
	//--Not Working
	/*Seek(Kinematic &c, Kinematic &t, float maxAcceleration)
	  : character_{c}, target_{t}, maxAcceleration{maxAcceleration_}
	{
	}*/
	//--NEWER
	Seek(Kinematic& c, Kinematic& t, float maxAcceleration)
		: character_{ c }, target_{ t }
	{
		maxAcceleration_ = maxAcceleration;
	}

	//SteeringOutput getSteering() const		//original line
	SteeringOutput getSteering() override
	{
		SteeringOutput result;

		result.linear_ = target_.position_ - character_.position_;

		result.linear_.normalise();
		result.linear_ *= maxAcceleration_;

		result.angular_ = 0;
		return result;
	}
};



class Align : public SteeringBehaviour {
public:
	Kinematic& character_;
	Kinematic& target_;

	Align(Kinematic& c, Kinematic& t) : character_{ c }, target_{ t } {
	}
	float maxAngularAcceleration_ = 1.4f;
	float maxRotation_ = 2.0f;

	float targetRadius_ = 0.02f;				// the radius for arriving at the target
	float slowRadius_ = 1.6f;				// the radius for beginning to slow down
	float timeToTarget_ = 0.1f;		// the time over which to achieve target speed
	
	//virtual SteeringOutput getSteering() {
	SteeringOutput getSteering() override{
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

	Face(Kinematic& c, Kinematic& t) :Align{ c, t } {
	}

	SteeringOutput getSteering() override {
		//--Calculate the target to delegate to align
		//--Work out the direction to target
		Vector direction = Align::target_.position_ - character_.position_;

		//--Check for zero direection, and make no change if so.
		if (direction.length() == 0)
		{
			SteeringOutput steer{ {0,0,0},0 };
			return steer;
		}
		//--2. Delegate to align
		Align::target_.orientation_ = atan2(-direction.x, direction.z);
		return Align::getSteering();
	}
};

//--Wander
class Wander : public Face {
public:
	float wanderOffset_;			// The radius and forward offset of the ander circle
	float wanderRadius_;
	float wanderRate_;				// The maximum rate at which the wander orientation can change
	float wanderOrientation_;		// The current orientation of the wander target
	float maxAcceleration_;			// The maximum acceleration of the character

	//--Constructor
	Wander(Kinematic& c, Kinematic& t, float offset, float radius, float rate, float orientation, float maxAcceleration)
		: Face{ c,t }, wanderOffset_{ offset }, wanderRadius_{ radius }, wanderRate_{ rate }, wanderOrientation_{ orientation }, maxAcceleration_{ maxAcceleration } {
	}

	SteeringOutput getSteering() override {
		//--1.Calculate the target delegate to face
		//--Update the wander orientation
		wanderOrientation_ += randomBinomial() * wanderRate_;

		//--Calculate the combined target orientation
		float targetOrientation = wanderOrientation_ + character_.orientation_;

		//--Calculate the center of the wander circle
		Vector characterOrientationAsVector = asVector(character_.orientation_);
		Face::target_.position_ = character_.position_ + wanderOffset_ * characterOrientationAsVector;

		//--Calculate the target location
		Vector targetOrientationAsVector = asVector(targetOrientation);
		Face::target_.position_ += targetOrientationAsVector * wanderRadius_;

		//--2. Delegate to Face.
		SteeringOutput result = Face::getSteering();

		//--3. Now set the linear acceleration to be at full acceleration in the direaction of the orientation
		//characterOrientationAsVector = asVector(character_.orientation_);
		result.linear_ = maxAcceleration_ * asVector(character_.orientation_);

		return result;
	}

	Vector asVector(const float orientation) {
		return { -std::sin(orientation), 0, std::cos(orientation) };
	}

	float randomBinomial() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		return dis(gen) - dis(gen);
	}


	bool tryPlayCatchSound(Kinematic& hunter, Kinematic& prey, raylib::Sound* snd, float caughtDistance = 10.0f) {
		Vector fromHunterToPrey = prey.position_ - hunter.position_;
		float currentDistance = fromHunterToPrey.length();
		if (currentDistance < caughtDistance) {
			snd->Play();
			return true;
		}
		return false;
	}

};


class BlendedSteering {
public:
	class BehaviorAndWeight {
	public:
		SteeringBehaviour* behaviour_{};
		float weight_{};
	};

	//--The overall Maximum acceleration and rotation
	BehaviorAndWeight behaviours_[2];
	float maxAcceleration_{};
	float maxRotation_{};



	
	SteeringOutput getSteering() {
		SteeringOutput result{};
		//--Accumulate all acceleration
		for each (BehaviorAndWeight b in behaviours_)
		{
			result += b.weight_ * b.behaviour_->getSteering();
		}
		//--Crop the result and return
		//result.linear_ = std::max(result.linear_, maxAcceleration_);
		if (result.linear_.length() > maxAcceleration_)
		{
			result.linear_.normalise();
			result.linear_ *= maxAcceleration_;
		}
		//result.angular_ = std::max(result.angular_, maxRotation_);
		if (result.angular_ > maxRotation_)
		{
			result.angular_ = maxRotation_;
		}
		return result;
	}


};

int main(int argc, char* argv[])
{
	int w{ 1024 }, h{ 768 };
	raylib::Window window(w, h, "Game AI: Assignment 1");
	
	SetTargetFPS(60);

	Ship hunter{ w / 2.0f + 50, h / 2.0f, 0, RED };
	Ship prey{ w / 2.0f + 250, h / 2.0f + 300, 270 * DEG2RAD, BLUE };
	//Ship prey{ w / 2.0f + 250, h / 2.0f + 300, 0, BLUE };

	float target_radius{ 5 };
	float slow_radius{ 60 };
	const float max_accel{ 200 };
	const float max_ang_accel{ 10 };
	const float max_speed{ 220 };
	const float drag_factor{ 0.5 };

	Seek seek{ hunter.k_, prey.k_, 1000 };

	//Align align{ hunter.k_, prey.k_ };
	Face face{ hunter.k_, prey.k_ };


	BlendedSteering blend{ {{&seek,1.0f},{&face,1.0f}}, 1000, max_ang_accel };
	//blend.behaviours_[0].behaviour_ = &seek;
	//blend.behaviours_[0].weight_ = 1.0f;

	//blend.behaviours_[1].behaviour_ = &face;
	//blend.behaviours_[1].weight_ = 1.0f;

	//Kinematic wanderTarget{ {0,0,0} ,0, {0,0,0},0 };
	TargetShip wanderTarget{ w / 2.0f + 250, h / 2.0f + 220, 0, GREEN };
	//										offset, radius, rate, orientation, max acceleration
	Wander wander{ prey.k_, wanderTarget.k_, 80.0f, 30.0f , 0.3f , 0.0f , max_accel };

	//--Sounds Setup
	raylib::AudioDevice audioDevice;
	raylib::Sound sound("../resources/weird.wav");        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	while (!window.ShouldClose()) // Detect window close button or ESC key
	{
		BeginDrawing();

		ClearBackground(RAYWHITE);

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			const auto mpos = GetMousePosition();
			prey.k_.position_.x = mpos.y;
			prey.k_.position_.z = mpos.x;
			//prey.goToRandomPosition(w,h);

		}

		prey.draw(w, h);
		hunter.draw(w, h);
		wanderTarget.draw(w, h);


		EndDrawing();
		//--SteeringOutputs
		//auto steer = seek.getSteering();
		auto blendOutput = blend.getSteering();
		auto steer2 = wander.getSteering();


		//hunter.k_.update(steer, max_speed, drag_factor, GetFrameTime());
		hunter.k_.update(blendOutput, max_speed, drag_factor, GetFrameTime());

		prey.k_.update(steer2, max_speed - 60, drag_factor, GetFrameTime());

		//--Play sound once hunter catches prey and move prey to random position on screen
		if (wander.tryPlayCatchSound(hunter.k_, prey.k_, &sound, 10))	//-- if hunter catches prey
		{
			prey.goToRandomPosition(w, h);
		}



	}

	return 0;
}



