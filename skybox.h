#pragma once
#include "gameObject.h"
class Skybox :public GameObject
{
public:
	virtual void Init();
	void Uninit();
	void Update();
	void Draw();

private:
	Model* m_Model;


};