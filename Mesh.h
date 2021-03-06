class CXMesh
{
private:
	LPD3DXMESH              g_pMesh = NULL; 
	D3DMATERIAL9*           g_pMeshMaterials = NULL; 
	LPDIRECT3DTEXTURE9*     g_pMeshTextures = NULL; 
	DWORD                   g_dwNumMaterials = 0L;   
	D3DXMATRIX				matWorld;
	
	HRESULT InitMeshGeometry(LPDIRECT3DDEVICE9 g_pd3dDevice, const char* meshFile);
public:
	CXMesh(LPDIRECT3DDEVICE9 g_pd3dDevice, const char* meshFile);
	VOID Cleanup();
	void drawMesh(LPDIRECT3DDEVICE9 g_pd3dDevice);
	void setMeshDefaultPos(LPDIRECT3DDEVICE9 g_pd3dDevice);
	void setMeshPos(LPDIRECT3DDEVICE9 g_pd3dDevice, float dz);
};

HRESULT CXMesh::InitMeshGeometry(LPDIRECT3DDEVICE9 g_pd3dDevice, const char* meshFile)
{
	LPD3DXBUFFER pD3DXMtrlBuffer;

	if (FAILED(D3DXLoadMeshFromX(meshFile, D3DXMESH_SYSTEMMEM,
		g_pd3dDevice, NULL,
		&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials,
		&g_pMesh)))
	{
		MessageBox(NULL, "Could not find your mesh model", "Source.exe", MB_OK);
		return E_FAIL;
	}

	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
	g_pMeshTextures = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];

	for (DWORD i = 0; i<g_dwNumMaterials; i++)
	{
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

		g_pMeshTextures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlen(d3dxMaterials[i].pTextureFilename) > 0)
		{
			if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice,
				d3dxMaterials[i].pTextureFilename,
				&g_pMeshTextures[i])))
			{
				const TCHAR* strPrefix = TEXT("..\\");
				const int lenPrefix = lstrlen(strPrefix);
				TCHAR strTexture[MAX_PATH];
				lstrcpyn(strTexture, strPrefix, MAX_PATH);
				lstrcpyn(strTexture + lenPrefix, d3dxMaterials[i].pTextureFilename, MAX_PATH - lenPrefix);
				if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice,
					strTexture,
					&g_pMeshTextures[i])))
				{
					MessageBox(NULL, "Could not find texture map for mesh object", "Source.exe", MB_OK);
				}
			}
		}
	}

	pD3DXMtrlBuffer->Release();

	LPDIRECT3DVERTEXBUFFER9 VertexBuffer = NULL;
	D3DXVECTOR3* Vertices = NULL;
	D3DXVECTOR3 Center;
	FLOAT Radius;
	DWORD FVFVertexSize = D3DXGetFVFVertexSize(g_pMesh->GetFVF());
	g_pMesh->GetVertexBuffer(&VertexBuffer);
	VertexBuffer->Lock(0, 0, (VOID**)&Vertices, D3DLOCK_DISCARD);
	D3DXComputeBoundingSphere(Vertices, g_pMesh->GetNumVertices(), FVFVertexSize, &Center, &Radius);

	VertexBuffer->Unlock();
	VertexBuffer->Release();
}

CXMesh::CXMesh(LPDIRECT3DDEVICE9 g_pd3dDevice, const char* meshFile)
{	
	InitMeshGeometry(g_pd3dDevice, meshFile);
}

VOID CXMesh::Cleanup()
{
	if (g_pMeshMaterials != NULL)
		delete[] g_pMeshMaterials;

	if (g_pMeshTextures)
	{
		for (DWORD i = 0; i < g_dwNumMaterials; i++)
		{
			if (g_pMeshTextures[i])
				g_pMeshTextures[i]->Release();
		}
		delete[] g_pMeshTextures;
	}
	if (g_pMesh != NULL)
		g_pMesh->Release();
}

void CXMesh::drawMesh(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	for (DWORD i = 0; i<g_dwNumMaterials; i++)
	{
		g_pd3dDevice->SetMaterial(&g_pMeshMaterials[i]);
		g_pd3dDevice->SetTexture(0, g_pMeshTextures[i]);

		g_pMesh->DrawSubset(i);
	}
}

void CXMesh::setMeshDefaultPos(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	D3DXMATRIX matScale, matTrans, matRotY;
	D3DXMatrixRotationY(&matRotY, 4.68f);
	D3DXMatrixTranslation(&matTrans, 0.0f, -1.4f, 0.0f);
	FLOAT scaleValue = 0.2f;
	D3DXMatrixScaling(&matScale, scaleValue, scaleValue, scaleValue);
	matWorld = matScale * matTrans * matRotY;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
}

void CXMesh::setMeshPos(LPDIRECT3DDEVICE9 g_pd3dDevice, float dz)
{
	D3DXMATRIX matTrans;
	D3DXMatrixTranslation(&matTrans, 0, 0, dz);
	matTrans = this->matWorld * matTrans;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matTrans);
}