/******************************************************************************/
/*!
\file	SpriteAnimation.h
\author Lim Chian Song
\par	email: 
\brief
Sprite Animation
*/
/******************************************************************************/
#pragma once
#include "Mesh.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <stdarg.h>

struct Animation
{
	//Constructor
	Animation():repeatCount(0),
		animTime(0.0f),
		ended(false),
		animActive(false)
	{
	}

	//repeat count, total animation time, 
	void Set(int repeat, float time, bool active)
	{
		this->repeatCount = repeat;
		this->animTime = time;
		this->animActive = active;
	}

	//Adding frames to the frame vector
	void AddFrame(int i)
	{
		frames.push_back(i);
	}

private:
	//name of the animation
	std::string animationName;

	//The frames
	std::vector<int> frames;

	//How many times to repeat
	int repeatCount;
	//The animation time
	float animTime;
	//Has the animation ended
	bool ended;
	//Is the animation active
	bool animActive;

	//Allow the Sprite Animation class to use private variable
	friend class SpriteAnimation;
};

//Sprite Animation that hold different classes
class SpriteAnimation : public Mesh
{
public:
	SpriteAnimation(const std::string& meshName, int row, int col);
	~SpriteAnimation();

	//Update the animated sprite
	void Update(double dt);
	void Render();

	void AddAnimation(std::string name, int start, int end);
	void AddSequeneAnimation(std::string name, int count ...);
	void PlayAnimation(std::string name, int repeat, float time);
	void Pause();
	void Resume();
	void Reset();

private:
	//number of rows
	int row;
	//number of columns 
	int col;

	//the current time of the animation
	float currentTime;
	//the current frame of the animation
	int currentFrame;
	//How many time it has play
	int playCount;

	//The name of the current animation
	std::string currentAnimation;
	//The list of the animations with their names
	std::unordered_map<std::string, Animation*> animationList;
};
