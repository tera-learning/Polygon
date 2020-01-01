#include "DrawManager.h"
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

	HWND hwnd = CreateWindow(
		winc.lpszClassName,
		TEXT("Polygon"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100,
		100,
		800,
		600,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (hwnd == NULL) return 0;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	
	DrawManager dx11Manager;
	dx11Manager.Create(hwnd);

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