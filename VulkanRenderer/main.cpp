// VulkanRenderer.cpp : Defines the entry point for the application.
//

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "resource.h"

#include <BaseInclude.h>
#include <../RHI/VulkanRHI/VulkanDevice.h>
#include "../Source/Helper/AssetManager.h"
#include "../Source/RHI/VulkanRHI/VulkanCommand.h"
#include "../Source/Framework/Framework.h"
#include "../Source/Component/InputStateData.h"
#include "../Source/Math/MathGlobal.h"

#define MAX_LOADSTRING 100

#define KEY_W 0x57
#define KEY_A 0x41
#define KEY_S 0x53
#define KEY_D 0x44
#define KEY_Q 0x51
#define KEY_E 0x45
#define VK_LSHIFT 0xA0
#define VK_RSHIFT 0xA1

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

const int width = 1024;
const int height = 768;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void SetupConsole(std::string title);

HWND g_window;
Core::Framework g_frameWork;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VULKANRENDERER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	SetupConsole("Vulkan validation output");

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VULKANRENDERER));

	LARGE_INTEGER frequence;
	QueryPerformanceFrequency(&frequence);

	LARGE_INTEGER lastTime;
	lastTime.LowPart = 0;
	lastTime.HighPart = 0;
	LARGE_INTEGER currentTime;

	std::unique_ptr<Core::VulkanDevice> vulkanDevice = std::make_unique<Core::VulkanDevice>();
	vulkanDevice->Initialize(hInstance, g_window);
	vulkanDevice->PrepareSTH();

	std::shared_ptr<Core::AssetManager> assetManager = std::make_shared<Core::AssetManager>();
	assetManager->Scan();

	g_frameWork.SetCurrentWorld(assetManager->GetDefaultWorld());
	g_frameWork.Initialize(vulkanDevice.get(), 1.0f * width / height);

	//assetManager->UploadToGPU(vulkanDevice.get());
	//vulkanDevice->PrepareSTH();

	//assetManager->CommitToDrawList(vulkanDevice.get());

	vulkanDevice->BuildCommandBuffers();

	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			float elasped = 0;

			if (lastTime.LowPart == 0 && lastTime.HighPart == 0)
			{
				QueryPerformanceCounter(&lastTime);
			}
			else
			{
				QueryPerformanceCounter(&currentTime);

				elasped = ((currentTime.QuadPart - lastTime.QuadPart) * 1000.0f) / frequence.QuadPart;
				elasped /= 1000.0f;
				lastTime = currentTime;
			}

			g_frameWork.Tick(elasped, vulkanDevice.get(), Core::InputStateData::Instance());

			vulkanDevice->Draw();

			Core::InputStateData::Instance()->mouseIsMoveing = False;
		}
	}

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VULKANRENDERER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_VULKANRENDERER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   RECT windowRect;
   windowRect.left = 0L;
   windowRect.top = 0L;
   windowRect.right = width;
   windowRect.bottom = height;

   DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
   DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN; 

   AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

   g_window = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, nullptr);

   if (!g_window)
   {
      return FALSE;
   }

   ShowWindow(g_window, nCmdShow);
   UpdateWindow(g_window);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CLOSE:
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case KEY_W:
			Core::InputStateData::Instance()->wDown = True;
			break;
		case KEY_S:
			Core::InputStateData::Instance()->sDown = True;
			break;
		case KEY_A:
			Core::InputStateData::Instance()->aDown = True;
			break;
		case KEY_D:
			Core::InputStateData::Instance()->dDown = True;
			break;
		case KEY_Q:
			Core::InputStateData::Instance()->qDown = True;
			break;
		case KEY_E:
			Core::InputStateData::Instance()->eDown = True;
			break;
		case VK_LSHIFT:
			Core::InputStateData::Instance()->leftShiftDown = True;
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case KEY_W:
			Core::InputStateData::Instance()->wDown = False;
			break;
		case KEY_S:
			Core::InputStateData::Instance()->sDown = False;
			break;
		case KEY_A:
			Core::InputStateData::Instance()->aDown = False;
			break;
		case KEY_D:
			Core::InputStateData::Instance()->dDown = False;
			break;
		case KEY_Q:
			Core::InputStateData::Instance()->qDown = False;
			break;
		case KEY_E:
			Core::InputStateData::Instance()->eDown = False;
			break;
		case VK_LSHIFT:
			Core::InputStateData::Instance()->leftShiftDown = False;
			break;
		}
		break;
	case WM_RBUTTONDOWN:
		Core::InputStateData::Instance()->rightBuffonDown = True;
		Core::InputStateData::Instance()->xPos = LOWORD(lParam);
		Core::InputStateData::Instance()->yPos = HIWORD(lParam);
		Core::InputStateData::Instance()->xPosPrev = LOWORD(lParam);
		Core::InputStateData::Instance()->yPosPrev = HIWORD(lParam);
		break;
	case WM_RBUTTONUP:
		Core::InputStateData::Instance()->rightBuffonDown = False;
		break;
	case WM_MOUSEMOVE:
		Core::InputStateData::Instance()->xPosPrev = Core::InputStateData::Instance()->xPos;
		Core::InputStateData::Instance()->yPosPrev = Core::InputStateData::Instance()->yPos;
		Core::InputStateData::Instance()->xPos = LOWORD(lParam);
		Core::InputStateData::Instance()->yPos = HIWORD(lParam);
		Core::InputStateData::Instance()->mouseIsMoveing = True;
		break;
	case WM_MOUSEWHEEL:
		Core::InputStateData::Instance()->zPos += (short)HIWORD(wParam) > 0 ? 0.3f : -0.3f;
		Core::InputStateData::Instance()->zPos = Core::ClampToLeft(Core::InputStateData::Instance()->zPos, 0.0f);
		break;
	case WM_SIZE:
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void SetupConsole(std::string title)
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w+", stdout);
	freopen_s(&stream, "CONOUT$", "w+", stderr);
	SetConsoleTitle(TEXT(title.c_str()));
}
