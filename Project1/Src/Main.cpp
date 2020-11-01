#include <Windows.h>
#include <DirectXMath.h>
#include <vector>
#include <time.h>
#include <mmsystem.h>
#include "Window.h"
#include "DirectGraphics.h"
#include "FbxMeshFile.h"
#define FBXSDK_SHARED
#pragma comment(lib, "winmm.lib")
#include "DXTK11Lib.h"
#include "input.h"
using namespace DXTK11LIB;

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmpLine,
	INT       nCmdShow)
{

	Window window("UNT", 700, 500);

	// ウィンドウを作成
	if (window.Create() == false)
	{
		//return 0;
	}

	//キー入力初期化
	InitInput(hInstance, FindWindow(Window::ClassName, nullptr));


	if (DirectGraphics::GetInstance()->Init() == false)
	{
		//return 0;
	}
	FbxMeshFile* model = new FbxMeshFile();
	model->con = DirectGraphics::GetInstance()->GetContext();
	model->Load("Res/Model/S.fbx",
		"../x64/Debug/VertexShader.cso",
		"../x64/Debug/PixelShader.cso");

	//3D用
	Vector3 pos = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 degree = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

	//// LIB2D用
	degree = Vector3(90.0f, 0.0f, 180.0f);

	InitDXTK11Lib();


	Image im(L"test2.png");

	float x = 0.1;
	float y = 0.3;

	while (true)
	{
		MSG msg;

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			UpdateInput();//入力処理更新

			//描画前処理
			DirectGraphics::GetInstance()->StartRendering();
			DirectGraphics::GetInstance()->SetUpContext(nullptr, nullptr);

			model->Render( //描画関数
				DirectGraphics::GetInstance(),
				pos,
				scale,
				degree);
			im.DrawEX(320, 240, 0.0, x,y);

			DirectGraphics::GetInstance()->FinishRendering();//描画する毎回の処理

		}
	}

	UninitInput();
	delete model;

	DirectGraphics::GetInstance()->Release();

	return 0;
}
