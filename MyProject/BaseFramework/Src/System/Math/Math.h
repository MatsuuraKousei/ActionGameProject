#pragma once

// 3Dベクトル
class Vector3 : public DirectX::XMFLOAT3	// ベクトルを保存する型を継承する
{
public:

	// 指定行列でVectorを変換する
	Vector3& TransformCoord(const DirectX::XMMATRIX& m)
	{
		*this = XMVector3TransformCoord(*this, m);
		return *this;
	}

	// 指定(回転)行列でVectorを変換する(移動は変わらない　回転だけ)
	Vector3& TransformNormal(const DirectX::XMMATRIX& m)
	{
		*this = XMVector3TransformNormal(*this, m);
		return *this;
	}

	// デフォルトコンストラクタ(引数なし)は座標の0クリアを行う
	Vector3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	// 座標指定付きコンストラクタ
	Vector3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	// XMVECTORから代入してきた時
	Vector3(const DirectX::XMVECTOR& v)	// XMVECTORは計算用のベクター
	{
		// 変換して代入
		DirectX::XMStoreFloat3(this, v);	// 計算用から保存用に変換？
		// 下記と同意だが、上記のSIMD命令を使用したほうが高速
		// x = v.m128_f32[0];
		// y = v.m128_f32[1];
		// z = v.m128_f32[2];
	}

	// XMVECTORへ変換
	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat3(this); }

	// Math::Vector3と互換性を持つための関数
	// 自分自身のポインタをMath::Vector3に変換して実体かしている
	operator Math::Vector3& () { return *(Math::Vector3*)this; }

	// 算術演算子　乗算(＊)
	// *=   って処理が書かれたらここを通るようになる？
	Vector3& operator*=(float s)
	{
		*this = DirectX::XMVectorScale(*this, s);
		return *this;
	}

	Vector3& operator+ (Vector3& vec)
	{
		Vector3 vec2;

		vec2.x = x + vec.x;
		vec2.y = y + vec.y;
		vec2.z = z + vec.z;

		return vec2;
	}

	// 自分を正規化
	void Normalize()
	{
		*this = DirectX::XMVector3Normalize(*this);
	}

	// 長さ
	float Length() const
	{
		return DirectX::XMVector3Length(*this).m128_f32[0];
	}

	// 長さの２乗(高速なので判定用に使用することが多い)
	float LengthSquared() const
	{
		return DirectX::XMVector3LengthSq(*this).m128_f32[0];
	}

	// 内積(１～-１の値が取得できる)
	//     (0°～180°)
	static float Dot(const Vector3& v1, const Vector3& v2)
	{
		return DirectX::XMVector3Dot(v1, v2).m128_f32[0];
	}
	
	// 外積(2つのベクトルの直角となるベクトルを求めるらしい)
	static Vector3 Cross(const Vector3& v1, const Vector3& v2)
	{
		return DirectX::XMVector3Cross(v1, v2);
	}

	inline void Complement(const Vector3& vTo, float rot) {
		// ※※※※※回転軸作成(この軸で回転する)※※※※※
		Vector3 vRotAxis = Vector3::Cross(*this, vTo);

		// 0ベクトルなら回転しない
		if (vRotAxis.LengthSquared() > 0)
		{
			// 自分のZ方向ベクトルと自身からターゲットへ向かうベクトルの内積
			float Zvector = Vector3::Dot(*this, vTo);

			//クランプ
			Zvector = std::clamp(Zvector, -1.0f * Radians, 1.0f * Radians);

			// 自分の前方向ベクトルと自身からターゲットへ向かうベクトル間の角度(radian)
			float radian = acos(Zvector); // ラジアン角に変換

			//度数法
			

			// 角度制限　１フレームにつき最大で１度以上回転しない
			if (radian > rot * Radians)
			{
				radian = rot * Radians;
			}

			DirectX::XMMATRIX mRot;

			mRot = DirectX::XMMatrixRotationAxis(vRotAxis, radian);

			*this = TransformNormal(mRot);


			// radian分thisを回転させる
			DirectX::XMMATRIX rotateMat;

			rotateMat = DirectX::XMMatrixRotationAxis(vRotAxis, radian);

			Vector3 CameraAngleEffect(rotateMat.r[3]);

			DirectX::XMMATRIX mat = DirectX::XMMatrixRotationAxis(vRotAxis, radian);

			*this = Vector3({ this->x + CameraAngleEffect.x,this->y + CameraAngleEffect.y,this->z + CameraAngleEffect.z });

			this->Normalize();
		}
	}
};

// 4 * 4 の行列
class Matrix : public DirectX::XMFLOAT4X4
{
public:

	// デフォルトコンストラクタは単位行列か
	Matrix()
	{
		*this = DirectX::XMMatrixIdentity();
	}

	// XMMatrixから代入してきた
	Matrix(const DirectX::XMMATRIX& m)
	{
		DirectX::XMStoreFloat4x4(this, m);
	}

	// XMFLOAT4X4,Math::Matrixから代入してきた
	Matrix(const DirectX::XMFLOAT4X4& m)
	{
		// メモリのコピー
		// 速度を求めない時はこれはやらないほうがいい
		memcpy_s(this, sizeof(float) * 16, &m, sizeof(XMFLOAT4X4));
	}

	// XMMatrixへ変換
	operator DirectX::XMMATRIX() const
	{
		return DirectX::XMLoadFloat4x4(this);
	}

	// Math::Matrixと互換性を持つための関数
	operator Math::Matrix& () { return *(Math::Matrix*)this; }

	// 代入演算子　乗算
	Matrix& operator *= (const Matrix& m)
	{
		*this = DirectX::XMMatrixMultiply(*this, m);
		return *this;
	}

	// 作成＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

	// 移動行列作成
	void CreateTranslation(float x, float y, float z)
	{
		*this = DirectX::XMMatrixTranslation(x, y, z);
	}

	// 座標を直接格納(回転行列の情報が消えない 自作)
	void SetTranslation(float x, float y, float z)
	{
		_41 = x;
		_42 = y;
		_43 = z;
	}

	// X回転行列作成
	void CreateRotationX(float angle)
	{
		*this = DirectX::XMMatrixRotationX(angle);
	}

	// 逆行列にする
	void Inverse()
	{
		*this = DirectX::XMMatrixInverse(nullptr, *this);
	}

	// 拡縮行列作成
	void CreateScalling(float x, float y, float z)
	{
		*this = DirectX::XMMatrixScaling(x, y, z);
	}

	//クォーターニオンから回転行列作成
	void CreateFromQuaternion(const Math::Quaternion& quat)
	{
		*this = DirectX::XMMatrixRotationQuaternion(quat);
	}

	// 指定軸回転行列作成
	void CreateRotationAxis(const Vector3& axis, float angle)
	{
		*this = DirectX::XMMatrixRotationAxis(axis, angle);
	}

	// 透視影行列の作成
	Matrix& CreateProjectionPerspectiveFov(float fovAngleY, float aspectRation,float nearZ, float farZ)
	{
		*this = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRation, nearZ, farZ);
		return *this;
	}

	// 操作＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

	// X軸回転
	void RotateX(float angle)
	{
		*this *= DirectX::XMMatrixRotationX(angle);
	}

	// Y軸回転
	void RotateY(float angle)
	{
		*this *= DirectX::XMMatrixRotationY(angle);
	}

	// Z軸回転
	// 	rotateMat.RotateZ(rotate.z * KdToRadians);  のようにラジアン角に変更すること！！！！！
	void RotateZ(float angle)
	{
		*this *= DirectX::XMMatrixRotationZ(angle);
	}

	// 拡縮
	void Scale(float x, float y, float z)
	{
		*this *= DirectX::XMMatrixScaling(x, y, z);
	}

	// 移動関数
	void Move(const Vector3& v)
	{
		_41 += v.x;
		_42 += v.y;
		_43 += v.z;
	}

	// プロパティ(属性)(取得)＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

	// X軸取得
	Vector3 GetAxisX() const { return { _11,_12,_13 }; }
	// Y軸取得
	Vector3 GetAxisY() const { return { _21,_22,_23 }; }
	// Z軸取得
	Vector3 GetAxisZ() const { return { _31,_32,_33 }; }

	// X軸セット
	void SetAxisX(const Vector3& v)
	{
		_11 = v.x;
		_12 = v.y;
		_13 = v.z;
	}

	// Y軸セット
	void SetAxisY(const Vector3& v)
	{
		_21 = v.x;
		_22 = v.y;
		_23 = v.z;
	}

	// Z軸セット
	void SetAxisZ(const Vector3& v)
	{
		_31 = v.x;
		_32 = v.y;
		_33 = v.z;
	}

	// 座標取得
	Vector3 GetTranslation() const {return { _41,_42,_43 };}

	void SetTranslation(const Vector3& v)
	{
		_41 = v.x;
		_42 = v.y;
		_43 = v.z;
	}

	// XYZの順番で合成したときの回転角度を算出する
	Vector3 GetAngles() const
	{
		Matrix mat = *this;

		// 各軸を取得
		Vector3 axisX = mat.GetAxisX();
		Vector3 axisY = mat.GetAxisY();
		Vector3 axisZ = mat.GetAxisZ();

		// 各軸を正規化
		axisX.Normalize();
		axisY.Normalize();
		axisZ.Normalize();

		// マトリックスを正規化
		mat.SetAxisX(axisX);
		mat.SetAxisY(axisY);
		mat.SetAxisZ(axisZ);

		// 各軸の角度
		Vector3 angles;
		angles.x = atan2f(mat.m[1][2], mat.m[2][2]);
		angles.y = atan2f(-mat.m[0][2], sqrtf(mat.m[1][2] * mat.m[1][2] + mat.m[2][2] * mat.m[2][2]));
		angles.z = atan2f(mat.m[0][1], mat.m[0][0]);

		return angles;
	}

	

	// 自分で作ってもいい
	// 例えば２点の間を求めるとか
	// 算術系はヘッダーの中での実体の宣言をしても大丈夫

	////////////////////////////////////////////////
	inline void SetBillboard(const Matrix& mat) 
	{
		// 参照した値を取得
		Matrix TargetMat = mat;

		// 取得した値の逆行列を取得
		TargetMat.Inverse();

		//記憶
		Vector3 pos = this->GetTranslation();

		//初期化
		this->SetTranslation({ 0,0,0 });

		// 逆行列と自身の行列を合成
		*this *= TargetMat;

		// 自身の座標を復元
		this->SetTranslation(pos);
	}
	/////////////////////////////////////////////////
};

// KdMatrix同士の合成
inline Matrix operator* (const Matrix& M1, const Matrix& M2)
{
	using namespace DirectX;
	return XMMatrixMultiply(M1, M2);
}

//========================================
//
// クォーターニオン
//
//========================================
class Quaternion :public DirectX::XMFLOAT4
{
public:
	//Constructorで初期値代入（とりあえず安全
	Quaternion()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}

	//XMVECTORから代入してきた時
	Quaternion(const DirectX::XMVECTOR& V)
	{
		//変換して代入
		DirectX::XMStoreFloat4(this, V);
	}

	//XMVECTORへ変換
	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat4(this); }
};