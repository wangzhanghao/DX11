#pragma once
#include "gameObject.h"
class Field :public GameObject
{
public:

	virtual void Init();
	void Uninit();
	void Update();
	void Draw();


private:

};