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
		nullptr,					//どのビデオアダプタを使用するかIDXGIAdapterのアドレスを渡す.既定ならばnullptr
		D3D_DRIVER_TYPE_HARDWARE,	//ドライバのタイプ
		nullptr,					//上記をD3D_DRIVER_TYPE_SOFTWAREに設定した際に、その処理を行うDLLのハンドル
		0,							//フラグ指定.D3D11_CREATE_DEVICE列挙型
		nullptr,					//作成を試みる機能レベルの順序を指定する配列
		0,							//作成を試みる機能レベルの順序を指定する配列の数
		D3D11_SDK_VERSION,			//SDKのバージョン
		&desc,						//スワップチェインの初期化パラメーター
		&m_Swapchain,				//作成されるスワップチェイン
		&m_Device,					//作成されるデバイス
		&level,						//作成されたデバイスの機能レベル
		&m_Context					//作成されるデバイスコンテキスト
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//バックバッファの取得
	//////////////////////////////////////////////////////////////////////////////////
	ID3D11Texture2D* backBuffer;
	hresult = m_Swapchain->GetBuffer(
		0,							//バッファのインデックス(基本は0)
		IID_PPV_ARGS(&backBuffer)	//バッファの取得先
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//レンダリングターゲットの生成
	//////////////////////////////////////////////////////////////////////////////////
	hresult = m_Device->CreateRenderTargetView(
		backBuffer,			//作成するバッファのリソース
		nullptr,			//作成するViewの設定内容データの指定(nullptrでデフォルト設定になる)
		&m_RenderTargetView	//作成されたRenderTargetView
	);

	backBuffer->Release();
	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//頂点バッファの生成
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = sizeof(Vertex) * m_VertexManager.GetVertexNum();	//バッファーのサイズ (バイト単位)
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;									//バッファーで想定されている読み込みおよび書き込みの方法
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;						//バッファーをどのようにパイプラインにバインドするか
	bufferDesc.CPUAccessFlags = 0;											//CPU アクセスのフラグ
	bufferDesc.MiscFlags = 0;												//その他のフラグ
	bufferDesc.StructureByteStride = 0;										//構造体が構造化バッファーを表す場合、その構造体のサイズ (バイト単位)

	D3D11_SUBRESOURCE_DATA subresource;
	ZeroMemory(&subresource, sizeof(D3D11_SUBRESOURCE_DATA));
	subresource.pSysMem = m_VertexManager.GetVertexList();	//初期化データへのポインタ
	subresource.SysMemPitch = 0;							//テクスチャーにある 1 本の線の先端から隣の線までの距離 (バイト単位) 
	subresource.SysMemSlicePitch = 0;						//1 つの深度レベルの先端から隣の深度レベルまでの距離 (バイト単位)

	hresult = m_Device->CreateBuffer(
		&bufferDesc,	//バッファーの記述へのポインタ
		&subresource,	//初期化データへのポインタ
		&m_VertexBuffer	//作成されるバッファーへのポインタ
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//VertexShaderの生成
	//////////////////////////////////////////////////////////////////////////////////
	ID3DBlob* pBlob;

	hresult = D3DCompileFromFile(
		TEXT("VertexShader.vsh"),			//ファイル名
		nullptr,							//D3D_SHADER_MACRO構造体の配列を指定.シェーダマクロを定義する際に設定する.
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//コンパイラがインクルードファイルを取り扱うために使用するID3DIncludeインタフェースへのポインタ
		"vs_main",							//エントリーポイントのメソッド名
		"vs_5_0",							//コンパイルターゲットを指定
		0,									//シェーダのコンパイルオプション
		0,									//エフェクトファイルのコンパイルオプション
		&pBlob,								//コンパイルされたコードへアクセスするためのID3DBlobインタフェースのポインタ
		nullptr								//コンパイルエラーメッセージへアクセスするためのID3DBlobインタフェースのポインタ
	);

	if (FAILED(hresult))
		return hresult;

	hresult = m_Device->CreateVertexShader(
		pBlob->GetBufferPointer(),	//コンパイル済みシェーダーへのポインタ
		pBlob->GetBufferSize(),		//コンパイル済み頂点シェーダーのサイズ
		nullptr,					//ID3D11ClassLinkage インターフェースへのポインタ
		&m_VertexShader				//ID3D11VertexShader インターフェイスへのポインタ
	);

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
		vertexDesc,					//入力アセンブラー ステージの入力データ型の配列
		ARRAYSIZE(vertexDesc),		//入力要素の配列内の入力データ型の数
		pBlob->GetBufferPointer(),	//コンパイル済みシェーダーへのポインタ
		pBlob->GetBufferSize(),		//コンパイル済みシェーダーのサイズ
		&m_InputLayout				//作成される入力レイアウト オブジェクトへのポインタ
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//PixelShaderの生成
	//////////////////////////////////////////////////////////////////////////////////
	hresult = D3DCompileFromFile(
		TEXT("PixelShader.psh"),			//ファイル名
		nullptr,							//D3D_SHADER_MACRO構造体の配列を指定.シェーダマクロを定義する際に設定する.
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//コンパイラがインクルードファイルを取り扱うために使用するID3DIncludeインタフェースへのポインタ
		"ps_main",							//エントリーポイントのメソッド名
		"ps_5_0",							//コンパイルターゲットを指定
		0,									//シェーダのコンパイルオプション
		0,									//エフェクトファイルのコンパイルオプション
		&pBlob,								//コンパイルされたコードへアクセスするためのID3DBlobインタフェースのポインタ
		nullptr								//コンパイルエラーメッセージへアクセスするためのID3DBlobインタフェースのポインタ
	);

	if (FAILED(hresult))
		return hresult;

	hresult = m_Device->CreatePixelShader(
		pBlob->GetBufferPointer(),	//コンパイル済みシェーダーへのポインタ
		pBlob->GetBufferSize(),		//コンパイル済み頂点シェーダーのサイズ
		nullptr,					//ID3D11ClassLinkage インターフェースへのポインタ
		&m_PixelShader				//ID3D11PixelShader インターフェイスへのポインタ
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//ビューポートの生成
	//////////////////////////////////////////////////////////////////////////////////
	m_ViewPort.TopLeftX = 0;					//ビューポートの左側の X 位置
	m_ViewPort.TopLeftY = 0;					//ビューポートの上部の Y 位置
	m_ViewPort.Width = (FLOAT)rect.Width();		//ビューポートの幅
	m_ViewPort.Height = (FLOAT)rect.Height();	//ビューポートの高さ
	m_ViewPort.MinDepth = 0.0f;					//ビューポートの最小深度
	m_ViewPort.MaxDepth = 1.0f;					//ビューポートの最大深度


	return hresult;
}

void DrawManager::Render()
{
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT strides = sizeof(Vertex);
	UINT offsets = 0;

	m_Context->IASetInputLayout(m_InputLayout.Get());										//インプットレイアウトの設定
	m_Context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &strides, &offsets);	//頂点バッファの設定
	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);				//頂点バッファがどの順番で三角形を作るか
	m_Context->VSSetShader(m_VertexShader.Get(), nullptr, 0);								//頂点シェーダーの設定
	m_Context->RSSetViewports(1, &m_ViewPort);												//ビューポートの設定
	m_Context->PSSetShader(m_PixelShader.Get(), nullptr, 0);								//ピクセルシェーダーの設定
	m_Context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr);			//レンダーターゲットの設定

	m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), color);						//レンダーターゲットをクリアする
	m_Context->Draw(m_VertexManager.GetVertexNum(), 0);										//描画する
	m_Swapchain->Present(0, 0);																//スワップ チェーンが所有するバック バッファのシーケンスの中の次のバッファの内容を表示
}

void DrawManager::AddVertex(const Vertex& vertex)
{
	m_VertexManager.AddVertex(vertex);
}