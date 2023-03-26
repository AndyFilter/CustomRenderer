#include <tchar.h>
#include "../External/ImGui/imgui.h"
#include "../External/ImGui/imgui_impl_win32.h"
#include "../External/ImGui/imgui_impl_dx11.h"

#include "gui.h"
#include "../Graphics/Graphics.h"

using namespace GUI;

static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const int windowX = 1200, windowY = 620;

static WNDCLASSEX wc;
static HWND hwnd;

static int (*mainGuiFunc)();

// Setup code, takes a function to run when doing GUI
HWND GUI::Setup(HINSTANCE instance, int (*OnGuiFunc)())
{
	if (OnGuiFunc != NULL)
		mainGuiFunc = OnGuiFunc;
	else
		return nullptr;

	// Create application window
	wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("D3D11 With ImGui Example"), NULL };
	::RegisterClassEx(&wc);

	hwnd = ::CreateWindowW(wc.lpszClassName, _T("D3D11 With ImGui Example"), (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX), 200, 100, windowX, windowY, NULL, NULL, wc.hInstance, NULL);

	// Initialize Graphics
	if (!GRAPHICS::Setup(DrawGui, hwnd))
	{
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return NULL;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(GRAPHICS::g_pd3dDevice, GRAPHICS::g_pd3dDeviceContext);

	return hwnd;
}

int GUI::DrawGui() noexcept
{
	static bool done = false;

	MSG msg;
	while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
		    done = true;
	}
	if (done)
		return 1;


	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//Create ImGui Test Window
	ImGui::Begin("Test");
	// Back to main to draw whatever GUI we want
	if (int GuiRetVal = mainGuiFunc())
		return GuiRetVal;
	ImGui::End();
	//Assemble Together Draw Data
	ImGui::Render();
	//Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return 0;
}

void GUI::Destroy() noexcept
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	using namespace GRAPHICS;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			GRAPHICS::CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			GRAPHICS::CreateRenderTarget();

			// Resize viewport
			g_pViewport->Width = (UINT)LOWORD(lParam);
			g_pViewport->Height = (UINT)HIWORD(lParam);
			g_pd3dDeviceContext->RSSetViewports(1, g_pViewport);
		}
		return 0;
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = windowX;
		lpMMI->ptMinTrackSize.y = windowY;
		break;
	}
	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE)
			break;

		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;

		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}


	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
