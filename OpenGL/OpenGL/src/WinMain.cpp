#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define WIN_32_EXTRA_LEAN

#include "glad/glad.h"
#include <Windows.h>
#include <iostream>
#include "Application.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int);


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program


#if _DEBUG 
	#pragma comment (linker, "/subsystem:console")
	int main(int argc, const char** argv) {
		return WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWDEFAULT);
	}
#else 
	#pragma comment (linker, "/subsystem:windows")
#endif
#pragma comment(lib, "opengl32.lib")

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0X2092
#define WGL_CONTEXT_FLAGS_ARB 0X2094
#define WGL_CONTEXT_COREPROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);

typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC)(void);
typedef BOOL (WINAPI* PFNWGLSWAPINTERVALEXTPROC)(int);
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

Application* gApplication = nullptr;
GLuint gVertexArrayObject = 0;

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int ICmdShow) {
	gApplication = new Application();

	const wchar_t CLASS_NAME[] = L"Win32 Game Window"; //Search more...


	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexa
	WNDCLASSEX  wndclass;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = 0;
	wndclass.style = CS_HREDRAW | CS_VREDRAW; // ACTIVATES RELOAD ON REDRAW
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hinstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndclass.lpszMenuName = 0;
	wndclass.lpszClassName = CLASS_NAME;

	RegisterClassEx(&wndclass);


	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	int clientWidth = 800;
	int clientHeight = 600;

	RECT windowRect;
	//https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setrect
	SetRect(&windowRect, (ScreenWidth / 2) - (clientWidth / 2),
		(ScreenHeight / 2) - (clientHeight / 2),
		(ScreenWidth / 2) + (clientWidth / 2),
		(ScreenHeight / 2) + (clientHeight / 2));


	// https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	DWORD style = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);

	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-adjustwindowrectex
	AdjustWindowRectEx(&windowRect, style, FALSE, 0);
	const wchar_t windowName[] = L"OpenGL Window";

	//DWORD secondaryStyle = (WS_EX_ACCEPTFILES);

	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa
	HWND hwnd = CreateWindowEx(0, wndclass.lpszClassName, windowName, style, windowRect.left, windowRect.top, windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top, NULL, NULL, hinstance, szCmdLine);
	HDC hdc = GetDC(hwnd);

	//https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getdc
	PIXELFORMATDESCRIPTOR pfd;
	// https://www.cplusplus.com/reference/cstring/memset/
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;

	//https://en.wikipedia.org/wiki/Stencil_buffer#:~:text=A%20stencil%20buffer%20is%20an,RAM%20of%20the%20graphics%20hardware.
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 8;

	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd );


	HGLRC tempRC = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempRC);
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

	const int attribList[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB,
		WGL_CONTEXT_COREPROFILE_BIT_ARB, 0, 
	};
	
	HGLRC hglrc = wglCreateContextAttribsARB(hdc, 0, attribList);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempRC);
	wglMakeCurrent(hdc, hglrc);

	if (!gladLoadGL())
	{
		std::cout << "Could not initialize GLAD \n";
	}
	else {
		std::cout << "OpenGL Version " << GLVersion.major << std::endl;
	}

	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
	bool swapControlSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;
	//https://www.khronos.org/opengl/wiki/Swap_Interval
	int vsynch = 0;

	if (swapControlSupported) {
		PFNWGLSWAPINTERVALEXTPROC wglSwapInternalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
		if (wglSwapInternalEXT(1))
		{
			std::cout << "VSynch enabled \n";
		}
		else
		{
			std::cout << "Could not enable VSynch";
		}
	}
	else
	{
		std::cout << "WGL_EXT_swap_control not supported \n";
	}

	glGenVertexArrays(1, &gVertexArrayObject);
	glBindVertexArray(gVertexArrayObject);

	// Shows window
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	gApplication->Initialize();


	DWORD lastTick = GetTickCount();
	MSG msg;
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DWORD thisTick = GetTickCount();
		float dt = float(thisTick - lastTick) * 0.001f;
		lastTick = thisTick;

		if (gApplication != nullptr) {
			gApplication->Update(dt);


			RECT clientRect;
			GetClientRect(hwnd, &clientRect);
			clientWidth = clientRect.right - clientRect.left;
			clientHeight = clientRect.bottom - clientRect.top;
			glViewport(0, 0, clientWidth, clientHeight);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glPointSize(5.0f);
			glBindVertexArray(gVertexArrayObject);
			glClearColor(1.0f, 0.0f, .7f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			float aspect = (float)clientWidth / (float)clientHeight;
			
			gApplication->Render(aspect);

			SwapBuffers(hdc);
			if (vsynch != 0)
				glFinish();
		}
			
	}

	if (gApplication != nullptr) {
		std::cout << "Expected application to be null on exit \n";
		delete gApplication;
	}

	return (int)msg.wParam;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	switch (iMsg) {
	case WM_CLOSE:
		if (gApplication != nullptr) {
			gApplication->Shutdown();
			delete gApplication;
			gApplication = 0;
			DestroyWindow(hwnd);
		}
		else {
			std::cout << "Already shut down! \n";
		}
		break;

	case WM_DESTROY:
		if (gVertexArrayObject != 0) {
			HDC hdc = GetDC(hwnd);
			HGLRC hglrc = wglGetCurrentContext();

			glBindVertexArray(0);
			glDeleteVertexArrays(1, &gVertexArrayObject);
			gVertexArrayObject = 0;

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hglrc);
			ReleaseDC(hwnd, hdc);

			PostQuitMessage(0);

		}
		else {
			std::cout << "Multiple destroy messages \n";
		}
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}