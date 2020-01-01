#include "DrawManager.h"
#include "VertexManager.h"
#include <atltypes.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

HRESULT DrawManager::Create(HWND hwnd)
{

	//////////////////////////////////////////////////////////////////////////////////
	//ウィンドウサイズ取得
	//////////////////////////////////////////////////////////////////////////////////
	CRect rect;
	GetClientRect(hwnd, &rect);


	//////////////////////////////////////////////////////////////////////////////////
	//デバイスとスワップチェインの生成
	//////////////////////////////////////////////////////////////////////////////////
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	desc.BufferCount = 1;									// スワップチェインのバッファ数
	desc.BufferDesc.Width = rect.Width();					// スワップチェインのバッファサイズ
	desc.BufferDesc.Height = rect.Height();					// スワップチェインのバッファサイズ
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// スワップチェインのバッファフォーマット
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// バッファをレンダーターゲットとして使用
	desc.OutputWindow = hwnd;								// HWNDハンドル
	desc.SampleDesc.Count = 1;								// マルチサンプリングのピクセル単位の数
	desc.SampleDesc.Quality = 0;							// マルチサンプリングの品質
	desc.Windowed = TRUE;									// ウィンドウモード

	D3D_FEATURE_LEVEL level;

	HRESULT hresult = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,					//有効にするランタイムレイヤー
		nullptr,			//作成を試みる機能レベルの順序を指定する配列
		0,					//作成を試みる機能レベルの順序を指定する配列の数
		D3D11_SDK_VERSION,	//D3D11_SDK_VERSIONを指定
		&desc,				//スワップチェインの初期化パラメーター
		&m_Swapchain,		//作成されるスワップチェイン
		&m_Device,			//作成されるデバイス
		&level,				//作成されたデバイスの機能レベル
		&m_Context			//作成されるデバイスコンテキスト
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//バックバッファの取得
	//////////////////////////////////////////////////////////////////////////////////
	ID3D11Texture2D* backBuffer;
	hresult = m_Swapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//レンダリングターゲットの生成
	//////////////////////////////////////////////////////////////////////////////////
	hresult = m_Device->CreateRenderTargetView(backBuffer, nullptr, &m_RenderTargetView);
	backBuffer->Release();
	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//頂点バッファの生成
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = sizeof(Vertex) * m_VertexManager.getVertexNum();
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresource;
	ZeroMemory(&subresource, sizeof(D3D11_SUBRESOURCE_DATA));
	subresource.pSysMem = m_VertexManager.getVertexList();
	subresource.SysMemPitch = 0;
	subresource.SysMemSlicePitch = 0;

	hresult = m_Device->CreateBuffer(&bufferDesc, &subresource, &m_VertexBuffer);
	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//VertexShaderの生成
	//////////////////////////////////////////////////////////////////////////////////
	ID3DBlob* pBlob;

	hresult = D3DCompileFromFile(
		TEXT("VertexShader.vsh"),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"vs_main",
		"vs_5_0",
		0,
		0,
		&pBlob,
		nullptr);

	if (FAILED(hresult))
		return hresult;

	hresult = m_Device->CreateVertexShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&m_VertexShader);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//入力レイアウトの生成
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hresult = m_Device->CreateInputLayout(
		vertexDesc,
		ARRAYSIZE(vertexDesc),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&m_InputLayout);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//PixelShaderの生成
	//////////////////////////////////////////////////////////////////////////////////
	hresult = D3DCompileFromFile(
		TEXT("PixelShader.psh"),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"ps_main",
		"ps_5_0",
		0,
		0,
		&pBlob,
		nullptr);

	if (FAILED(hresult))
		return hresult;

	hresult = m_Device->CreatePixelShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&m_PixelShader);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//ビューポートの生成
	//////////////////////////////////////////////////////////////////////////////////
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = (FLOAT)rect.Width();
	m_ViewPort.Height = (FLOAT)rect.Height();
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	return hresult;
}

void DrawManager::Render()
{
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT strides = sizeof(Vertex);
	UINT offsets = 0;

	m_Context->IASetInputLayout(m_InputLayout.Get());
	m_Context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &strides, &offsets);
	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_Context->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	m_Context->RSSetViewports(1, &m_ViewPort);
	m_Context->PSSetShader(m_PixelShader.Get(), nullptr, 0);
	m_Context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr);
	m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), color);
	m_Context->Draw(m_VertexManager.getVertexNum(), 0);
	m_Swapchain->Present(0, 0);
}