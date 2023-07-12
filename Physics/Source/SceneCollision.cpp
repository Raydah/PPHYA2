#include "SceneCollision.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneCollision::SceneCollision()
{
}

SceneCollision::~SceneCollision()
{
}

void SceneCollision::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;
	
	Math::InitRNG();

	//Exercise 1: initialize m_objectCount
	m_objectCount = 0;
	bLightEnabled = true;
	
	m_ghost = new GameObject(GameObject::GO_BALL);
	

	float angle = Math::QUARTER_PI;
	float wallLength = 30;
	float radius = wallLength * 0.5f / tan(angle * 0.5f);

	//Create Octangonal shape
	for (int i = 0; i < 8; ++i)
	{
		GameObject* go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->scale.Set(2.0f, wallLength + 0.9f, 1.f);
		go->pos = Vector3(radius * cosf(i * angle) + m_worldWidth / 2,
			radius * sinf(i * angle) + m_worldHeight / 2, 0.0f);
		go->normal = Vector3(cosf(i * angle), sinf(i * angle), 0.f);
		go->color.Set(0, 0, 1);
		go->vel.SetZero();

		MakeThickWall(20, 40, Vector3(cosf(Math::QUARTER_PI), sinf(Math::QUARTER_PI), 0), Vector3(m_worldWidth / 2, m_worldHeight / 2, 0));
	}
}

GameObject* SceneCollision::FetchGO()
{
	//Exercise 2a: implement FetchGO()
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
			continue;
		go->active = true;
		go->visible = true;
		go->otherwall = nullptr;
		++m_objectCount;
		return go;
	}
	//Exercise 2b: increase object count every time an object is set to active

	int prevSize = m_goList.size();
	for (int i = 0;i < 10;++i)
	{
		m_goList.push_back(new GameObject(GameObject::GO_BALL));
	}
	m_goList.at(prevSize)->active = true;
	m_goList.at(prevSize)->visible = true;
	m_goList.at(prevSize)->otherwall = nullptr;
	++m_objectCount;

	return m_goList[prevSize];
}

void SceneCollision::ReturnGO(GameObject *go)
{
	//Exercise 3: implement ReturnGO()
	if (go->active)
	{
		go->active = false;
		++m_objectCount;
	}
}

void SceneCollision::Update(double dt)
{
	SceneBase::Update(dt);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	
	if(Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if(Application::IsKeyPressed('0'))
	{
		m_speed += 0.1f;
	}

	//Mouse Section
	double x, y, windowWidth, windowHeight;
	Application::GetCursorPos(&x, &y);
	windowWidth = Application::GetWindowWidth();
	windowHeight = Application::GetWindowWidth();
	Vector3 mousePos(x * (m_worldWidth / windowWidth), (windowHeight - y) * (m_worldHeight / windowHeight), 0);
	
	static bool bLButtonState = false;
	if(!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
		m_ghost->active = true;
		m_ghost->pos = mousePos;
		//m_ghost->pos.y = m_worldHeight / 2.0f;
		m_ghost->scale.Set(2, 2, 2);
		m_ghost->mass = 8;  
		m_ghost->color.Set(Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1));


	}
	else if(bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;

		//Exercise 6: spawn small GO_BALL
		GameObject* go = FetchGO();
		 
		go->type = GameObject::GO_BALL;
		go->pos = m_ghost->pos;
		

		go->vel = m_ghost->pos-mousePos;
		//go->vel.y = 0;
		
		go->scale = m_ghost->scale;
		go->mass = m_ghost->mass;

		go->color = m_ghost->color;
		m_ghost->active = false;
	}
	static bool bRButtonState = false;
	if(!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;

		m_ghost->active = true;
		m_ghost->pos = mousePos;
		//m_ghost->pos.y = m_worldHeight / 2.0f;
		m_ghost->scale.Set(3, 3, 3);
		m_ghost->mass = 27;
		m_ghost->color.Set(Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1));

	}
	else if(bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;

		//Exercise 10: spawn large GO_BALL
		GameObject* go = FetchGO();

		go->type = GameObject::GO_BALL;

		go->pos = m_ghost->pos;


		go->vel = m_ghost->pos - mousePos;
		//go->vel.y = 0;

		go->scale = m_ghost->scale;
		go->mass = m_ghost->mass;
		go->color = m_ghost->color;
		m_ghost->active = false;
	}

	//Physics Simulation Section
	unsigned size = m_goList.size();
	for(unsigned i = 0; i<size;++i)
	{
		GameObject *go = m_goList[i];
		if(go->active)
		{
			//Exercise 7a: implement movement for game objects
			go->pos += go->vel * dt * m_speed;

			//Exercise 7b: handle out of bound game objects
			//Handle X-Axis bounds
			if (((go->pos.x - go->scale.x < 0) && go->vel.x < 0) ||
				((go->pos.x + go->scale.x > m_worldWidth) && go->vel.x > 0))
			{
				go->vel.x = -go->vel.x;
			}

			if (go->pos.x<0 - go->scale.x || go->pos.x > m_worldWidth + go->scale.x)
			{
				ReturnGO(go);
				continue;
			}

			//Handle Y-Axis bounds
			if (((go->pos.y - go->scale.y <0) && go->vel.y < 0) || 
				((go->pos.y + go->scale.y > m_worldHeight) && go->vel.y>0))
			{
				go->vel.y = -go->vel.y;
			}

			if (go->pos.y <0 - go->scale.y || go->pos.y > m_worldHeight + go->scale.y)
			{
				ReturnGO(go);
				continue;
			}

			GameObject* go2 = nullptr;
			for (int j = i+1; j < size;++j)
			{
				go2 = m_goList[j];
				GameObject* actor(go);
				GameObject* actee(go2);

				if (go->type != GameObject::GO_BALL)
				{
					actor = go2;
					actee = go;
				}
				if (go2->active && CheckCollision(actor, actee))
				{
					CollisionResponse(actor, actee);
				}
			}
			//Exercise 8a: handle collision between GO_BALL and GO_BALL using velocity swap

			//Exercise 8b: store values in auditing variables

			//Exercise 10: handle collision using momentum swap instead

			//Exercise 12: improve inner loop to prevent double collision

			//Exercise 13: improve collision detection algorithm 
		}
	}
}

void SceneCollision::MakeThinWall(float width, float height, const Vector3 normal, const Vector3& pos)
{
	GameObject* mid = FetchGO();
	mid->type = GameObject::GO_WALL;
	mid->scale.Set(width, height, 1.0f);
	mid->pos=pos;
	mid->normal=normal;
	mid->color.Set(1, 0, 0);
	mid->vel.SetZero();

	Vector3 tangent(-normal.y, normal.x);
	//Pillar 1
	GameObject* pillar = FetchGO();
	pillar->type = GameObject::GO_PILLAR;
	pillar->color.Set(1, 0, 0);
	pillar->scale.Set(width * 0.5, width * 0.5, 1);
	pillar->pos = pos + height * 0.5f * tangent;

	//pillar 2
	pillar = FetchGO();
	pillar->type = GameObject::GO_PILLAR;
	pillar->color.Set(1, 0, 0);
	pillar->scale.Set(width * 0.5, width * 0.5, 1);
	pillar->pos = pos - height * 0.5f * tangent;
}

void SceneCollision::MakeThickWall(float width, float height, const Vector3 normal, const Vector3& pos)
{
	Vector3 tangent(-normal.y, normal.x);

	float size = 0.1f;

	//4 Pillars
	GameObject* pillar = FetchGO();
	pillar->type = GameObject::GO_PILLAR;
	pillar->color.Set(1, 0, 0);
	pillar->scale.Set(size, size, 1);
	pillar->pos = pos + height * 0.5f * tangent + width * 0.5f * normal;

	pillar = FetchGO();
	pillar->type = GameObject::GO_PILLAR;
	pillar->color.Set(1, 0, 0);
	pillar->scale.Set(size, size, 1);
	pillar->pos = pos + height * 0.5f * tangent - width * 0.5f * normal;

	pillar = FetchGO();
	pillar->type = GameObject::GO_PILLAR;
	pillar->color.Set(1, 0, 0);
	pillar->scale.Set(size, size, 1);
	pillar->pos = pos - height * 0.5f * tangent - width * 0.5f * normal;

	pillar = FetchGO();
	pillar->type = GameObject::GO_PILLAR;
	pillar->color.Set(1, 0, 0);
	pillar->scale.Set(size, size, 1);
	pillar->pos = pos - height * 0.5f * tangent + width * 0.5f * normal;
	
	GameObject* wall1 = FetchGO();
	wall1->type = GameObject::GO_WALL;
	wall1->scale.Set(width, height, 1.0f);
	wall1->normal = normal;
	wall1->color.Set(1, 0, 0);
	wall1->pos = pos;


	GameObject* wall2 = FetchGO();
	wall2->type = GameObject::GO_WALL;
	wall2->scale.Set(height, width, 1.f);
	wall2->normal = tangent;
	wall2->color.Set(1, 0, 0);
	wall2->pos = pos;
	wall2->visible = false;

	wall1->otherwall = wall2;
	wall2->otherwall = wall1;
}

bool SceneCollision::CheckCollision(GameObject* go1, GameObject* go2)
{	
	//Prevent non ball vs non ball code
	if (go1->type != GameObject::GO_BALL)
	{
		return false;
	}

	//Collision Detection
	switch (go2->type)
	{
		case GameObject::GO_PILLAR:

		case GameObject::GO_BALL: //Ball-Ball
		{
			Vector3 relativeVel = go1->vel - go2->vel;
			Vector3 disDiff = go2->pos - go1->pos;

			if (relativeVel.Dot(disDiff) <= 0)
				return false;

			return disDiff.LengthSquared() <= (go1->scale.x + go2->scale.x) * (go1->scale.x + go2->scale.x);
		}
		case GameObject::GO_WALL: 
		{
			Vector3 diff = go1->pos - go2->pos;
			Vector3 axisX = go2->normal;
			Vector3 axisY = Vector3(-go2->normal.y, go2->normal.x, 0);

			float projectDist = diff.Dot(axisX);

			//If it is a think wall
			if (go2->otherwall != nullptr)
			{
				if (Math::FAbs(projectDist) / go2->scale.x < Math::FAbs(diff.Dot(axisY)) / go2->otherwall->scale.x)
				{
					return false;
				}
			}

			if (projectDist > 0)
			{
				axisX = -axisX;
			}

			return go1->vel.Dot(axisX) >= 0 && //Check 1: Travelling towards the wall
				go2->scale.x*0.5f+go1->scale.x>-diff.Dot(axisX) && //Check 2: Radius + Thickness vs Distance
				go2->scale.y * 0.5 > fabs(diff.Dot(axisY));//Check 3: Length Check
		}

	}

}

void SceneCollision::CollisionResponse(GameObject* go1, GameObject* go2)
{	
	//Swap Velocity
	/*Vector3 temp = go1->vel;
	go1->vel = go2->vel;
	go2->vel = temp;*/

	u1 = go1->vel;
	u2 = go2->vel;
	m1 = go1->mass;
	m2 = go2->mass;

	//MOM Swap
	/*Vector3 mom1 = u1 * m1;
	Vector3 mom2 = u2 * m2;
	go1->vel.Set(mom2.x / m1, mom2.y / m1, 0);
	go1->vel.Set(mom1.x / m2, mom1.y / m2, 0);*/

	//1D Elastic Collision
	//go1->vel = (m1 - m2) / (m2 + m1) * u1 + (2 * m2) / (m2 + m1) * u2;
	//go2->vel = (2 * m1) / (m2 + m1) * u1 + (m2 - m1) / (m2 + m1)* u2;
	
	////2D Elastic Collision
	////Step 1:Find n, un and ut
	//Vector3 n = go1->pos - go2->pos;
	//Vector3 un = n.Normalize();
	//Vector3 ut = Vector3(un.y, un.x, 0);

	//v1 = go1->vel;
	//v2 = go2->vel;

	////Step 3: find c1n,v2n,v1t,v2t
	//float v1n = un.Dot(u1);
	//float v1t = un.Dot(u1);
	//float v2n = un.Dot(u2);
	//float v2t = ut.Dot(u2);

	////Step 5: Find new normal Velocity
	//float v1np = (m1 - m2) / (m2 + m1) * v1n + (2 * m2) / (m2 + m1) * v2n;
	//float v2np = (2 * m1) / (m2 + m1) * v1n + (m2 - m1) / (m2 + m1)* v2n;
	//
	////Step 6: Convert 
	//Vector3 v1nVec = v1np * un;
	//Vector3 v1tVec = v1t * ut;
	//Vector3 v2nVec = v2np * un;
	//Vector3 v2tVec = v2t * ut;

	////Step 7
	//go1->vel = v1nVec + v1tVec;
	//go2->vel = v2nVec + v2tVec;	

	switch (go2->type)
	{
		case GameObject::GO_BALL:
		{
			//2D Version 2
			Vector3 n = go1->pos - go2->pos;
			Vector3 vec = (u1 - u2).Dot(n) / (n).LengthSquared() * n;
			go1->vel = u1 - (2 * m2 / (m1 + m2)) * vec;
			go2->vel = u2 - (2 * m1 / (m2 + m1)) * -vec;

			break;
		}
		case GameObject::GO_WALL:
		{
			go1->vel = u1 - (2.0 * u1.Dot(go2->normal)) * go2->normal;
			break;
		}
		case GameObject::GO_PILLAR:
		{
			Vector3 n = (go2->pos - go1->pos).Normalize();
			go1->vel = u1 - (2.0 * u1.Dot(n)) * n;
			break;
		}
	}

}

void SceneCollision::RenderGO(GameObject *go)
{
	switch(go->type)
	{
	case GameObject::GO_PILLAR:
	case GameObject::GO_BALL:
		//Exercise 4: render a sphere using scale and pos
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_BALL]->material.kAmbient.Set(go->color.x, go->color.y, go->color.z);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();
		//Exercise 11: think of a way to give balls different colors
		break;
	case GameObject::GO_WALL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_CUBE]->material.kAmbient.Set(go->color.x, go->color.y, go->color.z);
		RenderMesh(meshList[GEO_CUBE], true);
		modelStack.PopMatrix();
		break;
	}
}

void SceneCollision::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);
	
	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
						camera.position.x, camera.position.y, camera.position.z,
						camera.target.x, camera.target.y, camera.target.z,
						camera.up.x, camera.up.y, camera.up.z
					);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();
	
	RenderMesh(meshList[GEO_AXES], false);
	if (m_ghost->active)
	{
		RenderGO(m_ghost); 
	}
	
	
	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
		}
	}

	//On screen text
	std::ostringstream ss;

	//Exercise 5: Render m_objectCount
	RenderTextOnScreen(meshList[GEO_TEXT], "Object Count:"+std::to_string(m_objectCount), Color(0, 1, 0), 3, 0, 9);

	//Exercise 8c: Render initial and final momentum

	ss.precision(3);
	ss << "Speed: " << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 6);
	
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);
	
	RenderTextOnScreen(meshList[GEO_TEXT], "Collision", Color(0, 1, 0), 3, 0, 0);
}

void SceneCollision::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ghost)
	{
		delete m_ghost;
		m_ghost = NULL;
	}
}
