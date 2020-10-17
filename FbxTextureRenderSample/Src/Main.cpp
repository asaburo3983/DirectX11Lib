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
/*
#pragma comment( lib, "libfbxsdk-mt.lib" )//FBXSDK�p�@�ǉ��̈ˑ��t�@�C���ɓ����@MD(���I)orMT(�ÓI
#pragma comment( lib, "libxml2-mt.lib" )
#pragma comment( lib, "zlib-mt.lib" )
*/
#include "DXTK11Lib.h"
#include "input.h"
using namespace DXTK11LIB;




int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmpLine,
	INT       nCmdShow)
{
	Window window("UNT", 700, 500);
	// �E�B���h�E���쐬
	if (window.Create() == false)
	{
		return 0;
	}

	//�L�[���͏�����
	InitInput(hInstance, FindWindow(Window::ClassName, nullptr));


	if (DirectGraphics::GetInstance()->Init() == false)
	{
		return 0;
	}
	FbxMeshFile* model = new FbxMeshFile();
	model->con = DirectGraphics::GetInstance()->GetContext();
	model->Load("Res/Model/untitled.fbx");

	//3D�p
	Vector3 pos = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 degree = Vector3(0.0f, 0.0f,0.0f);
	Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

	//// LIB2D�p
   pos = Vector3(0.0f, 0.0f, 0.0f);
   degree = Vector3(270.0f, 0.0f, 180.0f);
   //scale = Vector3(10.0f, 10.0f, 10.0f);
   //scale = Vector3(150.0f, 150.0f, 150.0f);

	timeBeginPeriod(1);

	DWORD current = timeGetTime();
	DWORD before = current;
	InitDXTK11Lib();
	Image im(L"100x100.png");

	//Sound_MP3 smp(L"test.mp3");
	//smp.Play();

	//X_INPUT x1(0);

	bool Mod = true;
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
			UpdateInput();//���͏����X�V

			//x1.updata();

				if (1) {
					model->Animation(0,2.1);
					//model2->Animation(1, 2.0);
				}



			DirectGraphics::GetInstance()->StartRendering();
			DirectGraphics::GetInstance()->SetUpContext();


			model->Render( //�`��֐�
				DirectGraphics::GetInstance(),
				pos,
				scale,
				degree);

				if (KeyInput::GetInstance()->key[DIK_LSHIFT] >= 1) {

					if (KeyInput::GetInstance()->key[DIK_UP] >= 1) {
						degree.Y++;
					}
					if (KeyInput::GetInstance()->key[DIK_DOWN] >= 1) {
						degree.Y--;
					}
					if (KeyInput::GetInstance()->key[DIK_RIGHT] >= 1) {
						degree.X++;
					}
					if (KeyInput::GetInstance()->key[DIK_LEFT] >= 1) {
						degree.X--;
					}
					if (KeyInput::GetInstance()->key[DIK_Z] >= 1) {
						degree.Z--;
					}
				}
				else {
					if (KeyInput::GetInstance()->key[DIK_UP] >= 1) {
						pos.Y+=0.01;
					}
					if (KeyInput::GetInstance()->key[DIK_DOWN] >= 1) {
						pos.Y-=0.01;
					}
					if (KeyInput::GetInstance()->key[DIK_RIGHT] >= 1) {
						pos.X+=0.01;
					}
					if (KeyInput::GetInstance()->key[DIK_LEFT] >= 1) {
						pos.X-=0.01;
					}
					if (KeyInput::GetInstance()->key[DIK_Z] >= 1) {
						pos.Z-=0.01;
					}
					if (KeyInput::GetInstance()->key[DIK_A] >= 1) {
						pos.Z+=0.01;
					}
				}

				DrawFPS();
				char buf[20];
				WCHAR wbuf[20];
				sprintf_s(buf, sizeof(buf), "POS : %.0f", pos.X);
				mbstowcs_s(0, wbuf, 20, buf, _TRUNCATE);
				DrawString(wbuf, 0, 50, StrColor{ 1,0,0,1 });

				sprintf_s(buf, sizeof(buf), "POS : %.0f", pos.Y);
				mbstowcs_s(0, wbuf, 20, buf, _TRUNCATE);
				DrawString(wbuf, 0, 70, StrColor{ 1,0,0,1 });

				sprintf_s(buf, sizeof(buf), "POS : %.0f", pos.Z);
				mbstowcs_s(0, wbuf, 20, buf, _TRUNCATE);
				DrawString(wbuf, 0, 90, StrColor{ 1,0,0,1 });

				sprintf_s(buf, sizeof(buf), "deg : %.0f", degree.X);
				mbstowcs_s(0, wbuf, 20, buf, _TRUNCATE);
				DrawString(wbuf, 0, 120, StrColor{ 1,0,0,1 });

				sprintf_s(buf, sizeof(buf), "deg : %.0f", degree.Y);
				mbstowcs_s(0, wbuf, 20, buf, _TRUNCATE);
				DrawString(wbuf, 0, 140, StrColor{ 1,0,0,1 });

				sprintf_s(buf, sizeof(buf), "deg : %.0f", degree.Z);
				mbstowcs_s(0, wbuf, 20, buf, _TRUNCATE);
				DrawString(wbuf, 0, 160, StrColor{ 1,0,0,1 });
			//degree.Z++;

			for (int i = 0; i < 7; i++) {
				for (int h = 0; h < 5; h++) {
				//	;// 3D�ƃV�F�[�_�[����������̂łRD�`��̍ۂ͍ēx�V�F�[�_�[������
					//im.Draw(50 + i * 100, 50 + h * 100);
				}
			}
			
			DirectGraphics::GetInstance()->FinishRendering();//�`�悷�閈��̏���

			//while (current - before < 17)
			//{
			//	Sleep(current - before);

			//	current = timeGetTime();
			//}
			//before = current;
		}
	}

	timeEndPeriod(1);
	UninitInput();
	delete model;
	
	DirectGraphics::GetInstance()->Release();

	return 0;
}
