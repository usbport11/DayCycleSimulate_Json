#ifndef windowH
#define windowH

#define WND_ONEFRAMEMS 16 //14?

class MWindow;

typedef void (MWindow::*FrameFunc) ();

class MWindow
{
private:
	const char* sWindowClass;
	const char* sTitle;
	
	string OpenGLVersion;
	string OpenGLVendor;
	string OpenGLRenderer;
	string ShadersVersion;
	
	HINSTANCE m_hInstance;
	SYSTEM_INFO SystemInfo;
	OSVERSIONINFO VersionInfo;
	
	bool MouseInClient;
	
    bool GetSystemInformation();
    bool CreateConsole();
    bool InitResolutions();
    int InitContext(HWND hWnd);
	int InitGL();
	int CloseGL();
	void ResizeScene(int width, int height);
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	bool SetVSync(bool Enable);
	bool VSyncEnabled;
	
	FrameFunc pFrameWaitFunc;
	void FrameWaitVsync();
	void FrameWait();
	
protected:
	bool Exit;
	HDC m_hDC;
	HGLRC m_hGLRC;
	HWND m_hWnd;
	bool WindowInitialized;
	
	int WindowWidth;
    int WindowHeight;
    int HalfWindowSize[2];
    int WindowFix[2];
	
	stResolution InitialResolution;
	stResolution CurrentResolution;
	vector<stResolution> Resolutions;
	
	bool Fullscreen;
	bool EnterFullscreen();
	bool ExitFullscreen(int Width, int Height);
	bool ChangeWindowSize(int Width, int Height);
	
	//frametime
	DWORD TickCount;
	DWORD TickDelta;
	
	//virtual functions
	virtual void OnDraw();
	virtual void OnActivate(WPARAM wParam);
	virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);
	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam);
	virtual void OnClose();
	virtual void OnSize();
	virtual void OnMouseHover(WPARAM wParam, LPARAM lParam);
	virtual void OnMouseLButton(WPARAM wParam, LPARAM lParam);
	
public:
	MWindow();
	~MWindow();
	int CreateMainWindow(HINSTANCE hInstance);
	int CreateMainWindow(HINSTANCE hInstance, int Width, int Height);
	LRESULT CALLBACK WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
	
	void Run();
	HWND GetHWND();
};

#endif
