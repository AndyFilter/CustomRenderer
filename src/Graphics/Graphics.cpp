#include <string>
#include <cassert>
#include <direct.h>
#include <d3dcompiler.h>

#include "Graphics.h"


using namespace GRAPHICS;

IDXGISwapChain* GRAPHICS::g_pSwapChain = nullptr;
ID3D11Device* GRAPHICS::g_pd3dDevice = nullptr;
ID3D11DeviceContext* GRAPHICS::g_pd3dDeviceContext = nullptr;
std::vector<IDXGIOutput*> GRAPHICS::vOutputs = std::vector<IDXGIOutput*>();
std::vector<IDXGIAdapter*> GRAPHICS::vAdapters = std::vector<IDXGIAdapter*>();
ID3D11Texture2D* GRAPHICS::g_pBackBuffer = nullptr;
D3D11_VIEWPORT* GRAPHICS::g_pViewport = nullptr;
ID3D11RenderTargetView* GRAPHICS::g_mainRenderTargetView = nullptr;

ID3D11Buffer* GRAPHICS::g_VertexBuffer = nullptr;
ID3D10Blob* GRAPHICS::g_VertexShaderCode = nullptr, * GRAPHICS::g_PixelShaderCode = nullptr;
ID3D11VertexShader* GRAPHICS::g_VertexShader = nullptr;
ID3D11PixelShader* GRAPHICS::g_PixelShader = nullptr;
ID3D11InputLayout* GRAPHICS::g_InputLayout = nullptr;


const int windowX = 1200, windowY = 620;


// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int (*OnGuiFunc)();

bool GRAPHICS::Setup(int (*DrawGuiFunc)(), HWND hwnd)
{
	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		return false;
	}

	OnGuiFunc = DrawGuiFunc;

	g_pViewport = new D3D11_VIEWPORT();
	g_pViewport->TopLeftX = 0;
	g_pViewport->TopLeftY = 0;
	g_pViewport->Width = (FLOAT)windowX;
	g_pViewport->Height = (FLOAT)windowY;
	g_pViewport->MinDepth = 0.f;
	g_pViewport->MaxDepth = 1.f;
	g_pd3dDeviceContext->RSSetViewports(1, g_pViewport);


	struct Vertex
	{
		float Position[3];
		float Color[3];
		float UV[2];
	};

	// single triangle
	//Vertex myVertices[] = {
	//	{  0.0f,  0.9f, 0.f, {1,0,0}, {0.5f, 0.f} },
	//	{  0.9f, -0.9f, 0.f, {0,1,0}, {1.0f, 1.f} },
	//	{ -0.9f, -0.9f, 0.f, {0,0,0}, {0.f, 1.f} },
	//};

	// 2 triangles (rectangle)
	Vertex myVertices[] = {
	{  -0.9f,  0.9f, 0.f, {1,0,0}, {0.f, 1.f} },
	{  0.9f, -0.9f, 0.f, {0,1,0}, {1.0f, 0.f} },
	{ -0.9f, -0.9f, 0.f, {0,0,0}, { 0.f, 0.f} },
	{  -0.9f,  0.9f, 0.f, {1,0,0}, {0.0f, 1.f} },
	{  0.9f, 0.9f, 0.f, {0,1,0}, {1.0f, 1.f} },
	{ 0.9f, -0.9f, 0.f, {0,0,0}, {1.f, 0.f} },
	};

	D3D11_BUFFER_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(bufDesc));
	bufDesc.ByteWidth = sizeof(Vertex) * _countof(myVertices);
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA bufData;
	ZeroMemory(&bufData, sizeof(bufData));
	bufData.pSysMem = myVertices;

	HRESULT hr = g_pd3dDevice->CreateBuffer(&bufDesc, &bufData, &g_VertexBuffer);
	assert(SUCCEEDED(hr));


	// Compile and create vertex shader

	std::string path = "";
	char pBuf[1024];

 	auto _tmp = _getcwd(pBuf, 1024);
	path = pBuf;
	path += "\\";
	std::wstring wpath = std::wstring(path.begin(), path.end());

	std::wstring vertFragPath = wpath + L"src\\Graphics\\BasicVertexShader.hlsl";

	ID3D10Blob* errorBlob = 0;

	hr = D3DCompileFromFile(vertFragPath.c_str(), nullptr, nullptr, "main",
		"vs_5_0", 0, 0, &g_VertexShaderCode,
		&errorBlob);

	assert(SUCCEEDED(hr));

	hr = g_pd3dDevice->CreateVertexShader(g_VertexShaderCode->GetBufferPointer(), g_VertexShaderCode->GetBufferSize(), 0, &g_VertexShader);
	assert(SUCCEEDED(hr));

	if (errorBlob) errorBlob->Release();


	// Compile and create pixel shader

	std::wstring pixelFragPath = wpath + L"src\\Graphics\\BasicPixelShader.hlsl";

	errorBlob = 0;

	hr = D3DCompileFromFile(pixelFragPath.c_str(), nullptr, nullptr, "main",
		"ps_5_0", 0, 0, &g_PixelShaderCode,
		&errorBlob);
	assert(SUCCEEDED(hr));

	hr = g_pd3dDevice->CreatePixelShader(g_PixelShaderCode->GetBufferPointer(), g_PixelShaderCode->GetBufferSize(), 0, &g_PixelShader);
	assert(SUCCEEDED(hr));

	if (errorBlob) errorBlob->Release();


	// Create input layout

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = g_pd3dDevice->CreateInputLayout(inputDesc, _countof(inputDesc), g_VertexShaderCode->GetBufferPointer(), g_VertexShaderCode->GetBufferSize(), &g_InputLayout);
	assert(SUCCEEDED(hr));


	// Set all the buffers and shaders

	UINT vbStride = sizeof(Vertex), vbOffset = 0;
	g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &g_VertexBuffer, &vbStride, &vbOffset);
	g_pd3dDeviceContext->IASetInputLayout(g_InputLayout);
	g_pd3dDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_pd3dDeviceContext->VSSetShader(g_VertexShader, 0, 0);
	g_pd3dDeviceContext->PSSetShader(g_PixelShader, 0, 0);

	return true;
}

void GRAPHICS::Destroy()
{
	CleanupDeviceD3D();
}


int GRAPHICS::RenderFrame()
{
	float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.f };
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clearColor);

	// We render a rectangle (2 traingles) which comes up to be 6 vertices.
	// If you want to render a single triangle you would change the 6 to 3.
	g_pd3dDeviceContext->Draw(6, 0);

	if (int GuiRetVal = OnGuiFunc())
		return GuiRetVal;

	auto presentRes = g_pSwapChain->Present(1, 0);
	if (presentRes == DXGI_STATUS_OCCLUDED)
		Sleep(10);

	return 0;
}


bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0; // 60
	sd.BufferDesc.RefreshRate.Denominator = 0; // 1
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	GRAPHICS::CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	GRAPHICS::CleanupRenderTarget();
	g_pSwapChain->SetFullscreenState(FALSE, NULL);
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	if (g_pBackBuffer) { g_pBackBuffer->Release(); g_pBackBuffer = NULL; };

	if (g_InputLayout) g_InputLayout->Release();
	if (g_PixelShader) g_PixelShader->Release();
	if (g_VertexShader) g_VertexShader->Release();
	if (g_PixelShaderCode) g_PixelShaderCode->Release();
	if (g_VertexShaderCode) g_VertexShaderCode->Release();
	if (g_VertexBuffer) g_VertexBuffer->Release();
}

void GRAPHICS::CreateRenderTarget()
{
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&g_pBackBuffer));
	if (!g_pBackBuffer)
		ExitProcess(1);
	g_pd3dDevice->CreateRenderTargetView(g_pBackBuffer, NULL, &g_mainRenderTargetView);
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	g_pBackBuffer->Release();
}

void GRAPHICS::CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}