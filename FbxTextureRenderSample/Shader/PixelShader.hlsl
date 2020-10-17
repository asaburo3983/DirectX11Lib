struct PS_IN
{
        float4 pos : SV_POSITION;
		float4 color : COLOR;
		// 追加
		float2 texture_pos : TEXTURE0;
};

cbuffer ConstantBuffer
{
	float4x4	World;				// ワールド変換行列
	float4x4	View;				// ビュー変換行列
	float4x4	Projection;			// 透視射影変換行列
	float4		CameraPos;			// カメラ座標
	float4		LightVector;		// ライト方向
	float4		LightColor;			// ライトカラー
	float4		MaterialAmbient;	// アンビエント
	float4		MaterialDiffuse;	// ディフューズ
	float4		MaterialSpecular;	// スペキュラー
}

Texture2D    Texture : register(t0[0]); // Textureをスロット0の0番目のテクスチャレジスタに設定
SamplerState Sampler : register(s0[0]); // Samplerをスロット0の0番目のサンプラレジスタに設定

float4 main(PS_IN input) : SV_Target
{
	// テクスチャカラーの取得
	float4 tex_color = Texture.Sample(Sampler, input.texture_pos);
	if (tex_color.a <= 0.0)discard;//a０値非表示処理

	//白黒表示
	//if (tex_color.r <= 0.5&&tex_color.r <= 0.5&&tex_color.r <= 0.5)
	//	tex_color.r = 0, tex_color.g = 0, tex_color.b = 0;

	//if(tex_color.r > 0.5&&tex_color.r > 0.5&&tex_color.r > 0.5)
	//	tex_color.r = 1, tex_color.g = 1, tex_color.b = 1;

	// アンビエントカラー + ディフューズカラー + テクスチャカラー
	return tex_color;

}
