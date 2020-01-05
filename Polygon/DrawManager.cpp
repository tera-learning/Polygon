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
		nullptr,					//�ǂ̃r�f�I�A�_�v�^���g�p���邩IDXGIAdapter�̃A�h���X��n��.����Ȃ��nullptr
		D3D_DRIVER_TYPE_HARDWARE,	//�h���C�o�̃^�C�v
		nullptr,					//��L��D3D_DRIVER_TYPE_SOFTWARE�ɐݒ肵���ۂɁA���̏������s��DLL�̃n���h��
		0,							//�t���O�w��.D3D11_CREATE_DEVICE�񋓌^
		nullptr,					//�쐬�����݂�@�\���x���̏������w�肷��z��
		0,							//�쐬�����݂�@�\���x���̏������w�肷��z��̐�
		D3D11_SDK_VERSION,			//SDK�̃o�[�W����
		&desc,						//�X���b�v�`�F�C���̏������p�����[�^�[
		&m_Swapchain,				//�쐬�����X���b�v�`�F�C��
		&m_Device,					//�쐬�����f�o�C�X
		&level,						//�쐬���ꂽ�f�o�C�X�̋@�\���x��
		&m_Context					//�쐬�����f�o�C�X�R���e�L�X�g
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//�o�b�N�o�b�t�@�̎擾
	//////////////////////////////////////////////////////////////////////////////////
	ID3D11Texture2D* backBuffer;
	hresult = m_Swapchain->GetBuffer(
		0,							//�o�b�t�@�̃C���f�b�N�X(��{��0)
		IID_PPV_ARGS(&backBuffer)	//�o�b�t�@�̎擾��
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//�����_�����O�^�[�Q�b�g�̐���
	//////////////////////////////////////////////////////////////////////////////////
	hresult = m_Device->CreateRenderTargetView(
		backBuffer,			//�쐬����o�b�t�@�̃��\�[�X
		nullptr,			//�쐬����View�̐ݒ���e�f�[�^�̎w��(nullptr�Ńf�t�H���g�ݒ�ɂȂ�)
		&m_RenderTargetView	//�쐬���ꂽRenderTargetView
	);

	backBuffer->Release();
	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//���_�o�b�t�@�̐���
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = sizeof(Vertex) * m_VertexManager.GetVertexNum();	//�o�b�t�@�[�̃T�C�Y (�o�C�g�P��)
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;									//�o�b�t�@�[�őz�肳��Ă���ǂݍ��݂���я������݂̕��@
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;						//�o�b�t�@�[���ǂ̂悤�Ƀp�C�v���C���Ƀo�C���h���邩
	bufferDesc.CPUAccessFlags = 0;											//CPU �A�N�Z�X�̃t���O
	bufferDesc.MiscFlags = 0;												//���̑��̃t���O
	bufferDesc.StructureByteStride = 0;										//�\���̂��\�����o�b�t�@�[��\���ꍇ�A���̍\���̂̃T�C�Y (�o�C�g�P��)

	D3D11_SUBRESOURCE_DATA subresource;
	ZeroMemory(&subresource, sizeof(D3D11_SUBRESOURCE_DATA));
	subresource.pSysMem = m_VertexManager.GetVertexList();	//�������f�[�^�ւ̃|�C���^
	subresource.SysMemPitch = 0;							//�e�N�X�`���[�ɂ��� 1 �{�̐��̐�[����ׂ̐��܂ł̋��� (�o�C�g�P��) 
	subresource.SysMemSlicePitch = 0;						//1 �̐[�x���x���̐�[����ׂ̐[�x���x���܂ł̋��� (�o�C�g�P��)

	hresult = m_Device->CreateBuffer(
		&bufferDesc,	//�o�b�t�@�[�̋L�q�ւ̃|�C���^
		&subresource,	//�������f�[�^�ւ̃|�C���^
		&m_VertexBuffer	//�쐬�����o�b�t�@�[�ւ̃|�C���^
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//VertexShader�̐���
	//////////////////////////////////////////////////////////////////////////////////
	ID3DBlob* pBlob;

	hresult = D3DCompileFromFile(
		TEXT("VertexShader.vsh"),			//�t�@�C����
		nullptr,							//D3D_SHADER_MACRO�\���̂̔z����w��.�V�F�[�_�}�N�����`����ۂɐݒ肷��.
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//�R���p�C�����C���N���[�h�t�@�C������舵�����߂Ɏg�p����ID3DInclude�C���^�t�F�[�X�ւ̃|�C���^
		"vs_main",							//�G���g���[�|�C���g�̃��\�b�h��
		"vs_5_0",							//�R���p�C���^�[�Q�b�g���w��
		0,									//�V�F�[�_�̃R���p�C���I�v�V����
		0,									//�G�t�F�N�g�t�@�C���̃R���p�C���I�v�V����
		&pBlob,								//�R���p�C�����ꂽ�R�[�h�փA�N�Z�X���邽�߂�ID3DBlob�C���^�t�F�[�X�̃|�C���^
		nullptr								//�R���p�C���G���[���b�Z�[�W�փA�N�Z�X���邽�߂�ID3DBlob�C���^�t�F�[�X�̃|�C���^
	);

	if (FAILED(hresult))
		return hresult;

	hresult = m_Device->CreateVertexShader(
		pBlob->GetBufferPointer(),	//�R���p�C���ς݃V�F�[�_�[�ւ̃|�C���^
		pBlob->GetBufferSize(),		//�R���p�C���ςݒ��_�V�F�[�_�[�̃T�C�Y
		nullptr,					//ID3D11ClassLinkage �C���^�[�t�F�[�X�ւ̃|�C���^
		&m_VertexShader				//ID3D11VertexShader �C���^�[�t�F�C�X�ւ̃|�C���^
	);

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
		vertexDesc,					//���̓A�Z���u���[ �X�e�[�W�̓��̓f�[�^�^�̔z��
		ARRAYSIZE(vertexDesc),		//���͗v�f�̔z����̓��̓f�[�^�^�̐�
		pBlob->GetBufferPointer(),	//�R���p�C���ς݃V�F�[�_�[�ւ̃|�C���^
		pBlob->GetBufferSize(),		//�R���p�C���ς݃V�F�[�_�[�̃T�C�Y
		&m_InputLayout				//�쐬�������̓��C�A�E�g �I�u�W�F�N�g�ւ̃|�C���^
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//PixelShader�̐���
	//////////////////////////////////////////////////////////////////////////////////
	hresult = D3DCompileFromFile(
		TEXT("PixelShader.psh"),			//�t�@�C����
		nullptr,							//D3D_SHADER_MACRO�\���̂̔z����w��.�V�F�[�_�}�N�����`����ۂɐݒ肷��.
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//�R���p�C�����C���N���[�h�t�@�C������舵�����߂Ɏg�p����ID3DInclude�C���^�t�F�[�X�ւ̃|�C���^
		"ps_main",							//�G���g���[�|�C���g�̃��\�b�h��
		"ps_5_0",							//�R���p�C���^�[�Q�b�g���w��
		0,									//�V�F�[�_�̃R���p�C���I�v�V����
		0,									//�G�t�F�N�g�t�@�C���̃R���p�C���I�v�V����
		&pBlob,								//�R���p�C�����ꂽ�R�[�h�փA�N�Z�X���邽�߂�ID3DBlob�C���^�t�F�[�X�̃|�C���^
		nullptr								//�R���p�C���G���[���b�Z�[�W�փA�N�Z�X���邽�߂�ID3DBlob�C���^�t�F�[�X�̃|�C���^
	);

	if (FAILED(hresult))
		return hresult;

	hresult = m_Device->CreatePixelShader(
		pBlob->GetBufferPointer(),	//�R���p�C���ς݃V�F�[�_�[�ւ̃|�C���^
		pBlob->GetBufferSize(),		//�R���p�C���ςݒ��_�V�F�[�_�[�̃T�C�Y
		nullptr,					//ID3D11ClassLinkage �C���^�[�t�F�[�X�ւ̃|�C���^
		&m_PixelShader				//ID3D11PixelShader �C���^�[�t�F�C�X�ւ̃|�C���^
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//�r���[�|�[�g�̐���
	//////////////////////////////////////////////////////////////////////////////////
	m_ViewPort.TopLeftX = 0;					//�r���[�|�[�g�̍����� X �ʒu
	m_ViewPort.TopLeftY = 0;					//�r���[�|�[�g�̏㕔�� Y �ʒu
	m_ViewPort.Width = (FLOAT)rect.Width();		//�r���[�|�[�g�̕�
	m_ViewPort.Height = (FLOAT)rect.Height();	//�r���[�|�[�g�̍���
	m_ViewPort.MinDepth = 0.0f;					//�r���[�|�[�g�̍ŏ��[�x
	m_ViewPort.MaxDepth = 1.0f;					//�r���[�|�[�g�̍ő�[�x


	return hresult;
}

void DrawManager::Render()
{
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT strides = sizeof(Vertex);
	UINT offsets = 0;

	m_Context->IASetInputLayout(m_InputLayout.Get());										//�C���v�b�g���C�A�E�g�̐ݒ�
	m_Context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &strides, &offsets);	//���_�o�b�t�@�̐ݒ�
	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);				//���_�o�b�t�@���ǂ̏��ԂŎO�p�`����邩
	m_Context->VSSetShader(m_VertexShader.Get(), nullptr, 0);								//���_�V�F�[�_�[�̐ݒ�
	m_Context->RSSetViewports(1, &m_ViewPort);												//�r���[�|�[�g�̐ݒ�
	m_Context->PSSetShader(m_PixelShader.Get(), nullptr, 0);								//�s�N�Z���V�F�[�_�[�̐ݒ�
	m_Context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr);			//�����_�[�^�[�Q�b�g�̐ݒ�

	m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), color);						//�����_�[�^�[�Q�b�g���N���A����
	m_Context->Draw(m_VertexManager.GetVertexNum(), 0);										//�`�悷��
	m_Swapchain->Present(0, 0);																//�X���b�v �`�F�[�������L����o�b�N �o�b�t�@�̃V�[�P���X�̒��̎��̃o�b�t�@�̓��e��\��
}

void DrawManager::AddVertex(const Vertex& vertex)
{
	m_VertexManager.AddVertex(vertex);
}