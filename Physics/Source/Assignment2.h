#ifndef ASSIGNMENT_2_H
#define ASSIGNMENT_2_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class Assignment2 : public SceneBase
{
public:
	Assignment2();
	~Assignment2();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	GameObject* FetchGO();
	void ReturnGO(GameObject *go);

	bool CheckCollision(GameObject* go1, GameObject* go2);
	void CollisionResponse(GameObject*go1, GameObject*go2);
	
	void MakeThinWall(float width, float height, const Vector3 normal, const Vector3& pos);
	void MakeThickWall(float width, float height, const Vector3 normal, const Vector3& pos);

protected:

	//Physics
	std::vector<GameObject *> m_goList;
	std::vector<GameObject *> m_wallList;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ghost;
	int m_objectCount;
	int ballcount =10;
	int score = 0, previousscore = 0, highestscore = 0;

	//Auditing
	float m1, m2;
	Vector3 u1, u2, v1, v2;
};

#endif