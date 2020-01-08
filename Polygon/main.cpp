#include "DrawManager.h"
#include <atltypes.h>
#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	WNDCLASSEX winc;

	//構造体のサイズ
	winc.cbSize = sizeof(WNDCLASSEX);
	//ウィンドウクラスの基本スタイルを表す定数
	winc.style = CS_HREDRAW | CS_VREDRAW;
	//ウィンドウプロシージャ（ウィンドウで発生したイベントを処理する関数）のポイント型
	winc.lpfnWndProc = WndProc;
	//クラスの構造体の追加領域
	winc.cbClsExtra = 0;
	//ウィンドウ構造体の追加領域
	winc.cbWndExtra = 0;
	//インスタンスハンドル
	winc.hInstance = hInstance;
	//デスクトップ等に描画されるアイコンの情報を持つアイコンハンドル
	winc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	//ウィンドウのクライアントエリア上のマウスカーソル
	winc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//クライアントエリアの背景色
	winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	//クラスメニュー
	winc.lpszMenuName = nullptr;
	//ウィンドウクラスの名前
	winc.lpszClassName = TEXT("Polygon");
	//タスクバーやタイトルバーに表示される小さいアイコンの情報を持つアイコンハンドル
	winc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&winc)) return 0;

	//////////////////////////////////////////////////////////////////////////////////
	//ウィンドウ生成
	//////////////////////////////////////////////////////////////////////////////////
	int width = 600;
	int height = 600;

	HWND hwnd = CreateWindow(
		winc.lpszClassName,
		TEXT("Polygon"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0,
		0,
		width,
		height,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (hwnd == NULL) return 0;


	//////////////////////////////////////////////////////////////////////////////////
	//クライアント領域基準で再生成
	//////////////////////////////////////////////////////////////////////////////////
	CRect crect, wrect;
	GetWindowRect(hwnd, &wrect);
	GetClientRect(hwnd, &crect);

	int newWidth = width + ((wrect.right - wrect.left) - (crect.right - crect.left));
	int newHeight = height + ((wrect.bottom - wrect.top) - (crect.bottom - crect.top));

	SetWindowPos(hwnd, nullptr, 0, 0, newWidth, newHeight, SWP_SHOWWINDOW);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	

	//////////////////////////////////////////////////////////////////////////////////
	//描画
	//////////////////////////////////////////////////////////////////////////////////
	DrawManager dx11Manager;

	//クライアント領域の再取得
	GetClientRect(hwnd, &crect);
	//クライアント領域の中心座標
	CPoint center = crect.CenterPoint();
	//描画したい四角形のピクセル座標
	CRect pixel(200, 100, 400, 300);
	//クライアント領域中心～描画したい四角形の左上座標までの距離
	FLOAT left = static_cast<FLOAT>(pixel.left - center.x) / center.x;
	FLOAT top = static_cast<FLOAT>(center.y - pixel.top) / center.y;
	//クライアント領域中心～描画したい四角形の右下座標までの距離
	FLOAT right = static_cast<FLOAT>(pixel.right - center.x) / center.x;
	FLOAT bottom = static_cast<FLOAT>(center.y - pixel.bottom) / center.y;

	dx11Manager.AddVertex({ {left, top, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f} });
	dx11Manager.AddVertex({ {right, top, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f} });
	dx11Manager.AddVertex({ {left, bottom, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f} });
	dx11Manager.AddVertex({ {right, bottom, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f} });

	dx11Manager.Create(hwnd);


	//////////////////////////////////////////////////////////////////////////////////
	//描画
	//////////////////////////////////////////////////////////////////////////////////
	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			dx11Manager.Render();
		}
	}

	return msg.wParam;
}