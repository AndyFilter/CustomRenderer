#pragma once

#ifndef SRC_GRAPHICS_GRAPHICS_H
#define SRC_GRAPHICS_GRAPHICS_H

#include <d3d11.h>
#include <vector>

namespace GRAPHICS
{
	extern IDXGISwapChain* g_pSwapChain;
	extern ID3D11Device* g_pd3dDevice;
	extern ID3D11DeviceContext* g_pd3dDeviceContext;
	extern std::vector<IDXGIOutput*> vOutputs;
	extern std::vector<IDXGIAdapter*> vAdapters;
	extern ID3D11Texture2D* g_pBackBuffer;
	extern D3D11_VIEWPORT* g_pViewport;
	extern ID3D11RenderTargetView* g_mainRenderTargetView;

	extern ID3D11Buffer* g_VertexBuffer;
	extern ID3D10Blob* g_VertexShaderCode, * g_PixelShaderCode;
	extern ID3D11VertexShader* g_VertexShader;
	extern ID3D11PixelShader* g_PixelShader;
	extern ID3D11InputLayout* g_InputLayout;

	bool Setup(int (*DrawGuiFunc)(), HWND hwnd);
	void Destroy();
	int RenderFrame();

	void CreateRenderTarget();
	void CleanupRenderTarget();
}

#endif // !SRC_GRAPHICS_GRAPHICS_H
