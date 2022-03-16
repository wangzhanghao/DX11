#pragma once
#include <list>
#include <vector>
#include <typeinfo>

#include "renderer.h"
#include "model.h"
#include "gameObject.h"
#include "camera.h"
#include "field.h"
#include "effect.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "skybox.h"
#include "polygon2D.h"
#include "shadow.h"
#include "fieldModel.h"

#include "modelManager.h"
#include "soundManager.h"

class Scene
{
public:
	Scene(){}
	virtual ~Scene(){}

	virtual void Init() = 0;
	
		//for (int i = 0; i < 3; i++)
		//{
		//	//ModelManager::Load(i);
		//	mgr_Model->Load(i);
		//}
		
	

	template<typename T>//テンプレート関数
	T* AddGameObject(int layer)
	{
		T* gameObject = new T();
		gameObject->Init();
		g_GameObject[layer].push_back(gameObject);

		return gameObject;
	}

	template<typename T>//テンプレート関数
	T* GetGameObject(int layer)
	{
		for (GameObject* object : g_GameObject[layer])
		{
			if (typeid(*object) == typeid(T))
			{
				return(T*)object;
			}
		}
		return NULL;
	}
	template<typename T>//テンプレート関数
	std::vector<T*> GetGameObjects(int layer)
	{
		std::vector<T*> objects;
		for (GameObject* object : g_GameObject[layer])
		{
			if (typeid(*object) == typeid(T))
			{
				objects.push_back((T*)object);
			}
		}
		return objects;
	}


	virtual void Uninit()
	{
		for (int layer = 0; layer < 3; layer++)
		{
			for (GameObject* object : g_GameObject[layer])
			{
				object->Uninit();
				delete object;
			}
			g_GameObject[layer].clear();
		}
		//mgr_Model->Unload();
		//ModelManager::Unload();
		
	}
	virtual void Update()
	{
		for (int layer = 0; layer < 3; layer++)
		{
			for (GameObject* object : g_GameObject[layer])
			{
				object->Update();
			}
			g_GameObject[layer].remove_if([](GameObject* object) { return object->Destroy(); });
	
		}

		}
	virtual void Draw()
	{
		for (int layer = 0; layer < 3; layer++)
		{
			for (GameObject* object : g_GameObject[layer])
			{
				object->Draw();
			}
		}
	}

	ModelManager* GetModelManager() { return mgr_Model; }
	SoundManager* GetSoundManager() { return mgr_Sound; }

protected:
	std::list<GameObject*> g_GameObject[3];
	ModelManager* mgr_Model = new ModelManager();
	SoundManager* mgr_Sound = new SoundManager();


};

