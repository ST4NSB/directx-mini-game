
class DXSound
{
private:
	IGraphBuilder *pGraph = NULL;
	IMediaControl *pControl = NULL;
	IMediaEventEx *pEvent = NULL;
	
	HRESULT InitDirectShow(HWND hWnd, PCWSTR audiofile);
public:
	DXSound(HWND hWnd, LPCWSTR audiofile);
	void HandleGraphEvent();
	VOID Cleanup();
	void play();
};


HRESULT DXSound::InitDirectShow(HWND hWnd, LPCWSTR audiofile)
{
	//Create Filter Graph
	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL,
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);

	//Create Media Control and Events
	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	hr = pGraph->QueryInterface(IID_IMediaEventEx, (void **)&pEvent);

	//Load a file
	hr = pGraph->RenderFile(audiofile, NULL);

	//Set window for events
	pEvent->SetNotifyWindow((OAHWND)hWnd, WM_GRAPHNOTIFY, 0);

	return S_OK;
}

void DXSound::play()
{
	this->pControl->Run();
}

DXSound::DXSound(HWND hWnd, LPCWSTR audiofile)
{
	InitDirectShow(hWnd, audiofile);
}


void DXSound::HandleGraphEvent()
{
	if (pEvent == NULL)
	{
		return;
	}
	long evCode;
	LONG_PTR param1, param2;

	while (SUCCEEDED(pEvent->GetEvent(&evCode, &param1, &param2, 0)))
	{

		pEvent->FreeEventParams(evCode, param1, param2);
		switch (evCode)
		{
		case EC_COMPLETE:  // Fall through.
		case EC_USERABORT: // Fall through.
		case EC_ERRORABORT:
			PostQuitMessage(0);
			return;
		}
	}
}

VOID DXSound::Cleanup()
{
	if (pGraph)
		pGraph->Release();

	if (pControl)
		pControl->Release();

	if (pEvent)
		pEvent->Release();
}