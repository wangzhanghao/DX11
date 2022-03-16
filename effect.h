#pragma once
#include "gameObject.h"
class Effect :public GameObject
{
public:

	virtual void Init();
	void Uninit();
	void Update();
	void Draw();
	D3DXVECTOR3 GetTarget();

private:
	int time = 0;

};