#pragma once
#include "DirectGraphics.h"
#include <winsdkver.h>
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl/client.h>

#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>

//画像描画追加分
#ifdef _DEBUG
	#pragma comment(lib, "DirectXTK_d.lib")
#else
	#pragma comment(lib, "DirectXTK.lib")
#endif

#pragma comment(lib,"mfplay.lib")   //多分これが音楽LIB
#include <mfplay.h>　//多分これが音楽H

#include <SpriteBatch.h>
#include <SimpleMath.h>
#include <SpriteFont.h>



namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Set a breakpoint on this line to catch DirectX API errors
            throw std::exception();
        }
    }
}

namespace DXTK11LIB {

	struct StrColor {
		float rgba[4];
	};
	void InitDXTK11Lib();

	//文字の描画
	void DrawString(const wchar_t* str = L"未入力です", int x = 320, int y = 240, StrColor _color = StrColor{1,1,1,1}, float size = 1.0f, float angle = 0.0f);
	void DrawString(const char* str = "未入力です", int x = 320, int y = 240, StrColor _color = StrColor{ 1,1,1,1 }, float size = 1.0f, float angle = 0.0f);

	//FPSの描画
	void DrawFPS();

	//全体のデフォルトα値をセット
	void SetDefaultAlpha(float _alpha = 1);

	//全体の追加の拡大率をセット
	void SetDefaultImageExtend(float _extend = 1);

	class Image {
	private:
		int x = 0;
		int y = 0;
		float size = 1.0f;		
		int width = 0;
		int height = 0;

		int center = 1;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
		std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
		DirectX::SimpleMath::Vector2 textureCenter;
	public:

		Image() {};//コンストラクタ
		Image(const wchar_t* filePath);//コンストラクタ&画像ロード

		void Load(const wchar_t* filePath);//こっちのほうが処理がちょっと早い
		void Load(const char* filePath);

		void Draw(int _x = 0, int _y = 0, float _angle = 0.0f, float _size = 1.0f, float _alpha = -1000.0123f, int _center = 1);//描画のすべて、未入力でもデフォルト値が入る。注意！ 手動でalphaのデフォルト値と同じ値を入れると完全透過ではなくデフォルトの値が入る
		void DrawEX(int _x = 0, int _y = 0, float _angle = 0.0f, float _sizeX = 1.0f, float _sizeY = 1.0f, float _alpha = -1000.0123f, int _center = 1);
		bool Hit(int _x,int _y);//最後に描画した画像の当たり判定

		//アクセス用
		int GetX() { return x; };
		int GetY() { return y; };
		int GetWidth() { return width; };
		int GetHeight() { return height; };
		float GetSize() { return size; };
		int GetCenter() { return center; };
	};

	class Sound_MP3 {
	private:
		Microsoft::WRL::ComPtr<IMFPMediaPlayer> g_pPlayer = nullptr;//音楽用の変数
		MFP_MEDIAPLAYER_STATE state = MFP_MEDIAPLAYER_STATE_EMPTY;

	public:
		Sound_MP3() {};
		Sound_MP3(const wchar_t* filePath);
		void Load(const wchar_t* filePath); //こっちのほうが処理がちょっと早い
		void Load(const char* filePath);
		void Play();//ループする場合はゲームループに置くとできる
		void Stop();
		void Volume(float vol = 1.0f);

	};
}