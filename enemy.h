#pragma once
#include "gameObject.h"
class Enemy :public GameObject
{
public:
	virtual void Init();
	void Uninit();
	void Update();
	void Draw();

private:


};