#pragma once

class Manager
{

public:
	//メンバー関数、メソッド
	static void Init();
	static void Uninit();
	static void Update();
	static void Draw();

	static class Scene* GetScene() { return m_Scene; }

	template<typename T>//テンプレート関数
	static void SetScene()
	{
		if (m_Scene)
		{
			m_Scene->Uninit();
			delete m_Scene;
		}
		m_Scene = new T();
		m_Scene->Init();
	}


private:
	static class Scene* m_Scene;
};