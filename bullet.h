#pragma once
#include "gameObject.h"

class Bullet :public GameObject
{
public:
	virtual void Init();
	void Uninit();
	void Update();
	void Draw();

private:
	D3DXMATRIX _rot;

};