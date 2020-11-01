#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define AngleToRadian(angle) (angle/180.0*3.14)
#define RadianToAngle(radian) (radian*180.0/3.14)

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

namespace GameMath {
	struct Pos2 {
		float p[2];//xy
	};
	struct Pos3 {
		float p[3];//xyz
	};
	//四角判定のクラス
	class HitObject {
	private:
		int x0, x1, y0, y1;
	public:
		HitObject(int _x0, int _y0, int _x1, int _y1) {
			x0 = _x0;
			x1 = _x1;
			y0 = _y0;
			y1 = _y1;
		}
		bool Hit(int px, int py) {
			if (x0 <= px && px <= x1 && y0 <= py && py <= y1)
				return true;
			return false;
		}
	};

	//三角関数//////////////////////////////
	////原点と角度と半径を送ると位置が返ってくる 右回り
	void GetCirclePoint(float posX, float posY, float r, float  angle, float* px, float* py);
	//底辺と高さから斜辺を取り出す
	float GetHypotenuse(int x, int y);
	float GetCosWave(float angle);
	float GetSinWave(float angle);
	////////////////////////////////////////////////////////////
	//当たり判定//////////////////////////////
	//中心点を描画軸とする四角画像の当たり判定
	bool HitBar(float center, float size, float point);
	bool HitRect(Pos2 center, Pos2 size, Pos2 point);//矩形
	bool HitCircle(Pos2 center1, float r1, Pos2 center2, float r2);//円

	bool HitTriangle(Pos2 pos0, Pos2 pos1, Pos2 pos2, Pos2 point);

	//当たり判定の処理３D/////////////////////////////////////////////////
	bool HitRect3D(Pos3 center, Pos3 size, Pos3 point);
	bool HitCircle3D(Pos3 center1, float r1, Pos3 center2, float r2);
	////////////////////////////////////////////////////////////
	//ファイル操作//////////////////////////////


	////////////////////////////////////////////////////////////


}