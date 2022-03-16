#pragma once
#include "gameObject.h"
class Shadow :public GameObject
{
public:

	virtual void Init();
	void Uninit();
	void Update();
	void Draw();


private:

};