#pragma once
#include "gameObject.h"
class FieldModel :public GameObject
{
public:
	virtual void Init();
	void Uninit();
	void Update();
	void Draw();

private:

};