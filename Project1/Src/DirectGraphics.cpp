#include <stdio.h>
#include <atlstr.h>
#include <tchar.h>
#include <DirectXMath.h>
#include "DirectGraphics.h"
#include "ShaderBase.h"
#include "Window.h"
#include "GraphicsUtility.h"

#pragma comment(lib,"d3d11.lib")

bool DirectGraphics::Init()
{
	// DeviceとSwapChainの作成
	if (CreateDeviceAndSwapChain() == false)
	{
		return false;
	}

	// RenderTargetViewの作成
	if (CreateRenderTargetView() == false)
	{
		return false;
	}

	// Depth、StencilViewの作成
	if (CreateDepthAndStencilView() == false)
	{
		return false;
	}
	//ブレンドステートの作成
	CreateBlendState();

	if (CreateTextureSampler() == false)
	{
		return false;
	}

	if (CreateConstantBuffer() == false)
	{
		return false;
	}

	// ViewPort設定
	SetUpViewPort();

	// 変換行列設定
	SetUpTransform();

	//これがないと２Dと３Dの同時描画の際に３DのZバッファが狂う
	// **********************************************************
	// 深度/ステンシル・ステート・オブジェクトの作成 
	D3D11_DEPTH_STENCIL_DESC DepthStencil;
	DepthStencil.DepthEnable = TRUE; // 深度テストあり
	DepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // 書き込む
	DepthStencil.DepthFunc = D3D11_COMPARISON_LESS; // 手前の物体を描画
	DepthStencil.StencilEnable = FALSE; // ステンシル・テストなし
	DepthStencil.StencilReadMask = 0;     // ステンシル読み込みマスク。
	DepthStencil.StencilWriteMask = 0;     // ステンシル書き込みマスク。
			// 面が表を向いている場合のステンシル・テストの設定
	DepthStencil.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER; // 常に失敗
			// 面が裏を向いている場合のステンシル・テストの設定
	DepthStencil.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // 常に成功
	m_Device->CreateDepthStencilState(&DepthStencil,&g_pDepthStencilState);


	

	//// ラスタライザ・ステート・オブジェクトの作成
	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;   // 普通に描画する
	RSDesc.CullMode = D3D11_CULL_NONE;    // 両面を描画する
	RSDesc.FrontCounterClockwise = FALSE; // 時計回りが表面
	RSDesc.DepthBias = 0;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.DepthClipEnable = TRUE;
	RSDesc.ScissorEnable = FALSE;
	RSDesc.MultisampleEnable = FALSE;
	RSDesc.AntialiasedLineEnable = FALSE;
	m_Device->CreateRasterizerState(&RSDesc, &g_pRasterizerState);

	return true;
}

void DirectGraphics::Release()
{
	if (g_pRasterizerState != nullptr) {
		g_pRasterizerState->Release();
		g_pRasterizerState = nullptr;
	}
	
	if (g_pDepthStencilState != nullptr) {
		//delete g_pDepthStencilState;
		g_pDepthStencilState->Release();
		g_pDepthStencilState = nullptr;
	}
	if (g_pBlendState != nullptr) {
		delete g_pBlendState;
		g_pBlendState = nullptr;
	}

	if (m_Context != nullptr)
	{
		m_Context->ClearState();
		m_Context->Flush();
		m_Context->Release();
		m_Context = nullptr;
	}

	if (m_DepthStencilView != nullptr)
	{
		m_DepthStencilView->Release();
		m_DepthStencilView = nullptr;
	}

	if (m_RenderTargetView != nullptr)
	{
		m_RenderTargetView->Release();
		m_RenderTargetView = nullptr;
	}

	if (m_SwapChain != nullptr)
	{
		m_SwapChain->Release();
		m_SwapChain = nullptr;
	}

	if (m_Device != nullptr)
	{
		m_Device->Release();
		m_Device = nullptr;
	}
}

void DirectGraphics::StartRendering()
{
	float clear_color[ 4 ] = { 0.0f, 1.0f, 0.0f, 1.0f };	// RenderTarget塗りつぶしカラー(RGBA)

	// DirecX9ではRenderTargetとDepth、Stencilバッファのクリアは別々にする
    m_Context->ClearRenderTargetView( 
				m_RenderTargetView,							// クリア対象のView
				clear_color );								// クリアカラー

    m_Context->ClearDepthStencilView( 
				m_DepthStencilView,							// クリア対象のView	
				D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,	// クリアフラグ(今回はDepth、Stencilともにクリア)
				1.0f,										// 深度クリア値
				0 );										// ステンシルクリア値
}

void DirectGraphics::FinishRendering()
{
	// 描画コマンドを送信する
	m_SwapChain->Present(
			/*
				垂直同期のタイミング
					0なら即時描画
					1以上なら指定した値の垂直同期後に描画
			*/
			1,
			/*
				出力オプション
					フレーム出力を行うためのオプションで
					基本は0で問題ないと思われる
					その他のフラグはDXGI_PRESENTで確認可能
			*/
			0 );	// 出力オプション
}


void DirectGraphics::SetMaterial(ObjMaterial* material)
{
	m_ConstantBufferData.MaterialAmbient = DirectX::XMFLOAT4(material->Ambient[0], material->Ambient[1], material->Ambient[2], 1);
	m_ConstantBufferData.MaterialDiffuse = DirectX::XMFLOAT4(material->Diffuse[0], material->Diffuse[1], material->Diffuse[2], 1);
	m_ConstantBufferData.MaterialSpecular = DirectX::XMFLOAT4(material->Specular[0], material->Specular[1], material->Specular[2], 1);
}


void DirectGraphics::SetTexture(ID3D11ShaderResourceView* texture)
{
	// Samplerの設定
	m_Context->PSSetSamplers(
		0,					// スロット番号
		1,					// サンプラーの数
		&m_SamplerState);	// ID3D11SamplerState

	// PixelShaderで使用するテクスチャの設定
	m_Context->PSSetShaderResources(
		0,								// スロット番号
		1,								// リソースの数
		&texture);						// ID3D11ShaderResourceView

}

//射影変換投影、並行投影など
void DirectGraphics::SetUpTransform()
{
	HWND window_handle = FindWindow(Window::ClassName, nullptr);
	RECT rect;
	GetClientRect(window_handle, &rect);

	// Viewマトリクス設定
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
	DirectX::XMVECTOR focus = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX view_matrix = DirectX::XMMatrixLookAtLH(eye, focus, up);

	// プロジェクションマトリクス設定
	float    fov = DirectX::XMConvertToRadians(45.0f);
	float    aspect = (float)(rect.right - rect.left) / (rect.bottom  - rect.top);
	float    nearZ = 0.1f;
	float    farZ = 1000.0f;
	DirectX::XMMATRIX proj_matrix = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);

	if(1)//並行投影
		proj_matrix = DirectX::XMMatrixOrthographicLH(rect.right - rect.left, rect.bottom - rect.top, nearZ, farZ);


	// ライトの設定
	DirectX::XMVECTOR light = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f));

	// コンスタントバッファの設定
	/*
		HLSLの行列は行と列の考え方が違うので合わせる必要がある
		XMMatrixTransposeを使用して転置行列にすることでHLSLの形式に合わせられる

			転置行列 => 列と行を入れ替える
							(a, b)	=> (b, a)
							(c, d)	   (d, c)
	*/
	XMStoreFloat4x4(&m_ConstantBufferData.View, XMMatrixTranspose(view_matrix));
	XMStoreFloat4x4(&m_ConstantBufferData.Projection, XMMatrixTranspose(proj_matrix));
	XMStoreFloat4(&m_ConstantBufferData.LightVector, light);
	XMStoreFloat4(&m_ConstantBufferData.CameraPos, eye);

	// ライトのカラー設定
	m_ConstantBufferData.LightColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1);
}

void DirectGraphics::SetUpContext(VertexShader* m_VertexShader ,PixelShader* m_PixelShader)
{
	// プリミティブの形状を指定
	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (m_VertexShader != nullptr) {
		// VerteXShader、PixelShaderを設定
		m_Context->VSSetShader(
			m_VertexShader->GetShaderInterface(),	// 使用するVertexShder
			/*
				ClassInstanceのポインタを設定する
					これはShader作成時のID3D11ClassLinkageを使用した場合に
					用いる項目なので今回はnullptrを指定する
			*/
			nullptr,
			0);									// ClassInstanceの数
		m_Context->PSSetShader(m_PixelShader->GetShaderInterface(), nullptr, 0);
	}
	// OMに深度/ステンシル・ステート・オブジェクトを設定
	m_Context->OMSetDepthStencilState(g_pDepthStencilState, 0);

	m_Context->RSSetState(g_pRasterizerState);

	// (OutputManger)RnderTagetの指定
	m_Context->OMSetRenderTargets(
		1,							// 使用するViewの数
		&m_RenderTargetView,		// 使用するRenderTargetView
		m_DepthStencilView);		// 使用するDepthStencilView
}

void DirectGraphics::SetUpDxgiSwapChainDesc(DXGI_SWAP_CHAIN_DESC *dxgi)
{
	HWND window_handle = FindWindow(Window::ClassName, nullptr);
	RECT rect;
	GetClientRect(window_handle, &rect);

	/*
		DirectX11版PresentationParameter
			バッファの数やサイズ、カラーフォーマット等を設定する
	*/
	ZeroMemory(dxgi, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgi->BufferCount = 1;									// バッファの数
	dxgi->BufferDesc.Width = (rect.right - rect.left);		// バッファの横幅
	dxgi->BufferDesc.Height = (rect.bottom - rect.top);		// バッファの縦幅
	dxgi->BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// カラーフォーマット
	dxgi->BufferDesc.RefreshRate.Numerator = 60;			// リフレッシュレートの分母
	dxgi->BufferDesc.RefreshRate.Denominator = 1;			// リフレッシュレートの分子
	dxgi->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// バッファの使い方 Usage => 使用方法
	dxgi->OutputWindow = window_handle;						// 出力対象のウィンドウハンドル
	dxgi->SampleDesc.Count = 1;								// マルチサンプリングのサンプル数(未使用は1)
	dxgi->SampleDesc.Quality = 0;							// マルチサンプリングの品質(未使用は0)
	dxgi->Windowed = true;									// ウィンドウモード指定
}

bool DirectGraphics::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC dxgi;
	SetUpDxgiSwapChainDesc(&dxgi);

	D3D_FEATURE_LEVEL level;
	// デバイス生成とスワップチェーン作成を行う
	if (FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,					// ビデオアダプタ指定(nullptrは既定)
		D3D_DRIVER_TYPE_HARDWARE,	// ドライバのタイプ
		nullptr,					// D3D_DRIVER_TYPE_SOFTWARE指定時に使用
		0,							// フラグ指定
		nullptr,					// D3D_FEATURE_LEVEL指定で自分で定義した配列を指定可能
		0,							// 上のD3D_FEATURE_LEVEL配列の要素数
		D3D11_SDK_VERSION,			// SDKバージョン
		&dxgi,						// DXGI_SWAP_CHAIN_DESC
		&m_SwapChain,				// 関数成功時のSwapChainの出力先 
		&m_Device,					// 関数成功時のDeviceの出力先
		&level,						// 成功したD3D_FEATURE_LEVELの出力先
		&m_Context)))				// 関数成功時のContextの出力先
	{
		return false;
	}

	return true;
}

bool DirectGraphics::CreateRenderTargetView()
{
	// RenderTargetViewの対象となるBufferの取得
	ID3D11Texture2D* back_buffer;
	if (FAILED(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer)))
	{
		return false;
	}
	
	// BufferからRenderTargetViewの作成
	if (FAILED(m_Device->CreateRenderTargetView(back_buffer, NULL, &m_RenderTargetView)))
	{
		return false;
	}
	
	// Targetの取得終わったのでBufferを解放
	back_buffer->Release();

	return true;
}

bool DirectGraphics::CreateDepthAndStencilView()
{
	HWND window_handle = FindWindow(Window::ClassName, nullptr);
	RECT rect;
	GetClientRect(window_handle, &rect);

	//深度ステンシルバッファ作成
    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory( &texture_desc, sizeof( D3D11_TEXTURE2D_DESC ) );
    texture_desc.Width              = (rect.right - rect.left);			// 横幅
    texture_desc.Height             = (rect.bottom - rect.top);			// 縦幅
    texture_desc.MipLevels          = 1;
    texture_desc.ArraySize          = 1;
    texture_desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;	// テクスチャーフォーマット
	texture_desc.SampleDesc.Count   = 1;								
    texture_desc.SampleDesc.Quality = 0;
    texture_desc.Usage              = D3D11_USAGE_DEFAULT;				// テクスチャの使用方法(デフォルト)
    texture_desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;			// Bind設定はDepth、Stencilに設定
    texture_desc.CPUAccessFlags     = 0;
    texture_desc.MiscFlags          = 0;

	// texture_descの情報でテクスチャを作成
	if (FAILED(m_Device->CreateTexture2D(&texture_desc, NULL, &m_DepthStencilTexture)))
	{
		return false;
	}

	// Depth、StencilViewの設定
	// DepthStencilView 

    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    ZeroMemory( &dsv_desc, sizeof( dsv_desc ) );
    dsv_desc.Format             = texture_desc.Format;				// Viewのフォーマット(Textureのものを使用)
    dsv_desc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;	
    dsv_desc.Texture2D.MipSlice = 0;								// 最初に使用するミップマップのレベルを指定 


	// CreateTexture2Dとdsv_descからDepthとStencilバッファを作る
	if (FAILED(m_Device->CreateDepthStencilView(
				m_DepthStencilTexture,				// DSVとして使用されるTexture
				&dsv_desc,							// DSVの設定
				&m_DepthStencilView)))				// ID3D11DepthStencilViewの出力先
	{
		return false;
	}



	return true;
}

void DirectGraphics::CreateBlendState() {
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.AlphaToCoverageEnable = TRUE;
	BlendState.IndependentBlendEnable = TRUE;
	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_Device->CreateBlendState(&BlendState, &g_pBlendState);

	////ブレンディングステート生成
	//ID3D11BlendState* hpBlendState = NULL;
	//D3D11_BLEND_DESC BlendStateDesc;
	//BlendStateDesc.AlphaToCoverageEnable = TRUE;
	//BlendStateDesc.IndependentBlendEnable = FALSE;
	//for (int i = 0; i < 8; i++) {
	//	BlendStateDesc.RenderTarget[i].BlendEnable = TRUE;
	//	BlendStateDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//	BlendStateDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//	BlendStateDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
	//	BlendStateDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
	//	BlendStateDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
	//	BlendStateDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//	BlendStateDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//}
	//m_Device->CreateBlendState(&BlendStateDesc, &hpBlendState);
}



void DirectGraphics::SetUpViewPort()
{
 	HWND window_handle = FindWindow(Window::ClassName, nullptr);
	RECT rect;
	GetClientRect(window_handle, &rect);

	//ビューポートの設定
	D3D11_VIEWPORT view_port;
	view_port.TopLeftX = 0;								// 左上X座標
	view_port.TopLeftY = 0;								// 左上Y座標
	view_port.Width = (float)(rect.right - rect.left);	// 横幅
	view_port.Height = (float)(rect.bottom - rect.top);	// 縦幅
	view_port.MinDepth = 0.0f;							// 最小深度
	view_port.MaxDepth = 1.0f;							// 最大深度

	m_Context->RSSetViewports(	
		1,					// 設定するビューポートの数
		&view_port );		// 設定するビューポート情報のポインタ
}

bool DirectGraphics::CreateTextureSampler()
{
	D3D11_SAMPLER_DESC sampler_desc;

	ZeroMemory(&sampler_desc, sizeof(D3D11_SAMPLER_DESC));

	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// サンプリング時の補間方法
	// UVW値が0.0～1.0の範囲外になった場合の対応設定
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MipLODBias = 0.0f;
	sampler_desc.MaxAnisotropy = 2;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.BorderColor[0] = 0.0f;
	sampler_desc.BorderColor[1] = 0.0f;
	sampler_desc.BorderColor[2] = 0.0f;
	sampler_desc.BorderColor[3] = 0.0f;
	sampler_desc.MinLOD = -FLT_MAX;
	sampler_desc.MaxLOD = FLT_MAX;

	if (FAILED(m_Device->CreateSamplerState(&sampler_desc, &m_SamplerState)))
	{
		return false;
	}
	return true;
}

bool DirectGraphics::CreateConstantBuffer()
{
	/*
		Constantバッファー作成
			コンスタントバッファーはCPU側のデータを
			まとめてGPU側に送信するためのバッファー

			バッファーには座標変換行列などを設定する
	*/
	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.ByteWidth = sizeof(ConstantBuffer);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	if (FAILED(m_Device->CreateBuffer(&buffer_desc, nullptr, &m_ConstantBuffer)))
	{
		return false;
	}

	return true;
}


