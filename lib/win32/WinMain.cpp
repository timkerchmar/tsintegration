#include <windowsx.h>
#include "TSIntegration.h"
#include "TSThread.h"

HGLRC hRC;				/* opengl context */
HWND  hWnd;				/* window */
HDC hDC = 0;
HINSTANCE hInstance = 0;

LONG WINAPI
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void TSIntegration::createWindow()
{
	/* only register the window class once - use hInstance as a flag. */
	if (!hInstance) {
		hInstance = GetModuleHandle(NULL);
		WNDCLASS    wc;
		wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = (WNDPROC)WindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = L"OpenGL";

		if (!RegisterClass(&wc)) {
			MessageBox(NULL, L"RegisterClass() failed:  "
				L"Cannot register window class.", L"Error", MB_OK);
			exit(1);
		}
	}

	int width = 1900, height = 580;

	hWnd = CreateWindow(L"OpenGL", L"TSIntegration",
		WS_OVERLAPPEDWINDOW, 0, 0, width, height, NULL, NULL, hInstance, NULL);
	if (hWnd == NULL) {
		MessageBox(NULL, L"CreateWindow() failed:  Cannot create a window.",
			L"Error", MB_OK);
		exit(1);
	}

	hDC = GetDC(hWnd);

	/* there is no guarantee that the contents of the stack that become
	the pfd are zeroed, therefore _make sure_ to clear these bits. */
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SWAP_EXCHANGE | PFD_GENERIC_ACCELERATED | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 0;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int         pf;
	pf = ChoosePixelFormat(hDC, &pfd);
	if (pf == 0) {
		MessageBox(NULL, L"ChoosePixelFormat() failed:  "
			L"Cannot find a suitable pixel format.", L"Error", MB_OK);
		exit(1);
	}

	if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
		MessageBox(NULL, L"SetPixelFormat() failed:  "
			L"Cannot set format specified.", L"Error", MB_OK);
		exit(1);
	}

	DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	ReleaseDC(hWnd, hDC);
}

void TSIntegration::setPixelFormat()
{
}

void TSIntegration::createContext()
{
    hRC = wglCreateContext(hDC);

	TSIntegration::makeContextCurrent();

	glewInit();

	if (GLEW_VERSION_2_1) printf("Core extensions of OpenGL 2.1 are available!\n");
	else if (GLEW_VERSION_2_0) printf("Core extensions of OpenGL 2.0 are available!\n");
	else if (GLEW_VERSION_1_5) printf("Core extensions of OpenGL 1.5 are available!\n");
	else if (GLEW_VERSION_1_4) printf("Core extensions of OpenGL 1.4 are available!\n");
	else if (GLEW_VERSION_1_3) printf("Core extensions of OpenGL 1.3 are available!\n");
	else if (GLEW_VERSION_1_2) printf("Core extensions of OpenGL 1.2 are available!\n");
	else if (GLEW_VERSION_1_1) printf("Core extensions of OpenGL 1.1 are available!\n");

	if (!GLEW_VERSION_2_1)
	{
		printf("This application requires at least OpenGL 2.1\n");
		return;
	}
}

void TSIntegration::makeContextCurrent()
{
	wglMakeCurrent(hDC, hRC);
}

void TSIntegration::clearContext()
{
    wglMakeCurrent(NULL, NULL);
}

void TSIntegration::enableDisplay()
{
}

void TSIntegration::disableDisplay()
{
}

void TSIntegration::displaySize(GLint& width, GLint& height)
{
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	width = clientRect.right - clientRect.left;
	height = clientRect.bottom - clientRect.top;
}

void TSIntegration::setSwapInterval()
{
	wglSwapIntervalEXT(1);
}

void TSIntegration::pageFlip()
{
	SwapBuffers(hDC);
}

//==================================================================================================
// PLATFORM EVENT SCAFFOLDING
//==================================================================================================
LONG WINAPI
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static PAINTSTRUCT ps;

	switch (uMsg) {
	case WM_SHOWWINDOW:
		if (wParam)
		{
			TSIntegration::start();
		}
		else
		{
			TSIntegration::stop();
		}
		return 0;
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED || wParam == SIZE_MAXHIDE)
		{
			TSIntegration::stop();
		}
		else
		{
			TSIntegration::start();
		}
		return 0;
	case WM_SIZING:
	{
		TSIntegration::start();

		return 0;
	}

	case WM_CHAR:
		switch (wParam) {
		case 27:			/* ESC key */
			PostQuitMessage(0);
			break;
		}
		return 0;

	case WM_PAINT:

		return 0;

	case WM_LBUTTONDOWN:
	{
		if (GetFocus() != hWnd) SetFocus(hWnd);

		short x = GET_X_LPARAM(lParam);
		short y = GET_Y_LPARAM(lParam);
		TSTouchAdded(x, y, 0);
	}
	return 0;

	case WM_LBUTTONUP:
	{
		short x = LOWORD(lParam);
		short y = HIWORD(lParam);
		TSTouchRemoved(x, y, 0);
	}
		return 0;

	case WM_MOUSEMOVE:
	{
		short x = LOWORD(lParam);
		short y = HIWORD(lParam);

		RECT clientRect;
		GetClientRect(hWnd, &clientRect);

		if (x < clientRect.left || x >= clientRect.right ||
			y < clientRect.top || y >= clientRect.bottom) {

			if (GetCapture() == hWnd) ReleaseCapture();
			TSTouchRemoved(x, y, 0);
		}
		else {
			if (GetCapture() != hWnd) SetCapture(hWnd);
			TSTouchMoved(x, y, 0);
		}
	}
	return 0;

	case WM_MOUSEWHEEL:
	{
		short x = LOWORD(lParam);
		short y = HIWORD(lParam);

		RECT clientRect;
		GetClientRect(hWnd, &clientRect);

		if (x < clientRect.left || x >= clientRect.right ||
			y < clientRect.top || y >= clientRect.bottom)
		{
			if (GetCapture() == hWnd) ReleaseCapture();
			TSTouchRemoved(x, y, 0);
		}
		else
		{
			if (GetCapture() != hWnd) SetCapture(hWnd);
			//TSTouch::mouseMove(x, y);
		}
	}
	return 0;

	case WM_CLOSE:
		timeEndPeriod(3); // Must contain same arg as timeBeginPeriod and must be called immediately when high resolution is no longer necessary.

		TSIntegration::shutDown();

		ReleaseDC(hWnd, hDC);
		wglDeleteContext(hRC);
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int APIENTRY
WinMain(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst,
	LPSTR lpszCmdLine, int nCmdShow)
{
	TSIntegration::initialize();
	
	// Resize window so that client area width, height. 
	// Without this code, I see 1018, 740 instead of 1024, 768
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	int width = 1900, height = 580;
	MoveWindow(hWnd, 0, 0,
		width + (width - (clientRect.right - clientRect.left)),
		height + (height - (clientRect.bottom - clientRect.top)),
		false);
	SetForegroundWindow(hWnd);
	ShowWindow(hWnd, nCmdShow);

	// Make the game run smoothly
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	timeBeginPeriod(3); 

	MSG   msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) 
	{
		if (msg.message == WM_QUIT) break;


		TranslateMessage(&msg);


		DispatchMessage(&msg);
	}

    return msg.wParam;
}