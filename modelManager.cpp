#include <vector>
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "modelManager.h"

void ModelManager::Load(int type)
{
	Model* model = new Model();
	switch (type)
	{
	case PLAYER:
		model->Load("asset\\model\\Spitfire\\Spitfire.obj");
		break;
	case ENEMY:
		model->Load("asset\\model\\box.obj");
		break;
	case BULLET:
		model->Load("asset\\model\\bullet\\bullet.obj");
		break;
	case FIELD:
		model->Load("asset\\model\\field\\mountain.obj");
		break;
	default:
		break;
	}
	v_Model.push_back(model);

}

void ModelManager::Unload()
{
	for (Model* model : v_Model)
	{
		model->Unload();
		delete model;
	}
	v_Model.clear();
}

Model* ModelManager::GetModelP(int type)
{
	switch (type)
	{
	case PLAYER:
		return v_Model[0];
	case ENEMY:
		return v_Model[1];
	case BULLET:
		return v_Model[2];
	case FIELD:
		return v_Model[3];
	default:
		return nullptr;
	}
}