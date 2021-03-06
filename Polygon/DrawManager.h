#pragma once
#include "VertexManager.h"
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>

class DrawManager
{
private:
	VertexManager m_VertexManager;

public:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
	Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_Swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
	D3D11_VIEWPORT m_ViewPort;

	DrawManager() {};
	~DrawManager() { m_Context->ClearState();  };

	HRESULT Create(HWND hwnd);
	void Render();
};

