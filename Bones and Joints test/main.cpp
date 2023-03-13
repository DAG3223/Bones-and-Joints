#include <iostream>
#include <list>
#include "raylib.h"

class Joint;
class Bone;

class Joint {
public:
	Vector2 position;
	Bone* boneOrigin;
	float angle;
	float size;

	Joint(Vector2 pos, float ang, float radius, Bone* bone = nullptr) {
		position = pos;
		angle = ang;
		size = radius;
		boneOrigin = bone;
	}

	bool isJointClicked() {
		if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			return false;
		}
		if (!CheckCollisionPointCircle({ (float)GetMouseX(), (float)GetMouseY() }, position, size)) {
			return false;
		}
		return true;
	}

	void Draw() {
		DrawCircleV(position, size, BLACK);
	}
};

class Bone {
public:
	Joint* jointOrigin;
	Vector2 endpoint;
	float length;
	float angleOffset;

	Bone(Joint* origin, Vector2 end, float len, float offset) {
		jointOrigin = origin;
		length = len;
		endpoint = end;
		angleOffset = offset;
	}

	float previousAnglesSum(Bone bone) {
		if (bone.jointOrigin->boneOrigin == nullptr) {
			return bone.jointOrigin->angle + bone.angleOffset;
		}
		return (bone.jointOrigin->angle + bone.angleOffset) + previousAnglesSum(*(bone.jointOrigin->boneOrigin));
	}

	void setEndpoint() {
		float angle = previousAnglesSum(*this);
		
		float Y = PI / 2 - angle;
		float X = angle;

		float dY = this->length * sinf(Y);
		float dX = this->length * sinf(X);

		Vector2 endpoint = { this->jointOrigin->position.x + dX, this->jointOrigin->position.y + dY };
		this->endpoint = endpoint;
	}

	void Draw() {
		DrawLineV(jointOrigin->position, endpoint, WHITE);
	}
};

class Skeleton {
public:
	std::list<Joint> joints;
	std::list<Bone> bones;
};

int main() {
	InitWindow(800, 800, "Joints and Bones test");
	SetTargetFPS(60);

	//connections
	Joint joint1({ 400.0f, 400.0f }, 0, 4.0f);
	Bone bone1(&joint1, { 0,0 }, 100.0f, -PI / 4);

	Joint joint2(bone1.endpoint, 0, 4.0f, &bone1);
	Bone bone2(&joint2, { 0,0 }, 50.0f, PI / 2);

	Joint joint3(bone2.endpoint, 0, 4.0f, &bone2);
	Bone bone3(&joint3, { 0,0 }, 25.0f, -PI / 4);
	Bone bone4(&joint3, { 0,0 }, 25.0f, PI / 4);

	Joint* selectedJoint = nullptr;

	while (!WindowShouldClose()) {
		//LOGIC
		//select joint
		if (joint1.isJointClicked()) {
			selectedJoint = &joint1;
		} 
		else if (joint2.isJointClicked()) {
			selectedJoint = &joint2;
		}
		else if (joint3.isJointClicked()) {
			selectedJoint = &joint3;
		}

		if (selectedJoint != nullptr) {
			if (IsKeyDown(KEY_RIGHT)) {
				selectedJoint->angle += DEG2RAD * -2.0f;
			}
			if (IsKeyDown(KEY_LEFT)) {
				selectedJoint->angle += DEG2RAD * 2.0f;
			}
		}

		//updates endpoints for each bone (each bone updates independent from one another)
		bone1.setEndpoint();
		bone2.setEndpoint();
		bone3.setEndpoint();
		bone4.setEndpoint();

		//joint position updates (based on endpoint updates from joint angles and offsets)
		joint2.position = joint2.boneOrigin->endpoint;
		joint3.position = joint3.boneOrigin->endpoint;

		//DRAW
		BeginDrawing();
		ClearBackground(DARKBLUE);

		bone1.Draw();
		bone2.Draw();
		bone3.Draw();
		bone4.Draw();

		joint1.Draw();
		joint2.Draw();
		joint3.Draw();

		DrawFPS(0, 0);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}