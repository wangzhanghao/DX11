#include "main.h"
#include "renderer.h"
#include "titleField.h"

#define OFFSET_X			SCREEN_WIDTH/2		// スクリーンの幅オフセット
#define OFFSET_Y			SCREEN_HEIGHT*3/4	// スクリーンの高さオフセット

#define PARTICLE_MAX		100					// パーティクルの数

// 変数
typedef struct {
	int		status;		// 状態
	float	px, py;		// 位置
	float	vx, vy;		// 速度
	float	ax, ay;		// 加速度
} PARTICLE;

PARTICLE	g_Particle[PARTICLE_MAX];			// パーティクル本体

HPEN		g_hpenRed, g_hpenGreen, g_hpenBlue;
HBRUSH		g_hbrRed, g_hbrGreen, g_hbrBlue;

void TitleField::Init()
{
	m_Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_hpenRed = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	g_hpenGreen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	g_hpenBlue = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	g_hbrRed = CreateSolidBrush(RGB(255, 0, 0));
	g_hbrGreen = CreateSolidBrush(RGB(0, 255, 0));
	g_hbrBlue = CreateSolidBrush(RGB(0, 0, 255));
	VERTEX_3D vertex[4];

	vertex[0].Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);

	vertex[1].Position = D3DXVECTOR3(1280.0f, 0.0f, 0.0f);
	vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

	vertex[2].Position = D3DXVECTOR3(0.0f, 720.0f, 0.0f);
	vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);

	vertex[3].Position = D3DXVECTOR3(1280.0f, 720.0f, 0.0f);
	vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

	//頂点バッファ生成
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	//テクスチャ読み込み
	D3DX11CreateShaderResourceViewFromFile(Renderer::GetDevice(),
		"asset/texture/titlelogo.png",
		NULL, NULL,
		&m_Texture,
		NULL);
	assert(m_Texture);/**/

	D3DX11CreateShaderResourceViewFromFile(Renderer::GetDevice(),
		"asset/texture/titleback.png",
		NULL, NULL,
		&t_Back,
		NULL);
	assert(t_Back);
	D3DX11CreateShaderResourceViewFromFile(Renderer::GetDevice(),
		"asset/texture/titlefront.png",
		NULL, NULL,
		&t_Front,
		NULL);
	assert(t_Front);
	D3DX11CreateShaderResourceViewFromFile(Renderer::GetDevice(),
		"asset/texture/flight.png",
		NULL, NULL,
		&t_Flight,
		NULL);
	assert(t_Flight);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "unlitTexturePS.cso");
}

void TitleField::Uninit()
{
	DeleteObject(g_hpenRed);
	DeleteObject(g_hpenGreen);
	DeleteObject(g_hpenBlue);
	DeleteObject(g_hbrRed);
	DeleteObject(g_hbrGreen);
	DeleteObject(g_hbrBlue);

	m_VertexBuffer->Release();
	m_Texture->Release();

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void TitleField::Update()
{
	ExecParticle();
	m_Position.x += 10.0f;
	m_Position.y -= 5.0f;
	if (m_Position.x >= 1000.0f)
	{
		srand(m_Position.y);
		float xx = rand() % 300;
		m_Position.x = -340.0f - xx;
		m_Position.y = 160.0f + xx;
	}
}

void TitleField::Draw()
{
	HDC			hdc;
	PAINTSTRUCT	ps;
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	//シェーダー設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);
	//マトリクス設定
	Renderer::SetWorldViewProjection2D();
	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	//テクスチャ設定
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &t_Back);
	//プリミティブトボロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ポリゴン描画
	Renderer::GetDeviceContext()->Draw(4, 0);

	//マトリクス設定
	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans, m_Position.x, m_Position.y, 0.0f);
	Renderer::SetWorldMatrix(&trans);

	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &t_Flight);
	Renderer::GetDeviceContext()->Draw(4, 0);

	Renderer::SetWorldViewProjection2D();
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &t_Front);
	Renderer::GetDeviceContext()->Draw(4, 0);

	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);
	Renderer::GetDeviceContext()->Draw(4, 0);

	HWND hWnd = GetWindow();
	hdc = BeginPaint(hWnd, &ps);
	DrawParticle(hdc);		// パーティクルを描画
	EndPaint(hWnd, &ps);
}

void TitleField::ExecParticle()
{
	int		i;
	bool	bBorn;

	bBorn = false;
	for (i = 0; i < PARTICLE_MAX; i++) {
		switch (g_Particle[i].status) {
		case 0:					// 待機状態
			if (!bBorn) {
				bBorn = true;
				g_Particle[i].status = 1;
			}
			break;
		case 1:
			g_Particle[i].px = 500.0f;
			g_Particle[i].py = 500.0f;
			g_Particle[i].vx = rand() % 10 - 5.0f;
			g_Particle[i].vy = 15.0f;
			g_Particle[i].ax = rand() % 10 / 20.0f - 0.25f;
			g_Particle[i].ay = -0.5f;
			g_Particle[i].status = 2;
			// THRU
		case 2:
			g_Particle[i].vx += g_Particle[i].ax;
			g_Particle[i].vy += g_Particle[i].ay;

			g_Particle[i].px += g_Particle[i].vx;
			g_Particle[i].py += g_Particle[i].vy;

			if (g_Particle[i].py <= -(SCREEN_HEIGHT - OFFSET_Y) ||
				g_Particle[i].py >= OFFSET_Y) {
				g_Particle[i].status = 0;
			}
			break;
		}
	}
}
void TitleField::DrawParticle(HDC hdc)
{
	HPEN hpenOld = (HPEN)SelectObject(hdc, g_hpenBlue);
	HBRUSH hbrOld = (HBRUSH)SelectObject(hdc, g_hbrBlue);
	for (int i = 0; i < PARTICLE_MAX; i++) {
		if (g_Particle[i].status == 2) {
			Ellipse(hdc,
				(int)(g_Particle[i].px - 5.0f) + OFFSET_X,
				(int)(-g_Particle[i].py - 5.0f) + OFFSET_Y,
				(int)(g_Particle[i].px + 5.0f) + OFFSET_X,
				(int)(-g_Particle[i].py + 5.0f) + OFFSET_Y);
		}
	}
	SelectObject(hdc, hbrOld);
	SelectObject(hdc, hpenOld);
}
