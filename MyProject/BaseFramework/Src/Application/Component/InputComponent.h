#pragma once

#include "../Game/GameObject.h"

// コントローラーを作るイメージ？

namespace Input
{
	enum Axes	// axisの複数形
	{
		L,
		R,
		AXS_MAX
	};

	enum Buttons
	{
		A,
		B,
		X,
		Y,
		L1,
		R1,
		BTN_MAX
	};
}

class InputComponent
{
public:

	// ボタン状態
	enum
	{
		FREE	= 0x00000000,	// 何も押していない
		ENTER	= 0x00000001,	// 押した
		STAY	= 0x00000002,	// 押し続けている
		EXIT	= 0x00000004,	// 離された
	};
	// コンストラクター：オーナーの設定・ボタンの初期化
	InputComponent(GameObject& owner);

	virtual ~InputComponent() {};

	// 入力の準備
	virtual void Update() {};

	// 操作軸取得
	inline const Math::Vector2& GetAxiz(Input::Axes no) const
	{
		// マックス値ならエラーが出る
		assert(no != Input::Axes::AXS_MAX);
		return m_axes[no];
	}

	// ボタンフラグ取得
	inline int GetButton(Input::Buttons no) const
	{
		// マックス値ならエラーが出る
		assert(no != Input::Buttons::BTN_MAX);
		return m_buttons[no];
	}

	// ボタンを押した
	void PushButton(Input::Buttons no);
	// ボタンを離す
	void ReleaseButton(Input::Buttons no);

protected:

	// 操作軸
	std::array<Math::Vector2, Input::Axes::AXS_MAX> m_axes;
	// ボタン軸
	std::array<int, Input::Buttons::BTN_MAX> m_buttons;
	// 持ち主
	GameObject& m_owner;
};

// ===========================================
// キーボード用入力コンポーネント
// (Player入力用)
// ===========================================
class PlayerInputComponent : public InputComponent
{
public:
	// 初期化の時にInputComponentの初期化を呼ぶ
	PlayerInputComponent(GameObject& owner) : InputComponent(owner){}

	virtual void Update() override;
};

// ===========================================
// 敵用入力コンポーネント
// ===========================================
class EnemyInputComponent : public InputComponent
{
public:
	// 初期化の時にInputComponentの初期化を呼ぶ
	EnemyInputComponent(GameObject& owner) : InputComponent(owner) {}

	virtual void Update() override;

	float rotate = 0.0f;	// 先生が後から追加していたやつ
};

//地面を動き回るキャラクタ用のInputコンポーネント
//WASD:移動
//マウス:カメラ回転
class ActionPlayerInputComponent :public InputComponent
{
public:
	ActionPlayerInputComponent(GameObject& rOwner) :InputComponent(rOwner) {}

	virtual void Update()override;

	POINT m_prevMousePos;
};