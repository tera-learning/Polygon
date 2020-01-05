#include "DrawManager.h"
#include "VertexManager.h"
#include <atltypes.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

HRESULT DrawManager::Create(HWND hwnd)
{

	//////////////////////////////////////////////////////////////////////////////////
	//�E�B���h�E�T�C�Y�擾
	//////////////////////////////////////////////////////////////////////////////////
	CRect rect;
	GetClientRect(hwnd, &rect);


	//////////////////////////////////////////////////////////////////////////////////
	//�f�o�C�X�ƃX���b�v�`�F�C���̐���
	//////////////////////////////////////////////////////////////////////////////////
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	desc.BufferCount = 1;									// �X���b�v�`�F�C���̃o�b�t�@��
	desc.BufferDesc.Width = rect.Width();					// �X���b�v�`�F�C���̃o�b�t�@�T�C�Y
	desc.BufferDesc.Height = rect.Height();					// �X���b�v�`�F�C���̃o�b�t�@�T�C�Y
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �X���b�v�`�F�C���̃o�b�t�@�t�H�[�}�b�g
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// �o�b�t�@�������_�[�^�[�Q�b�g�Ƃ��Ďg�p
	desc.OutputWindow = hwnd;								// HWND�n���h��
	desc.SampleDesc.Count = 1;								// �}���`�T���v�����O�̃s�N�Z���P�ʂ̐�
	desc.SampleDesc.Quality = 0;							// �}���`�T���v�����O�̕i��
	desc.Windowed = TRUE;									// �E�B���h�E���[�h

	D3D_FEATURE_LEVEL level;

	HRESULT hresult = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,					//�L���ɂ��郉���^�C�����C���[
		nullptr,			//�쐬�����݂�@�\���x���̏������w�肷��z��
		0,					//�쐬�����݂�@�\���x���̏������w�肷��z��̐�
		D3D11_SDK_VERSION,	//D3D11_SDK_VERSION���w��
		&desc,				//�X���b�v�`�F�C���̏������p�����[�^�[
		&m_Swapchain,		//�쐬�����X���b�v�`�F�C��
		&m_Device,			//�쐬�����f�o�C�X
		&level,				//�쐬���ꂽ�f�o�C�X�̋@�\���x��
		&m_Context			//�쐬�����f�o�C�X�R���e�L�X�g
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//�o�b�N�o�b�t�@�̎擾
	//////////////////////////////////////////////////////////////////////////////////
	ID3D11Texture2D* backBuffer;
	hresult = m_Swapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//�����_�����O�^�[�Q�b�g�̐���
	//////////////////////////////////////////////////////////////////////////////////
	hresult = m_Device->CreateRenderTargetView(backBuffer, nullptr, &m_RenderTargetView);
	backBuffer->Release();
	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//���_�o�b�t�@�̐���
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
	//VertexShader�̐���
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
	//���̓��C�A�E�g�̐���
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
	//PixelShader�̐���
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
	//�r���[�|�[�g�̐���
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