
class DXSound
{
private:
	IGraphBuilder *pGraph = NULL;
	IMediaControl *pControl = NULL;
	IMediaEventEx *pEvent = NULL;
	int audioState;
	
	HRESULT InitDirectShow(HWND hWnd, PCWSTR audiofile);
public:
	DXSound(HWND hWnd, LPCWSTR audiofile);
	void HandleGraphEvent();
	VOID Cleanup();
	void play();
	void pause();
	int getAudioState() { return audioState; }
};


HRESULT DXSound::InitDirectShow(HWND hWnd, LPCWSTR audiofile)
{
	audioState = 0;
	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL,
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);

	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	hr = pGraph->QueryInterface(IID_IMediaEventEx, (void **)&pEvent);

	hr = pGraph->RenderFile(audiofile, NULL);

	pEvent->SetNotifyWindow((OAHWND)hWnd, WM_GRAPHNOTIFY, 0);

	return S_OK;
}

void DXSound::play()
{
	this->pControl->Run();
	audioState++;
}

void DXSound::pause()
{
	this->pControl->Pause();
	audioState++;
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
		case EC_COMPLETE: 
		case EC_USERABORT: 
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