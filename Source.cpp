
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <strsafe.h>
#include <dinput.h>
#include <dshow.h>
#include <math.h>

#define D3DXToRadian(degree) ((degree) * (D3DX_PI / 180.0f))
#define WM_GRAPHNOTIFY  WM_APP + 1

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include "Camera.h"
#include "Skybox.h"
#include "Mesh.h"
#include "Sound.h"
#include "Input.h"

/*
	Keys:
		P - play/pause music
		Camera (mouseValue even):
			UP-Arrow - move camera forward
			DOWN-Arrow - move camera backwards
			LEFT-Arrow - rotate camera left
			RIGHT-Arrow - rotate camera right
			W - rotate camera up
			S - rotate camera down
		General (mouseValue odd):
			UP-Arrow - move mesh forward
			DOWN-Arrow - move mesh backwards
			V - change mesh camera view
	Mouse:
		LEFT-Button - change Mode (Camera/General with mouseValue)
		RIGHT-Button - change Mode (Camera/General with mouseValue)
*/

LPDIRECT3D9             g_pD3D = NULL; 
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; 
HDC hdc;
HWND hWnd;

CXCamera *camera;
CXMesh *mesh;
DXInput *dxinput;
CXSkybox *skybox;
DXSound *music;

#define PACK 1
#if PACK == 1
const char* pack[6] = {
	"skybox\\pack1\\side.png", "skybox\\pack1\\side.png", "skybox\\pack1\\bottom.png",
	"skybox\\pack1\\top.png", "skybox\\pack1\\side.png", "skybox\\pack1\\side.png" };
LPCWSTR audio = L"sounds\\audio1.wav";
#elif PACK == 2
const char* pack[6] = {
	"skybox\\pack2\\side.png", "skybox\\pack2\\side.png", "skybox\\pack2\\bottom.png",
	"skybox\\pack2\\top.png", "skybox\\pack2\\side.png", "skybox\\pack2\\side.png" };
LPCWSTR audio = L"sounds\\audio2.wav";
#endif
const char* mesh_file = "meshes\\walle.x";

const float	movementSpeed = 0.015f;
float dz;
int mouseValue = 0;
int cameraOption = 0;


HRESULT InitD3D(HWND hWnd)
{
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &g_pd3dDevice)))
			return E_FAIL;
	}

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	return S_OK;
}


HRESULT InitGeometry()
{
	camera = new CXCamera(g_pd3dDevice);
	mesh = new CXMesh(g_pd3dDevice, mesh_file);
	skybox = new CXSkybox(g_pd3dDevice, pack);

	return S_OK;
}

VOID Cleanup()
{
	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

VOID SetupMatrices()
{
	mesh->setMeshDefaultPos(g_pd3dDevice);
	
	if(mouseValue % 2 == 0)
		camera->setCameraPos(cameraOption);
	
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

VOID SetupLigtsOutside()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	g_pd3dDevice->SetMaterial(&mtrl);

	D3DXVECTOR3 vecDir;
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;

	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;

	light.Direction = D3DXVECTOR3(1, 1, 1);

	light.Range = 1000.0f;

	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA(170, 170, 170, 200));
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->LightEnable(0, TRUE); 
}

VOID SetupLightsRedAlert()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 0.4f;
	g_pd3dDevice->SetMaterial(&mtrl);

	D3DXVECTOR3 vecDir;
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 0.8f;
	light.Diffuse.g = 0.1f;
	light.Diffuse.b = 0.1f;
	vecDir = D3DXVECTOR3(cosf(timeGetTime() / 450.0f),
		1.0f,
		sinf(timeGetTime() / 450.0f));
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
	light.Range = 1000.0f;
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->LightEnable(0, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x00202020);
}

VOID Render()
{
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);


	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		#if PACK == 1
			SetupLigtsOutside();
		#elif PACK == 2
			SetupLightsRedAlert();
		#endif PACK == 2

		SetupMatrices();

		dxinput->detectKeyMovement(dz, movementSpeed, music, camera, mouseValue, cameraOption);
		dxinput->detectMouseInput(mouseValue);
		
		skybox->setSkyboxDefaultPos(g_pd3dDevice);
		skybox->drawSkybox(g_pd3dDevice);
		
		mesh->setMeshPos(g_pd3dDevice, dz);
		mesh->drawMesh(g_pd3dDevice);

		g_pd3dDevice->EndScene();
	}
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}


LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		music->Cleanup();
		mesh->Cleanup();
		skybox->Cleanup();
		Cleanup();
		dxinput->CleanDInput();
		PostQuitMessage(0);
		return 0;
	case WM_GRAPHNOTIFY:
		music->HandleGraphEvent();
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"D3D Wall-e", NULL };
	RegisterClassEx(&wc);

	hWnd = CreateWindow("D3D Wall-e", "D3D Wall-e: Mini Simulator",
		WS_OVERLAPPEDWINDOW, 150, 100, 1000, 750,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	HRESULT hr = CoInitialize(NULL);
	hdc = GetDC(hWnd);

	if (SUCCEEDED(InitD3D(hWnd)))
	{
		music = new DXSound(hWnd, audio);
		music->play();
		dxinput = new DXInput(hInst, hWnd);
		if (SUCCEEDED(InitGeometry()))
		{
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			MSG msg;
			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					dxinput->DetectInput();
					Render();
					dxinput->closeWindow(hWnd);
				}
			}
		}
	}

	UnregisterClass("D3D Wall-e", wc.hInstance);
	return 0;
}