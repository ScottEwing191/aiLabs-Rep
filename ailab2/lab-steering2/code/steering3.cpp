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
	Vector linear;
	float angular;
};

class Kinematic
{
public:
	Vector position;
	float orientation;
	Vector velocity{ 0,0,0 };
	float rotation = 0;

	/*Kinematic() {

	}*/

	void Update(const SteeringOutput steering, float maxSpeed, float time) {
		//--Update the position and orientation
		position += velocity * time;
		orientation += rotation * time;

		//--and the velocity and rotation
		velocity += steering.linear * time;
		rotation += steering.angular * time;

		//--Check for speeding and clip
		if (velocity.length() > maxSpeed)
		{
			velocity.normalise();
			velocity *= maxSpeed;
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
		result.linear = target_.position - character_.position;

		//--Give the full acceleration along this direction
		result.linear.normalise();
		result.linear *= maxAcceleration_;

		result.angular = 0;
		return result;
	}
};

class Ship : public Kinematic {
public:
	ai::Vector2 l, r, nose;
	ai::Vector2 pos;
	//float rotation;
	raylib::Color col = RED;

	/*Ship() {

	}*/

	Ship(Vector position, ai::Vector2 l, ai::Vector2 r, ai::Vector2 nose, float rotation, raylib::Color col) {
		this->position = position;
		this->l = l;
		this->r = r;
		this->nose = nose;
		this->rotation = rotation;
		this->col = col;
	}

	void Draw() {
		pos.x = position.x;
		pos.y = position.z;
		ai::DrawTrianglePro(pos, l, r, nose, rotation, col);		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!should rotation be orientation

	}
};

class Arrive {
public:
	Kinematic* character;
	Kinematic* target;

	float maxAcceleration;
	float maxSpeed;


	float targetRadius;				// the radius for arriving at the target
	float slowRadius;				// the radius for beginning to slow down
	float timeToTarget = 0.1f;		// the time over which to achieve target speed

	Arrive(Kinematic* character, Kinematic* target, float maxAcceleration, float maxSpeed, float targetRadius, float slowRadius) {
		this->character = character;
		this->target = target;
		this->maxAcceleration = maxAcceleration;
		this->maxSpeed = maxSpeed;
		this->targetRadius = targetRadius;
		this->slowRadius = slowRadius;
	}

	SteeringOutput GetSteering() {
		SteeringOutput result{};

		//--Get the direction to the target
		Vector direction = target->position - character->position;
		float distance = direction.length();

		//--Check if we are there, return no steering
		if (distance <= targetRadius) {
			//result.linear = { 0,0,0 };
			//result.linear = { -character->velocity.x, 0, -character->velocity.z };		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
			//result.angular = 0;
			return result;
			//return __null;				// this is probably wrong
		}

		//--If we are outside the slow radius then move at max speed
		float targetSpeed{};			// using name from pseudocode p.99 ln.30, doesn't really make sende to me (characterSpeed better name?)
		if (distance > slowRadius)
			targetSpeed = maxSpeed;
		//--Otherwise calculate scaled speed
		else
			targetSpeed = maxSpeed * distance / slowRadius;

		//--The target velocity combines speed and direction
		Vector targetVelocity = direction;
		targetVelocity.normalise();
		targetVelocity *= targetSpeed;

		//--Acceleration tries to get to the speed and velocity
		result.linear = targetVelocity - character->velocity;
		result.linear /= timeToTarget;

		//--Check if acceleration is too fast
		if (result.linear.length() > maxAcceleration)
		{
			result.linear.normalise();
			result.linear *= maxAcceleration;
		}
		result.angular = 0;
		return result;
	}
};

class Align {
public:
	Kinematic& character_;
	Kinematic& target_;

	float maxAngularAcceleration_ = 1.0f;
	float maxRotation_ = 3.0f;

	float targetRadius;				// the radius for arriving at the target
	float slowRadius;				// the radius for beginning to slow down
	float timeToTarget = 0.1f;		// the time over which to achieve target speed

	SteeringOutput GetSteering() {
		SteeringOutput result;

		//--Get the direction to the target
		float rotation = target_.orientation - character_.orientation;

		//Map the resut to the (-pi,pi) interval
		rotation = MapToRange(rotation);
		float rotationSize = abs(rotation);

		return result;
	}

	float MapToRange(float rotation) {						//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		float modRotation   = fmod(rotation, 360);
		float rotationRad = rotation * DEG2RAD;
	}

	
};

int main(int argc, char* argv[])
{
	float sw{ 1024 }, sh{ 768 };
	raylib::Window window(sw, sh, "Steering 3");

	SetTargetFPS(60);

	//--Declare Ships
	Ship* ship = new Ship({ sw / 2,0,sh / 2 }, { 0,-10 }, { 0,10 }, { 30,0 }, 0, BLUE);
	//Ship ship2({ sw / 2,0,sh / 2 }, { 0,-10 }, { 0,10 }, { 30,0 }, 0, BLUE);
	Ship* enemy = new Ship({ sw / 2,0,sh / 2 - 200 }, { 0,-10 }, { 0,10 }, { 30,0 }, 0, RED);


	//--Declare Seek
	Seek* seekEnemy = new Seek(*ship, *enemy, 1.0f);
	Seek* seekShip = new Seek(*enemy, *ship, 1.0f);

	//--Declare Arrive
	Arrive arriveEnemy = *(new Arrive(ship, enemy, 140, 200, 10, 200));
	Arrive arriveShip = *(new Arrive(enemy, ship, 70, 100, 10, 100));

	while (!window.ShouldClose()) // Detect window close button or ESC key
	{
		BeginDrawing();

		ClearBackground(RAYWHITE);

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			const auto mpos = GetMousePosition();
			Vector mposV3{ mpos.x, 0, mpos.y };
			enemy->position = mposV3;
		}

		enemy->Draw();
		ship->Draw();
		//ai::DrawTrianglePro({ sw / 2,sh / 2 }, { 0,-10 }, { 0,10 }, { 30,0 }, 0, BLUE);

		EndDrawing();

		ship->Update(arriveEnemy.GetSteering(), 200.0f, GetFrameTime());
		//enemy->Update(arriveShip.GetSteering(), 100.0f, GetFrameTime());

		
	}

	return 0;
}
