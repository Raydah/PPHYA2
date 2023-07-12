#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_BALL,
		GO_POWERUP,
		GO_CUBE,
		GO_WALL,
		GO_ARROW,
		GO_PILLAR,
		GO_TOTAL, //must be last
	};
	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	Vector3 normal;

	float angle;
	bool active;
	float mass;
	Vector3 color;

	//Rotation variables
	Vector3 direction;
	float momentOfInertia;
	float angularVelocity;

	GameObject* otherwall;
	bool visible;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();
};

#endif