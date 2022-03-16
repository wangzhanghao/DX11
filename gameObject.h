#pragma once
class GameObject
{
public:
	GameObject() {};//コンストラクタ
	virtual ~GameObject() {};//デストラクタ

	virtual void Init() = 0;
	virtual void Uninit() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	void SetPosition(D3DXVECTOR3 Positon) { m_Position = Positon; }
	void SetRotation(D3DXVECTOR3 Rotation) { m_Rotation = Rotation; }
	void SetScale(D3DXVECTOR3 Scale) { m_Scale = Scale; }

	D3DXVECTOR3 GetPosition() { return m_Position; }
	D3DXVECTOR3 GetRotation() { return m_Rotation; }
	D3DXVECTOR3 GetTarget() { return m_Target; };
	D3DXVECTOR3 GetForward()
	{
		D3DXMATRIX rot;
		if(m_Quaternion!=nullptr)
			D3DXMatrixRotationQuaternion(&rot, m_Quaternion);
		else
		D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
		

		D3DXVECTOR3 forward;
		forward.x = rot._31;
		forward.y = rot._32;
		forward.z = rot._33;

		return forward;
	}
	D3DXVECTOR3 GetRight()
	{
		D3DXMATRIX rot;
		if (m_Quaternion != nullptr)
			D3DXMatrixRotationQuaternion(&rot, m_Quaternion);
		else
			D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
		

		D3DXVECTOR3 right;
		right.x = rot._11;
		right.y = rot._12;
		right.z = rot._13;

		return right;
	}
	D3DXVECTOR3 GetUp()
	{
		D3DXMATRIX rot;
		if (m_Quaternion != nullptr)
			D3DXMatrixRotationQuaternion(&rot, m_Quaternion);
		else
			D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
		D3DXVECTOR3 up;
		up.x = rot._21;
		up.y = rot._22;
		up.z = rot._23;

		return up;
	}

	void SetDestroy() { m_Destroy = true; }
	bool Destroy()
	{
		if (m_Destroy)
		{
			Uninit();
			delete this;
			return true;
		}
		else
		{
			return false;
		}
	}

protected:
	bool m_Destroy = false;
	D3DXVECTOR3 m_Position;
	D3DXVECTOR3 m_Rotation;
	D3DXVECTOR3 m_Scale;
	D3DXQUATERNION* m_Quaternion=nullptr;

	D3DXVECTOR3	m_Target;

	ID3D11VertexShader*			m_VertexShader = NULL;		//頂点シェーダー
	ID3D11PixelShader*			m_PixelShader = NULL;
	ID3D11InputLayout*			m_VertexLayout = NULL;
	ID3D11Buffer*				m_VertexBuffer = NULL;		//頂点バッファ
	ID3D11ShaderResourceView*	m_Texture = NULL;			//テクスチャ
};


