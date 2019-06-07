struct CUSTOMVERTEX
{
	D3DXVECTOR3 position; // The position
	D3DXVECTOR3 normal;
	DWORD color;    // The color
//#ifndef SHOW_HOW_TO_USE_TCI
	FLOAT tu, tv;   // The texture coordinates
//#endif
};

//#ifdef SHOW_HOW_TO_USE_TCI
//#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
//#else
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX1)
//#endif

class CXSkybox
{
private:
	LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; 
	LPDIRECT3DINDEXBUFFER9	g_pIB = NULL;
	LPDIRECT3DTEXTURE9      g_pTexture = NULL; 
	LPDIRECT3DTEXTURE9		*g_pTextureArray = NULL;
	
	HRESULT InitGeometry(LPDIRECT3DDEVICE9 g_pd3dDevice, const char* szTextureFiles[]);
public:
	CXSkybox(LPDIRECT3DDEVICE9 g_pd3dDevice, const char* szTextureFiles[]);
	VOID Cleanup();
	void drawSkybox(LPDIRECT3DDEVICE9 g_pd3dDevice);
	void setSkyboxDefaultPos(LPDIRECT3DDEVICE9 g_pd3dDevice);
};


HRESULT CXSkybox::InitGeometry(LPDIRECT3DDEVICE9 g_pd3dDevice, const char* szTextureFiles[])
{
	g_pTextureArray = new LPDIRECT3DTEXTURE9[6];
	for (int i = 0; i < 6; i++) 
	{
		if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, szTextureFiles[i], &g_pTextureArray[i])))
		{
			MessageBox(NULL, "Could not find texture files for skybox", "Textures.exe", MB_OK);
			return E_FAIL;
		}
	}

	CUSTOMVERTEX g_pVertices[] =
	{
		//RIGHT
		{ D3DXVECTOR3(0,8,0),D3DXVECTOR3(0,8,0),D3DCOLOR_XRGB(255,255,255), 0, 0 },
		{ D3DXVECTOR3(0,8,-8),D3DXVECTOR3(0,8,-8),D3DCOLOR_XRGB(255,255,255), 1, 0 },
		{ D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),D3DCOLOR_XRGB(255,255,255), 0, 1 },
		{ D3DXVECTOR3(0,0,-8),D3DXVECTOR3(0,0,-8),D3DCOLOR_XRGB(255,255,255), 1, 1 },

		//LEFT
		{ D3DXVECTOR3(8,0,0),D3DXVECTOR3(8,0,0),D3DCOLOR_XRGB(255,255,255), 0, 1 },
		{ D3DXVECTOR3(8,0,-8),D3DXVECTOR3(8,0,-8),D3DCOLOR_XRGB(255,255,255), 1, 1 },
		{ D3DXVECTOR3(8,8,0),D3DXVECTOR3(8,8,0),D3DCOLOR_XRGB(255,255,255), 0, 0 },
		{ D3DXVECTOR3(8,8,-8),D3DXVECTOR3(8,8,-8),D3DCOLOR_XRGB(255,255,255), 1, 0 },

		//BOTTOM
		{ D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),D3DCOLOR_XRGB(255,255,255), 0, 1 },
		{ D3DXVECTOR3(8,0,0),D3DXVECTOR3(8,0,0),D3DCOLOR_XRGB(255,255,255), 1, 1 },
		{ D3DXVECTOR3(0,0,-8),D3DXVECTOR3(0,0,-8),D3DCOLOR_XRGB(255,255,255), 0, 0 },
		{ D3DXVECTOR3(8,0,-8),D3DXVECTOR3(4,0,-8),D3DCOLOR_XRGB(255,255,255), 1, 0 },

		//TOP
		{ D3DXVECTOR3(0,8,0),D3DXVECTOR3(0,8,0),D3DCOLOR_XRGB(255,255,255), 0, 1 },
		{ D3DXVECTOR3(8,8,0),D3DXVECTOR3(8,8,0),D3DCOLOR_XRGB(255,255,255), 1, 1 },
		{ D3DXVECTOR3(0,8,-8),D3DXVECTOR3(0,8,-8),D3DCOLOR_XRGB(255,255,255), 0, 0 },
		{ D3DXVECTOR3(8,8,-8),D3DXVECTOR3(8,8,-8),D3DCOLOR_XRGB(255,255,255), 1, 0 },

		//FRONT
		{ D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),D3DCOLOR_XRGB(255,255,255), 1, 1 },
		{ D3DXVECTOR3(8,0,0),D3DXVECTOR3(8,0,0),D3DCOLOR_XRGB(255,255,255), 0, 1 },
		{ D3DXVECTOR3(0,8,0),D3DXVECTOR3(0,8,0),D3DCOLOR_XRGB(255,255,255), 1, 0 },
		{ D3DXVECTOR3(8,8,0),D3DXVECTOR3(8,8,0),D3DCOLOR_XRGB(255,255,255), 0, 0 },

		//BACK
		{ D3DXVECTOR3(8,0,-8),D3DXVECTOR3(8,0,-8),D3DCOLOR_XRGB(255,255,255), 1, 1 },
		{ D3DXVECTOR3(0,0,-8),D3DXVECTOR3(0,0,-8),D3DCOLOR_XRGB(255,255,255), 0, 1 },
		{ D3DXVECTOR3(8,8,-8),D3DXVECTOR3(8,8,-8),D3DCOLOR_XRGB(255,255,255), 1, 0 },
		{ D3DXVECTOR3(0,8,-8),D3DXVECTOR3(0,8,-8),D3DCOLOR_XRGB(255,255,255), 0, 0 },
	};

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(6 * 4 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	VOID* pVertices;

	if (FAILED(g_pVB->Lock(0, sizeof(g_pVertices), (void**)& pVertices, 0)))
		return E_FAIL;

	// copy the vertices into the buffer
	memcpy(pVertices, g_pVertices, sizeof(g_pVertices));

	// unlock the vertex buffer
	g_pVB->Unlock();

	return S_OK;
}

CXSkybox::CXSkybox(LPDIRECT3DDEVICE9 g_pd3dDevice, const char* szTextureFiles[])
{
	InitGeometry(g_pd3dDevice, szTextureFiles);
}

VOID CXSkybox::Cleanup()
{
	if (g_pTexture != NULL)
		g_pTexture->Release();

	if (g_pTextureArray != NULL)
		for (int i = 0; i < 6; i++)
			g_pTextureArray[i]->Release();

	if (g_pVB != NULL)
		g_pVB->Release();
}

void CXSkybox::drawSkybox(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	for (int i = 0; i < 6; i++) {

		/*#ifdef SHOW_HOW_TO_USE_TCI
				D3DXMATRIXA16 mat;
				mat._11 = 0.25f; mat._12 = 0.00f; mat._13 = 0.00f; mat._14 = 0.00f;
				mat._21 = 0.00f; mat._22 = -0.25f; mat._23 = 0.00f; mat._24 = 0.00f;
				mat._31 = 0.00f; mat._32 = 0.00f; mat._33 = 1.00f; mat._34 = 0.00f;
				mat._41 = 0.50f; mat._42 = 0.50f; mat._43 = 0.00f; mat._44 = 1.00f;

				g_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &mat);
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
		#endif*/

		g_pd3dDevice->SetTexture(0, g_pTextureArray[i]);
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i * 4, 2);
	}
}


void CXSkybox::setSkyboxDefaultPos(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	D3DXMATRIXA16 matWorld, matRotationy;
	D3DXMATRIXA16 matScale, matTrans;

	// 4 top, 3 bottom
	D3DXMatrixRotationY(&matRotationy, 20.77f);
	FLOAT yScale = 1.1f, xzScale = (1.5 * 5.0f);
	D3DXMatrixScaling(&matScale, xzScale, yScale, xzScale);
	D3DXMatrixTranslation(&matTrans, -38.0f, -0.93f, 30.5f);
	matWorld = matScale * matTrans * matRotationy;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
}
