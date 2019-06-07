class DXInput
{
private:
	BYTE					g_Keystate[256];
	LPDIRECTINPUTDEVICE8	g_pDinmouse;					// the pointer to the mouse device
	DIMOUSESTATE			g_pMousestate;
	LPDIRECTINPUT8			g_pDin;			
	LPDIRECTINPUTDEVICE8	g_pDinKeyboard;
	bool					mouseState[2];

	HRESULT InitDInput(HINSTANCE hInstance, HWND hWnd);
public:
	
	DXInput(HINSTANCE hInstance, HWND hWnd);
	VOID DetectInput();
	VOID CleanDInput();
	void closeWindow(HWND hwnd);
	void detectKeyMovement(float &dz, const float movementSpeed, CXCamera *camera);
	void detectMouseInput(CXCamera *camera, int &mouseValue);
};

HRESULT DXInput::InitDInput(HINSTANCE hInstance, HWND hWnd)
{
	// create the DirectInput interface
	DirectInput8Create(hInstance,    // the handle to the application
		DIRECTINPUT_VERSION,    // the compatible version
		IID_IDirectInput8,    // the DirectInput interface version
		(void**)&g_pDin,    // the pointer to the interface
		NULL);    // COM stuff, so we'll set it to NULL

				  // create the keyboard device
	g_pDin->CreateDevice(GUID_SysKeyboard,    // the default keyboard ID being used
		&g_pDinKeyboard,    // the pointer to the device interface
		NULL);    // COM stuff, so we'll set it to NULL

				  // create the mouse device
	g_pDin->CreateDevice(GUID_SysMouse,
		&g_pDinmouse,  // the pointer to the device interface
		NULL); // COM stuff, so we'll set it to NULL

			   // set the data format to keyboard format
	g_pDinKeyboard->SetDataFormat(&c_dfDIKeyboard);

	// set the data format to mouse format
	g_pDinmouse->SetDataFormat(&c_dfDIMouse);

	// set the control we will have over the keyboard
	g_pDinKeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	// set the control we will have over the mouse
	g_pDinmouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

	return S_OK;
}

DXInput::DXInput(HINSTANCE hInstance, HWND hWnd)
{
	mouseState[0] = false;
	mouseState[1] = false;
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
	g_pDinKeyboard->Unacquire();    // make sure the keyboard is unacquired
	g_pDinmouse->Unacquire();    // make sure the mouse in unacquired
	g_pDin->Release();    // close DirectInput before exiting
}

void DXInput::detectKeyMovement(float &dz, const float movementSpeed, CXCamera *camera)
{
	if (g_Keystate[DIK_UPARROW] & 0x80)
	{
		dz += movementSpeed;
		//camera->MoveForward(dz/2);
		//camera->Update();
	}
	if (g_Keystate[DIK_DOWNARROW] & 0x80)
	{
		dz -= movementSpeed;
		///camera->MoveBackward(dz/2);
		//camera->Update();
	}
}

void DXInput::detectMouseInput(CXCamera *camera, int &mouseValue)
{
	if (!(g_pMousestate.rgbButtons[0] & 0x80) && mouseState[0])
		mouseState[0] = false;
	if (!(g_pMousestate.rgbButtons[1] & 0x80) && mouseState[1])
		mouseState[1] = false;
	if (g_pMousestate.rgbButtons[0] & 0x80 && !mouseState[0]) 
	{
		++mouseValue %= 6;
		mouseState[0] = true;
	}
	if (g_pMousestate.rgbButtons[1] & 0x80 && !mouseState[1]) 
	{
		--mouseValue %= 6;
		mouseState[1] = true;
	}
	
}

void DXInput::closeWindow(HWND hwnd)
{
	if (g_Keystate[DIK_ESCAPE] & 0x80)
		PostMessage(hwnd, WM_DESTROY, 0, 0);
}