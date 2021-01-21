#include "InputComponent.h"
#include "../Game/Scene.h"

// コンストラクター：オーナーの設定・ボタンの初期化
InputComponent::InputComponent(GameObject& owner) : m_owner(owner)
{
	// 操作軸初期化
	for (auto& axes : m_axes)
	{
		// 毎フレーム初期化する
		axes = { 0.0f,0.0f };
	}
	// ボタン配列の中身をFREEで埋め尽くす
	m_buttons.fill(FREE);
}

// ボタンを押す
void InputComponent::PushButton(Input::Buttons no)
{
	assert(no != Input::Buttons::BTN_MAX);

	// 押している
	if (m_buttons[no] & STAY)
	{
		m_buttons[no] &= ~ENTER;	// ENTER反転とAND　＝　ENTERだけをOFF
	}
	// 押していない
	else
	{
		m_buttons[no] |= ENTER;		// ENTERをOR	＝　ENTERだけON
		m_buttons[no] |= STAY;		// STAYをOR		＝　STAYをON
	}
}

// ボタンを離す
void InputComponent::ReleaseButton(Input::Buttons no)
{
	assert(no != Input::Buttons::BTN_MAX);

	// 押している
	if (m_buttons[no] & STAY)
	{
		m_buttons[no] &= ~ENTER;	// ENTER反転とAND　＝　ENTERだけをOFF
		m_buttons[no] &= ~STAY;		// STAY反転とAND　 ＝　STAYだけをOFF
		m_buttons[no] |= EXIT;		// EXITをOR		   ＝　EXITだけON
	}
	// 押していない
	else
	{
		m_buttons[no] &= ~EXIT;		// EXIT反転とAND　＝　EXITだけをOFF
	}
}

// 更新
void PlayerInputComponent::Update()
{
	// 操作軸初期化
	for (auto& axes : m_axes)
	{
		axes = { 0.0f,0.0f };
	}

	// [左の軸値]入力処理
	if (GetAsyncKeyState(VK_UP) & 0x8000) { m_axes[Input::Axes::L].y = 1.0f; }
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) { m_axes[Input::Axes::L].y = -1.0f; }
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { m_axes[Input::Axes::L].x = 1.0f; }
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) { m_axes[Input::Axes::L].x = -1.0f; }

	// [右の軸値]入力処理
	if (GetAsyncKeyState('W') & 0x8000) { m_axes[Input::Axes::R].y = 1.0f; }
	if (GetAsyncKeyState('S') & 0x8000) { m_axes[Input::Axes::R].y = -1.0f; }
	if (GetAsyncKeyState('D') & 0x8000) { m_axes[Input::Axes::R].x = 1.0f; }
	if (GetAsyncKeyState('A') & 0x8000) { m_axes[Input::Axes::R].x = -1.0f; }

	// [ボタン]入力処理
	if (GetAsyncKeyState('Z')) { PushButton(Input::Buttons::A); }
	else { ReleaseButton(Input::Buttons::A); }

	if (GetAsyncKeyState('X')) { PushButton(Input::Buttons::B); }
	else { ReleaseButton(Input::Buttons::B); }

	if (GetAsyncKeyState('C')) { PushButton(Input::Buttons::X); }
	else { ReleaseButton(Input::Buttons::X); }

	if (GetAsyncKeyState('V')) { PushButton(Input::Buttons::Y); }
	else { ReleaseButton(Input::Buttons::Y); }

	if (GetAsyncKeyState('Q')) { PushButton(Input::Buttons::L1); }
	else { ReleaseButton(Input::Buttons::L1); }

	if (GetAsyncKeyState('E')) { PushButton(Input::Buttons::R1); }
	else { ReleaseButton(Input::Buttons::R1); }
}

void EnemyInputComponent::Update()
{
	m_axes[Input::Axes::L].y = 1.0f;
	m_axes[Input::Axes::R].y = 1.0f;

	// 先生が後から追加していたやつ
	rotate += 0.006f;
	m_axes[Input::Axes::R].x = sin(rotate);
}

void ActionPlayerInputComponent::Update()
{
	//各種の初期化
	for (auto& rAxis : m_axes)
	{
		rAxis = { 0.0f,0.0f };
	}

	//左軸の入力
	if (GetAsyncKeyState('W') & 0x8000) { m_axes[Input::Axes::L].y = 1.0f; }
	if (GetAsyncKeyState('S') & 0x8000) { m_axes[Input::Axes::L].y = -1.0f; }
	if (GetAsyncKeyState('D') & 0x8000) { m_axes[Input::Axes::L].x = 1.0f; }
	if (GetAsyncKeyState('A') & 0x8000) { m_axes[Input::Axes::L].x = -1.0f; }



	//右側の入力（マウスの入力）
	m_prevMousePos.x = 1280/2;
	m_prevMousePos.y = 720/2;
	POINT nowMousePos;
	GetCursorPos(&nowMousePos);	//マウスの現在位置の取得
	m_axes[Input::R].x = (float)(nowMousePos.x - m_prevMousePos.x);
	m_axes[Input::R].y = (float)(nowMousePos.y - m_prevMousePos.y);

	if (!Scene::GetInstance().EditorCameraEnable)
	{
		SetCursorPos(m_prevMousePos.x, m_prevMousePos.y);
	}

	//ボタン入力
	//ジャンプ処理
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) { PushButton(Input::Buttons::A); }
	else { ReleaseButton(Input::Buttons::A); }
	//ダッシュ処理
	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) { PushButton(Input::Buttons::L1); }
	else { ReleaseButton(Input::Buttons::L1); }

	if (GetAsyncKeyState(VK_RETURN)&0x8000) { PushButton(Input::Buttons::R1); }
	else { ReleaseButton(Input::Buttons::R1); }

	if (GetAsyncKeyState(VK_MBUTTON) & 0x8000) { PushButton(Input::Buttons::Y); }
	else { ReleaseButton(Input::Buttons::Y); }

	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) { PushButton(Input::Buttons::B); }
	else { ReleaseButton(Input::Buttons::B); }

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) { PushButton(Input::Buttons::X); }
	else { ReleaseButton(Input::Buttons::X); }
}
