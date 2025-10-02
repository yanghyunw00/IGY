

#pragma once
#include <raylib.h>
#include <raymath.h>

class Mode7 {
public:
	Mode7(Texture2D texture) : texMap(texture)
	{
		texSky = LoadTexture("Assets/Sky.png");

		imgMap = LoadImageFromTexture(texMap);

	}
	void Load();
	void Update();
	void Draw();
	void unload();
	
private:
	

	
	struct Frustum
	{
		Vector2 Far1;
		Vector2 Far2;
		Vector2 Near1;
		Vector2 Near2;
	};
	float fRotationSpeed = 0.0f;
	float fRotationAccel = 6.0f;
	float fRotationDamping = 2.0f;
	float fMaxRotationSpeed = 1.8f;
	float fWorldX = 0.0f;
	float fWorldY = 0.0f;
	float fWorldA = 0.0f;
	float fNear = -171;
	float fFar = 28;
	

	float fFoVHalf = 1.3f;
	float fSpeed = 100.0f;
	float fMaxSpeed = 300.0f;
	float fStartSpeed = 0.0f;
	float skyOffset = 0.0f;
	Vector2 windowsize = { 800,450 };
	Vector2 position = { 0,0 };
	Texture2D texSky;
	Texture2D texMap;
	
	Image imgMap;
	Frustum frustum = {};

	void DrawMode7Line(int y);


	const int blowup_scale = 1;

	int counter = 0;



};