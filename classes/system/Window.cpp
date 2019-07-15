#include "../../stdafx.h"
#include "Window.h"

LRESULT CALLBACK ChildWProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

MWindow::MWindow()
{   
    //set default width and height
    WindowWidth = 800;
    WindowHeight = 600;
    //window class and caption text
    sWindowClass = "win32app";
	sTitle = "Win32 2D Engine";
	
	Exit = false;
	WindowInitialized = false;
	Fullscreen = false;
	
	HalfWindowSize[0] = HalfWindowSize[1] = 0;
	
	//pFrameUpdateFunc = NULL;
	pFrameWaitFunc = NULL;
	
	TickDelta = 0;
	TickCount = 0;
}

MWindow::~MWindow()
{
}

bool MWindow::CreateConsole()
{
	int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE *fp;
    
    if(!AllocConsole())
    {
    	LogFile<<"Window: Can't allocate console window"<<endl;
    	return false;
	}
    if(!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&coninfo))
    {
    	LogFile<<"Window: Can't get console buffer"<<endl;
    	return false;
	}
    coninfo.dwSize.Y = 500;
    if(!SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),coninfo.dwSize))
    {
    	LogFile<<"Window: Can't set console buffer"<<endl;
    	return false;
	}
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "r" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );
    if(!ios::sync_with_stdio()) 
    {
    	LogFile<<"Window: Can't sync console with stdio"<<endl;
    	return false;
	}
    
    return true;
}

bool MWindow::SetVSync(bool Enable)
{
	typedef BOOL (APIENTRY* PFNWGLSWAPINTERVALPROC)(int);
	PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;
	const char* Extensions = (char*)glGetString(GL_EXTENSIONS);
	if(strstr(Extensions, "WGL_EXT_swap_control") == 0)
	{
		LogFile<<"Window: Vsync not supported"<<endl;
		return false;
	}
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if(wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT(Enable);
		LogFile<<"Window: V-sync: "<<Enable<<endl;
		return true;
	}
	else 
		return false;
}

void MWindow::FrameWaitVsync()
{
}

void MWindow::FrameWait()
{
	while(TickDelta < WND_ONEFRAMEMS)
	{
		Sleep(0);
	}
}

bool MWindow::GetSystemInformation()
{
	//get os version and architecture
    memset(&VersionInfo, 0, sizeof(OSVERSIONINFO));
    VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&VersionInfo);
	switch(VersionInfo.dwMajorVersion)
	{
		case 5:
			LogFile<<"Window: Windows XP"<<endl;
			//pFrameUpdateFunc = &MWindow::FrameUpdateWindowBefore8;
			break;
		case 6:
			switch(VersionInfo.dwMinorVersion)
			{
				case 0:
					LogFile<<"Window: OS: Windows Vista"<<endl;
					//pFrameUpdateFunc = &MWindow::FrameUpdateWindowBefore8;
					break;
				case 1:
					LogFile<<"Window: OS: Windows 7"<<endl;
					//pFrameUpdateFunc = &MWindow::FrameUpdateWindowBefore8;
					break;
				case 2:
					LogFile<<"Window: OS: Windows 8"<<endl;
					//pFrameUpdateFunc = &MWindow::FrameUpdateWindowAfter8;
					break;
				case 3:
					LogFile<<"Window: OS: Windows 8.1"<<endl;
					//pFrameUpdateFunc = &MWindow::FrameUpdateWindowAfter8;
					break;
			}
			break;
		case 10:
			LogFile<<"Window: Windows 10"<<endl;
			//pFrameUpdateFunc = &MWindow::FrameUpdateWindowAfter8;
			break;
		default:
			LogFile<<"Window: OS is not supported. Sorry"<<endl;
			return false;
	}
	GetNativeSystemInfo(&SystemInfo);
	switch(SystemInfo.wProcessorArchitecture)
	{
		case PROCESSOR_ARCHITECTURE_AMD64:
			LogFile<<"Window: Architecture: x64"<<endl;
			break;
		case PROCESSOR_ARCHITECTURE_INTEL:
			LogFile<<"Window: Architecture: x86"<<endl;
			break;
	}
	
	return true;
}

int MWindow::CreateMainWindow(HINSTANCE hInstance, int Width, int Height)
{
	WindowWidth = Width;
    WindowHeight = Height;
    
    return CreateMainWindow(hInstance);
}

int MWindow::CreateMainWindow(HINSTANCE hInstance)
{
	if(!hInstance)
	{
		LogFile<<"Window: Null input instance"<<endl;
		return 0;
	}
    LogFile<<"Window: initializing"<<endl;
    
    if(!GetSystemInformation()) return 0;
	if(!CreateConsole()) return 0;
	if(!InitResolutions()) return 0;

	WNDCLASS wc;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.lpszClassName = sWindowClass;
	wc.style = CS_OWNDC;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpfnWndProc = StaticWndProc;
	wc.lpszMenuName = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClass(&wc))
    {
		MessageBox(NULL, "Can't reg window", "Error", 0);
		return 0;
	}

	m_hInstance = hInstance;

	m_hWnd = CreateWindow(sWindowClass, 
		sTitle, 
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, //WS_OVERLAPPEDWINDOW WS_OVERLAPPED
		CW_USEDEFAULT, CW_USEDEFAULT,
		WindowWidth, WindowHeight,
		NULL, 
		NULL, 
		hInstance, 
		this);// VERY IMPORTANT!

	if(!m_hWnd)
	{
		MessageBox(NULL, "Can't create window", "Error", 0);
		return 0;
	}
	
	//set half window size (need change while resize window)
	HalfWindowSize[0] = WindowWidth >> 1;
	HalfWindowSize[1] = WindowHeight >> 1;
	
	LogFile<<"Window: Main HWND: "<<m_hWnd<<endl;

	//fix client rect size
	RECT wndRect;
	RECT cltRect;
	GetWindowRect(m_hWnd, &wndRect);
	GetClientRect(m_hWnd, &cltRect);
	WindowFix[0] = WindowWidth - cltRect.right;
	WindowFix[1] = WindowHeight - cltRect.bottom;
	MoveWindow(m_hWnd, wndRect.left, wndRect.top, WindowWidth + WindowFix[0], WindowHeight + WindowFix[1], true);
	LogFile<<"Window: Border fix: "<<WindowFix[0]<<" "<<WindowFix[1]<<endl;

	if(!InitContext(m_hWnd)) return 0;
	
	//init glew
	GLenum Error = glewInit();
	if(GLEW_OK != Error)
	{
		LogFile<<"Window: GLEW Loader error: "<<glewGetErrorString(Error)<<endl;
		return 0;
	}
	
	//init opengl
	if(!InitGL()) return 0;
	
	//trying to turn on v-sync
	VSyncEnabled = SetVSync(true);
	if(VSyncEnabled) pFrameWaitFunc = &MWindow::FrameWaitVsync;
	else pFrameWaitFunc = &MWindow::FrameWait;

	return 1;
}

int MWindow::InitContext(HWND hWnd)
{
	//prepare window
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		16,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16, 
		0,
		0,
		PFD_MAIN_PLANE,	
		0,
		0, 0, 0
	};

	m_hDC = GetDC(hWnd);
	if(!m_hDC)
	{
		MessageBox(NULL, "Can't get DC", "Error", 0);
		return 0;
	}
	unsigned short int PixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	if(!PixelFormat)
	{
		MessageBox(NULL, "Can't chose pixelformat", "Error", 0);
		return 0;
	}
	if(!SetPixelFormat(m_hDC, PixelFormat, &pfd))
	{
		MessageBox(NULL, "Can't set pixelformat", "Error", 0);
		return 0;
	}
	DescribePixelFormat(m_hDC, PixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	m_hGLRC = wglCreateContext(m_hDC);
	if(!m_hGLRC)
	{
		MessageBox(NULL, "Can't create context", "Error", 0);
		return 0;
	}
	if(!wglMakeCurrent(m_hDC, m_hGLRC))
	{
		MessageBox(NULL, "Can't make context current", "Error", 0);
		return 0;
	}
	
	WindowInitialized = true;

	return 1;
}

int MWindow::InitGL()
{
    LogFile<<"Window: initialize OpenGl"<<endl;
    
    //get opengl data (here was fall on suspisious notebook)
	OpenGLVersion = (char*)glGetString(GL_VERSION);
	OpenGLVendor = (char*)glGetString(GL_VENDOR);
	OpenGLRenderer = (char*)glGetString(GL_RENDERER);
	ShadersVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	LogFile<<"Window: OpenGL version: "<<OpenGLVersion<<endl;
	LogFile<<"Window: OpenGL vendor: "<<OpenGLVendor<<endl;
	LogFile<<"Window: OpenGL renderer: "<<OpenGLRenderer<<endl;
	LogFile<<"Window: Shaders version: "<<ShadersVersion<<endl;
	
	float VersionOGL, VersionSHD;
	sscanf(OpenGLVersion.c_str(), "%f", &VersionOGL);
	if(VersionOGL < 3.0f)
	{
		LogFile<<"Window: Old version of OpenGL. Sorry"<<endl;
		return 0;
	}
	sscanf(ShadersVersion.c_str(), "%f", &VersionSHD);
	if(VersionSHD < 3.3f)
	{
		LogFile<<"Window: Old version of shaders. Sorry"<<endl;
		return 0;
	}
    
	//set blending function 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return 1;
}

int MWindow::CloseGL()
{
	if(m_hGLRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hGLRC);
	}
	return 1;
}

void MWindow::ResizeScene(int width, int height)
{
    if(width <= 0) width = 1;
	if(height <= 0) height = 1;
	WindowWidth = width;
	WindowHeight = height;
	HalfWindowSize[0] = WindowWidth >> 1;
	HalfWindowSize[1] = WindowHeight >> 1;
}

LRESULT CALLBACK MWindow::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MWindow* pWindow;
	if(msg == WM_CREATE)
	{
		pWindow = (MWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)pWindow);
		LogFile<<"Window: connect static window proc"<<endl;
	}
	else
	{
		pWindow = (MWindow*)GetWindowLongPtr(hWnd, GWL_USERDATA);
		if(!pWindow) return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	pWindow->m_hWnd = hWnd;
	return pWindow->WndProc(msg, wParam, lParam);
}

LRESULT CALLBACK MWindow::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(Exit) return 0;
	
	switch (msg)
	{
		case WM_MOUSEMOVE:
			if(!MouseInClient)
			{
				MouseInClient = true;
		        TRACKMOUSEEVENT tme = { sizeof(tme) };
		        tme.dwFlags = TME_LEAVE;
		        tme.hwndTrack = m_hWnd;
		        TrackMouseEvent(&tme);
			}
			else OnMouseHover(wParam, lParam);
			break;
		case WM_MOUSELEAVE: 
			MouseInClient = false;
			break;
		case WM_LBUTTONDOWN:
			OnMouseLButton(wParam, lParam);
			break;
		case WM_ACTIVATE:
			OnActivate(wParam);
			break;
		case WM_KEYDOWN:
			OnKeyDown(wParam, lParam);
			break;
		case WM_KEYUP:
            OnKeyUp(wParam, lParam);
			break;
		case WM_SIZE:
			ResizeScene(LOWORD(lParam), HIWORD(lParam));
			OnSize();
			break;
		case WM_CLOSE:
		case WM_DESTROY:
			Exit = true;
            OnClose();
            Resolutions.clear();
			CloseGL();
			PostQuitMessage(0);
			break;
		case WM_ERASEBKGND://IMPORTANT!
    		break;
		default:
			return DefWindowProc(m_hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

void MWindow::Run()
{
	if(!pFrameWaitFunc)
	{
		LogFile<<"Window: Some of frame functions are not inited"<<endl;
		return;
	}
	
	//show window
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
	
	//start calculate timing
	TickCount = GetTickCount();
	
	MSG msg = {0};
	while (!Exit)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		OnDraw();
		SwapBuffers(m_hDC);
		TickDelta = GetTickCount() - TickCount;
		TickCount = GetTickCount();
		if(pFrameWaitFunc) ((*this).*(pFrameWaitFunc))();//if vsync is not enabled we must controll fps
	}
}

HWND MWindow::GetHWND()
{
	return m_hWnd;
}

bool MWindow::InitResolutions()
{	
	InitialResolution = CurrentResolution = GetCurrentResolution();
	if(!GetSupportedResolutions(&Resolutions))
	{
		LogFile<<"Window: Can't get supported resolutions"<<endl;
		return false;
	}
	LogFile<<"Window: Current monitor resolution: "<<CurrentResolution.X<<"x"<<CurrentResolution.Y<<endl;
	LogFile<<"Window: Current window size: "<<WindowWidth<<"x"<<WindowHeight<<endl;
	
	return true;
}

bool MWindow::EnterFullscreen()
{	
	DEVMODE NewSettings;
	memset(&NewSettings, 0, sizeof(DEVMODE));
	EnumDisplaySettings(NULL, 0, &NewSettings);
	NewSettings.dmPelsWidth = CurrentResolution.X;
	NewSettings.dmPelsHeight = CurrentResolution.Y;
	NewSettings.dmBitsPerPel = 32;
	NewSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	
	//change window style
	if(!Fullscreen)
	{
		if(!SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE)) LogFile<<"Window: SetWindowLong failed"<<endl;
	}
	if(!SetWindowPos(m_hWnd, 0, 0, 0, CurrentResolution.X, CurrentResolution.Y, SWP_NOZORDER | SWP_NOACTIVATE | SWP_DRAWFRAME)) LogFile<<"Window: SetWidowPos failed"<<endl;
	
	LONG Result = ChangeDisplaySettings(&NewSettings, CDS_FULLSCREEN);
	switch(Result)
	{
		case DISP_CHANGE_SUCCESSFUL:
			Fullscreen = true;
			break;
		case DISP_CHANGE_BADDUALVIEW:
			LogFile<<"Window: The settings change was unsuccessful because the system is DualView capable"<<endl;
			return false;
		case DISP_CHANGE_BADFLAGS:
			LogFile<<"Window: An invalid set of flags was passed in"<<endl;
			return false;
		case DISP_CHANGE_BADMODE:
			LogFile<<"Window: The graphics mode is not supported"<<endl;
			return false;
		case DISP_CHANGE_BADPARAM:
			LogFile<<"Window: An invalid parameter was passed in"<<endl;
			return false;
		case DISP_CHANGE_FAILED:
			LogFile<<"Window: The display driver failed the specified graphics mode"<<endl;
			return false;
		case DISP_CHANGE_NOTUPDATED:
			LogFile<<"Window: Unable to write settings to the registry"<<endl;
			return false;
		case DISP_CHANGE_RESTART:
			LogFile<<"Window: The computer must be restarted for the graphics mode to work"<<endl;
			return false;
	}
	
	return true;
}

bool MWindow::ExitFullscreen(int Width, int Height)
{
	//exit from fullscreen
	if(!Fullscreen)
	{
		LogFile<<"Window: alredy windowed"<<endl;
		return false;
	}
	
	DEVMODE NewSettings;
	memset(&NewSettings, 0, sizeof(DEVMODE));
	EnumDisplaySettings(NULL, 0, &NewSettings);
	NewSettings.dmPelsWidth = CurrentResolution.X;
	NewSettings.dmPelsHeight = CurrentResolution.Y;
	NewSettings.dmBitsPerPel = 32;
	NewSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	
	LONG Result = ChangeDisplaySettings(&NewSettings, 0);
	switch(Result)
	{
		case DISP_CHANGE_SUCCESSFUL:
			Fullscreen = false;
			break;
		case DISP_CHANGE_BADDUALVIEW:
			LogFile<<"Window: The settings change was unsuccessful because the system is DualView capable"<<endl;
			return false;
		case DISP_CHANGE_BADFLAGS:
			LogFile<<"Window: An invalid set of flags was passed in"<<endl;
			return false;
		case DISP_CHANGE_BADMODE:
			LogFile<<"Window: The graphics mode is not supported"<<endl;
			return false;
		case DISP_CHANGE_BADPARAM:
			LogFile<<"Window: An invalid parameter was passed in"<<endl;
			return false;
		case DISP_CHANGE_FAILED:
			LogFile<<"Window: The display driver failed the specified graphics mode"<<endl;
			return false;
		case DISP_CHANGE_NOTUPDATED:
			LogFile<<"Window: Unable to write settings to the registry"<<endl;
			return false;
		case DISP_CHANGE_RESTART:
			LogFile<<"Window: The computer must be restarted for the graphics mode to work"<<endl;
			return false;
	}
	
	if(!SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE ))
	{
		LogFile<<"Window: SetWindowLong failed"<<endl;
		return false;
	}
	if(!ChangeWindowSize(Width, Height)) return false;
	
	return true;
}

bool MWindow::ChangeWindowSize(int Width, int Height)
{
	if(!MoveWindow(m_hWnd, 0, 0, Width + WindowFix[0], Height + WindowFix[1], true))
	{
		LogFile<<"Window: Failed to move window"<<endl;
		return false;
	}
	WindowWidth = Width;
	WindowHeight = Height;
	
	return true;
}

void MWindow::OnDraw(){}
void MWindow::OnActivate(WPARAM wParam){}
void MWindow::OnKeyUp(WPARAM wParam, LPARAM lParam){}
void MWindow::OnKeyDown(WPARAM wParam, LPARAM lParam){}
void MWindow::OnClose(){}
void MWindow::OnSize(){}
void MWindow::OnMouseHover(WPARAM wParam, LPARAM lParam){}
void MWindow::OnMouseLButton(WPARAM wParam, LPARAM lParam){}


