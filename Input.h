class DXInput
{
private:
	BYTE					g_Keystate[256];
	LPDIRECTINPUTDEVICE8	g_pDinmouse;				
	DIMOUSESTATE			g_pMousestate;
	LPDIRECTINPUT8			g_pDin;			
	LPDIRECTINPUTDEVICE8	g_pDinKeyboard;
	bool					mouseState[2];
	bool					pKeyState;
	bool					vKeyState;
	bool					cameraMovementKeys[6];

	HRESULT InitDInput(HINSTANCE hInstance, HWND hWnd);
public:
	
	DXInput(HINSTANCE hInstance, HWND hWnd);
	VOID DetectInput();
	VOID CleanDInput();
	void closeWindow(HWND hwnd);
	void detectKeyMovement(float &dz, const float movementSpeed, DXSound *music, CXCamera *camera, int mouseValue, int &cameraOption);
	void detectMouseInput(int &mouseValue);
	bool getSpaceState() { return vKeyState; }
	void buttonPressed(unsigned char DIK, bool &bttnState);
};

HRESULT DXInput::InitDInput(HINSTANCE hInstance, HWND hWnd)
{
	DirectInput8Create(hInstance,   
		DIRECTINPUT_VERSION,    
		IID_IDirectInput8,  
		(void**)&g_pDin,    
		NULL);    

	g_pDin->CreateDevice(GUID_SysKeyboard,    
		&g_pDinKeyboard,    
		NULL);    

	g_pDin->CreateDevice(GUID_SysMouse,
		&g_pDinmouse,  
		NULL); 

	g_pDinKeyboard->SetDataFormat(&c_dfDIKeyboard);

	g_pDinmouse->SetDataFormat(&c_dfDIMouse);

	g_pDinKeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	g_pDinmouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

	return S_OK;
}

DXInput::DXInput(HINSTANCE hInstance, HWND hWnd)
{
	pKeyState = false;
	vKeyState = false;
	mouseState[0] = false;
	mouseState[1] = false;
	for (int i = 0; i < 6; i++) cameraMovementKeys[i] = false;
	InitDInput(hInstance, hWnd);
}

VOID DXInput::DetectInput()
{
	g_pDinKeyboard->Acquire();
	g_pDinmouse->Acquire();

	g_pDinKeyboard->GetDeviceState(256, (LPVOID)g_Keystate);
	g_pDinmouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&g_pMousestate);
}

VOID DXInput::CleanDInput()
{
	g_pDinKeyboard->Unacquire();    
	g_pDinmouse->Unacquire();    
	g_pDin->Release();    
}

void DXInput::buttonPressed(unsigned char DIK, bool &bttnState)
{
	if ((g_Keystate[DIK] & 0x80) && !bttnState)
		bttnState = true;
	if (!(g_Keystate[DIK] & 0x80) && bttnState)
		bttnState = false;
}

void DXInput::detectKeyMovement(float &dz, const float movementSpeed, DXSound *music, CXCamera *camera, int mouseValue, int &cameraOption)
{
	if (mouseValue % 2 == 0)
	{
		if (g_Keystate[DIK_UPARROW] & 0x80)
			dz += movementSpeed;
		if (g_Keystate[DIK_DOWNARROW] & 0x80)
			dz -= movementSpeed;

		if (!(g_Keystate[DIK_V] & 0x80) && vKeyState)
			vKeyState = false;
		if (g_Keystate[DIK_V] & 0x80 && !vKeyState)
		{
			++cameraOption %= 5;
			vKeyState = true;
		}
	}
	else if (mouseValue % 2 == 1)
	{
		const float camera_speed = movementSpeed * 2.5;

		buttonPressed(DIK_UPARROW, cameraMovementKeys[0]);
		if (cameraMovementKeys[0])
		{
			camera->MoveForward(camera_speed * 3);
			camera->Update();
		}
		buttonPressed(DIK_DOWNARROW, cameraMovementKeys[1]);
		if (cameraMovementKeys[1])
		{
			camera->MoveBackward(camera_speed * 3);
			camera->Update();
		}
		buttonPressed(DIK_LEFTARROW, cameraMovementKeys[2]);
		if (cameraMovementKeys[2])
		{
			camera->RotateLeft(camera_speed);
			camera->Update();
		}
		buttonPressed(DIK_RIGHTARROW, cameraMovementKeys[3]);
		if (cameraMovementKeys[3])
		{
			camera->RotateRight(camera_speed);
			camera->Update();
		}
		buttonPressed(DIK_W, cameraMovementKeys[4]);
		if (cameraMovementKeys[4])
		{
			camera->RotateUp(camera_speed);
			camera->Update();
		}
		buttonPressed(DIK_S, cameraMovementKeys[4]);
		if (cameraMovementKeys[4])
		{
			camera->RotateDown(camera_speed);
			camera->Update();
		}
	}
	
	if (!(g_Keystate[DIK_P] & 0x80) && pKeyState)
		pKeyState = false;
	if (g_Keystate[DIK_P] & 0x80 && !pKeyState)
	{
		if (music->getAudioState() % 2 == 0)
			music->play();
		else music->pause();
		pKeyState = true;
	}
}

void DXInput::detectMouseInput(int &mouseValue)
{
	if (!(g_pMousestate.rgbButtons[0] & 0x80) && mouseState[0])
		mouseState[0] = false;
	if (g_pMousestate.rgbButtons[0] & 0x80 && !mouseState[0])
	{
		mouseValue++;
		mouseState[0] = true;
	}

	if (!(g_pMousestate.rgbButtons[1] & 0x80) && mouseState[1])
		mouseState[1] = false;
	if (g_pMousestate.rgbButtons[1] & 0x80 && !mouseState[1])
	{
		mouseValue++;
		mouseState[1] = true;
	}
}

void DXInput::closeWindow(HWND hwnd)
{
	if (g_Keystate[DIK_ESCAPE] & 0x80)
		PostMessage(hwnd, WM_DESTROY, 0, 0);
}