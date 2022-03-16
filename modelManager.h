#pragma once

#define PLAYER	(0)
#define ENEMY	(1)
#define BULLET	(2)
#define FIELD	(3)

class ModelManager 
{
public:
	void Load(int type);

	void Unload();

	Model* GetModelP(int type);

	std::vector<Model*> GetModeMgr() { return v_Model; }

protected:
	std::vector<Model*> v_Model;
};