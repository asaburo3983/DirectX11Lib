struct PS_IN
{
        float4 pos : SV_POSITION;
		float4 color : COLOR;
		// �ǉ�
		float2 texture_pos : TEXTURE0;
};

cbuffer ConstantBuffer
{
	float4x4	World;				// ���[���h�ϊ��s��
	float4x4	View;				// �r���[�ϊ��s��
	float4x4	Projection;			// �����ˉe�ϊ��s��
	float4		CameraPos;			// �J�������W
	float4		LightVector;		// ���C�g����
	float4		LightColor;			// ���C�g�J���[
	float4		MaterialAmbient;	// �A���r�G���g
	float4		MaterialDiffuse;	// �f�B�t���[�Y
	float4		MaterialSpecular;	// �X�y�L�����[
}

Texture2D    Texture : register(t0[0]); // Texture���X���b�g0��0�Ԗڂ̃e�N�X�`�����W�X�^�ɐݒ�
SamplerState Sampler : register(s0[0]); // Sampler���X���b�g0��0�Ԗڂ̃T���v�����W�X�^�ɐݒ�

float4 main(PS_IN input) : SV_Target
{
	// �e�N�X�`���J���[�̎擾
	float4 tex_color = Texture.Sample(Sampler, input.texture_pos);
	if (tex_color.a <= 0.0)discard;//a�O�l��\������

	//�����\��
	//if (tex_color.r <= 0.5&&tex_color.r <= 0.5&&tex_color.r <= 0.5)
	//	tex_color.r = 0, tex_color.g = 0, tex_color.b = 0;

	//if(tex_color.r > 0.5&&tex_color.r > 0.5&&tex_color.r > 0.5)
	//	tex_color.r = 1, tex_color.g = 1, tex_color.b = 1;

	// �A���r�G���g�J���[ + �f�B�t���[�Y�J���[ + �e�N�X�`���J���[
	return tex_color;

}