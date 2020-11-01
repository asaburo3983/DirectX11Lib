#include "DXTK11Lib.h"

namespace DXTK11LIB {

	float imageExtend = 1.0f;//全画像の倍率

	std::unique_ptr<DirectX::SpriteBatch> fontBatch;//フォント用の共通変数
	std::unique_ptr<DirectX::SpriteFont> fontSprite;

	float alphaDefault = 1.0f;//全体のデフォルトアルファ値

	ID3D11DeviceContext* context;
	ID3D11Device* device;

	//初期処理
	void InitDXTK11Lib() {
		context = DirectGraphics::GetInstance()->GetContext();
		device = DirectGraphics::GetInstance()->GetDevice();
		//フォント読み込み
		fontBatch = std::make_unique<DirectX::SpriteBatch>(context);
		fontSprite = std::make_unique<DirectX::SpriteFont>(device, L"Font/myfile.spritefont");

	}

	//２D描画系//////////////////////////////

	//文字描画
	void DrawString(const wchar_t* str, int x, int y, StrColor _color, float size, float angle) {

		DirectX::XMVECTORF32 color;
		for (int i = 0; i < 4; i++) {
			color.f[i] = _color.rgba[i];
		}
		
		//描画
		fontBatch->Begin();

		fontSprite->DrawString(fontBatch.get(), str, DirectX::XMFLOAT2(x, y), color, 0, DirectX::XMFLOAT2(0, 0), size);

		fontBatch->End();
	}
	void DrawString(const char* str, int x, int y, StrColor _color, float size, float angle) {
		size_t tmpsize = strlen(str);//char型をWCHARに変換
		int  tmpsizei = tmpsize + 1;
		WCHAR* tmpwbuf = new WCHAR[tmpsizei];
		mbstowcs_s(0, tmpwbuf, tmpsizei, str, _TRUNCATE);

		DrawString(tmpwbuf, x, y, _color, size, angle);

		delete tmpwbuf;
	}

	//FPSを描画
	int frame = 0;
	unsigned long base_time = 0.0;
	unsigned long prev = 0.0;
	unsigned long current = 0.0;
	char	buf[20];
	WCHAR wbuf[20];
	void DrawFPS() {
		// 時間取得
		base_time = timeGetTime();


		float intarval = 0.0;

		// フレームレート出力
		if ((frame + 1) % 60 == 0)
		{
			current = timeGetTime();

			intarval = 60000.0f / (current - prev);

			sprintf_s(buf, sizeof(buf), "FPS : %.1f", intarval);
			prev = current;
		}
		frame++;
		mbstowcs_s(0, wbuf, 20, buf, _TRUNCATE);
		DrawString(wbuf, 0, 0, StrColor{ 1,0,0,1 });

	}

	//全体のデフォルトα値をセット
	void SetDefaultAlpha(float _alpha) {
		alphaDefault = _alpha;
	}
	//全体の追加の拡大率をセット
	void SetDefaultImageExtend(float _extend) {
		imageExtend = _extend;
	}
	//////////////////////////////
	//2D画像//////////////////////////////
	Image::Image(const wchar_t* filePath) {//画像ロード コンストラクタ
		Load(filePath);
	}
	void Image::Load(const wchar_t* filePath) {//画像ロード　
		//画像描画追加分
		m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;

		//画像読み込み
		DirectX::CreateWICTextureFromFile(device, filePath, resource.GetAddressOf(), m_texture.ReleaseAndGetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		DX::ThrowIfFailed(resource.As(&texture));

		//画像データをとる
		CD3D11_TEXTURE2D_DESC textureDesc;
		texture->GetDesc(&textureDesc);

		textureCenter = DirectX::SimpleMath::Vector2(textureDesc.Width / 2, textureDesc.Height / 2);

		width = textureDesc.Width;//画像サイズの格納
		height = textureDesc.Height;

	}

	void Image::Load(const char* filePath) {//画像ロード　
		size_t tmpsize = strlen(filePath);//char型をWCHARに変換
		int  tmpsizei = tmpsize + 1;
		WCHAR* tmpwbuf = new WCHAR[tmpsizei];
		mbstowcs_s(0, tmpwbuf, tmpsizei, filePath, _TRUNCATE);

		Load(tmpwbuf);

		delete tmpwbuf;
	}

	void Image::Draw(int _x, int _y, float _angle, float _size, float _alpha, int _center) {
		x = _x;
		y = _y;
		size = _size * imageExtend;

		center = _center;

		//シェーダーを無効化にしている これがないと別のシェーダーが適応されてバグる
		// VSに頂点シェーダを設定
		context->VSSetShader(NULL, NULL, 0);
		// GSにジオメトリ・シェーダを設定
		context->GSSetShader(NULL, NULL, 0);
		// RSにラスタライザ・ステート・オブジェクトを設定
		context->RSSetState(NULL);
		// PSにピクセル・シェーダを設定
		context->PSSetShader(NULL, NULL, 0);

		//未入力の際デフォルトのα値を入れる
		if (_alpha == -1000.0123f)
			_alpha = alphaDefault;
		//画像描画
		m_spriteBatch->Begin();
		
		m_spriteBatch->Draw(m_texture.Get(), DirectX::SimpleMath::Vector2(x, y), NULL, DirectX::Colors::White * _alpha, _angle / 180.0 * 3.1415, textureCenter*_center, size);
		m_spriteBatch->End();
	}
	void Image::DrawEX(int _x, int _y, float _angle, float _sizeX,float _sizeY, float _alpha, int _center) {
		x = _x;
		y = _y;

		center = _center;

		//シェーダーを無効化にしている これがないと別のシェーダーが適応されてバグる
		// VSに頂点シェーダを設定
		context->VSSetShader(NULL, NULL, 0);
		// GSにジオメトリ・シェーダを設定
		context->GSSetShader(NULL, NULL, 0);
		// RSにラスタライザ・ステート・オブジェクトを設定
		context->RSSetState(NULL);
		// PSにピクセル・シェーダを設定
		context->PSSetShader(NULL, NULL, 0);

		//未入力の際デフォルトのα値を入れる
		if (_alpha == -1000.0123f)
			_alpha = alphaDefault;

		m_spriteBatch->Begin();
		m_spriteBatch->Draw(m_texture.Get(), DirectX::SimpleMath::Vector2(x, y), NULL, DirectX::Colors::White * _alpha, _angle / 180.0 * 3.1415, textureCenter*_center, DirectX::XMFLOAT2(_sizeX*imageExtend, _sizeY*imageExtend));
		m_spriteBatch->End();

	}
	bool Image::Hit(int _x, int _y) {//画像内に入った当たり判定 回転してない四角い画像だけ当たり判定をとれる
		int minX, maxX, minY, maxY;
		if (center == 1) {
			maxX = x + (width / 2 * size);
			minX = x - (width / 2 * size);
			maxY = y + (height / 2 * size);
			minY = y - (height / 2 * size);
		}
		else {
			maxX = x + (width * size);
			minX = x;
			maxY = y + (height * size);
			minY = y;
		}

		//当たり判定内
		if (minX <= _x && _x <= maxX && minY <= _y && _y <= maxY) {
			return true;
		}
		return false;
	}
	//////////////////////////////
	//音楽//////////////////////////////

	Sound_MP3::Sound_MP3(const wchar_t* filePath) {
		Load(filePath);
	}
	void Sound_MP3::Load(const wchar_t* filePath) {

		MFPCreateMediaPlayer(filePath, FALSE, 0, NULL, NULL, &g_pPlayer);
		
	}
	void Sound_MP3::Load(const char* filePath) {
		size_t tmpsize = strlen(filePath);//char型をWCHARに変換
		int  tmpsizei = tmpsize+1;
		WCHAR* tmpwbuf = new WCHAR[tmpsizei];
		mbstowcs_s(0, tmpwbuf, tmpsizei, filePath, _TRUNCATE);

		MFPCreateMediaPlayer(tmpwbuf, FALSE, 0, NULL, NULL, &g_pPlayer);
		delete tmpwbuf;
	}
	void Sound_MP3::Play() {
		g_pPlayer->GetState(&state);
		if (state == MFP_MEDIAPLAYER_STATE_STOPPED || state ==MFP_MEDIAPLAYER_STATE_EMPTY)
		{
			g_pPlayer->Play();

		}
	}
	void Sound_MP3::Stop() {
		g_pPlayer->GetState(&state);
		if (state != MFP_MEDIAPLAYER_STATE_STOPPED) {
			g_pPlayer->Stop();
		}
	}
	void Sound_MP3::Volume(float vol) {
		g_pPlayer->SetVolume(vol);
	}
	//////////////////////////////
}