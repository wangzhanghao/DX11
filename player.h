#pragma once
#include "gameObject.h"
#include"shadow.h"
class Player :public GameObject
{
public:
	virtual void Init();
	void Uninit();
	void Update();
	void Draw();

	D3DXMATRIX returnRotation() { return rot; }

private:
	Shadow* playerShadow;
	D3DXMATRIX rot;
};

