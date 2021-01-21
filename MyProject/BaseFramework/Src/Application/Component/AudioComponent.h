#pragma once

//SoundEffect->SoundInstance->AudioManager
//音源→データ→操作

class SoundInstance;
class SoundEffect;

namespace Track
{
	enum Music
	{
		// タイトル
		Title_BGM,
		ItemGet
	};

}

class SoundInstance : public std::enable_shared_from_this<SoundInstance> {
	friend class SoundEffect;
public:


	std::shared_ptr <SoundInstance> Deserialize(Track::Music music);
	std::string Conv(Track::Music music, bool &b3D);
	//==================================================================
	// 取得
	//==================================================================
	DirectX::AudioEmitter& GetEmitter() { return m_emitter; }
	std::shared_ptr<SoundEffect>	GetSoundEffect() { return m_srcData; }

	//==================================================================
	// 設定
	//==================================================================
	// 初期設定
	void Init(std::unique_ptr<DirectX::SoundEffectInstance> inst, const std::shared_ptr<SoundEffect>& soundEffect, bool enable3D)
	{
		m_instance = std::move(inst);
		m_srcData = soundEffect;
		m_is3D = enable3D;
	}

	//==================================================================
	// 再生
	//==================================================================

	// 2D再生
	// ・loop		… ループ再生する？
	// ・manage		… AudioManagerが再生管理をする？
	void Play(bool loop = false, bool manage = true);

	//// 3D再生　※モノラルサウンドのみ3D再生可能です
	//// ・pos		… 座標
	//// ・loop		… ループ再生するか？
	//// ・manage		… AudioManagerが再生管理をする？
	void Play3D(const Math::Vector3& pos, bool loop = false, bool manage = true);

	// m_Emitterの情報を適用する
	void Apply3D();

	//==================================================================
	// 停止
	//==================================================================
	// 停止
	void Stop() {
		if (m_instance)m_instance->Stop();
	}

	// 一時停止
	void Pause() {
		if (m_instance)m_instance->Pause();
	}

	// 再開
	void Resume() {
		if (m_instance)m_instance->Resume();
	}

	////==================================================================
	//// 設定
	////==================================================================
	//// ボリューム設定
	//// ・vol	… ボリューム設定(1.0が100%)
	//void SetVolume(float vol);

	// 3Dサウンド座標設定
	// ・pos		… 座標
	void SetPos(const Math::Vector3& pos);

	// 減衰倍率設定 1:通常 FLT_MIN～FLT_MAX
	void SetCurveDistanceScaler(float val);

	//==================================================================
	// 取得
	//==================================================================
	// 再生中？
	bool IsPlay();

	//// ポーズ中？
	//bool IsPause();

	//==================================================================
	~SoundInstance();


private:

	// 
	SoundInstance() {}

private:

	// サウンドの再生インスタンス
	std::unique_ptr<DirectX::SoundEffectInstance>	m_instance;

	// 再生サウンドの元データ
	std::shared_ptr<SoundEffect>					m_srcData;

	// エミッター 主に3Dサウンドソースの定義
	DirectX::AudioEmitter							m_emitter;

	// 3Dサウンドか
	bool											m_is3D = false;

private:
	// コピー禁止用
	SoundInstance(const SoundInstance& src) = delete;
	void operator=(const SoundInstance& src) = delete;
};


class SoundEffect : public std::enable_shared_from_this<SoundEffect> {
public:

	// WAVEサウンド読み込み
	// ・filename	… ファイル名
	// 戻り値		… 成功：true
	bool Load(const std::string& fileName);

	// 再生用のサウンドインスタンス作成
	// ・b3D			… 3Dサウンドにするか？
	// ・戻り値		… 新たに作成した再生用サウンドインスタンス
	std::shared_ptr<SoundInstance> CreateInstance(bool b3D);

	~SoundEffect() {
		m_soundEffect = nullptr;
		m_fileName = "";
	}

	// 
	SoundEffect() {}

private:
	// サウンドエフェクト
	std::unique_ptr<DirectX::SoundEffect>	m_soundEffect;

	// ファイル名
	std::string								m_fileName;

private:
	// コピー禁止用
	SoundEffect(const SoundEffect& src) = delete;
	void operator=(const SoundEffect& src) = delete;
};


class AudioManager {
	friend class SoundInstance;
public:
	//==================================================================
	// 取得系
	//==================================================================
	//  AudioEngine取得
	std::unique_ptr<DirectX::AudioEngine>& GetAudioEngine() { return m_audioEng; }

	// 3Dリスナー(聞き手)取得
	DirectX::AudioListener& GetListener() { return m_listener; }

	// 初期化されているか？
	bool	IsInit() { return (m_audioEng != nullptr); }

	// 現在再生中のサウンド数取得
	int		GetNumPlaying() { return (int)m_playList.size(); }


	//==================================================================
	// 初期化・解放
	//==================================================================

	// 初期化
	bool Init();

	// 解放
	void Release();

	//==================================================================
	// 再生サウンド操作
	//==================================================================

	// すべて停止
	void StopAll();

	//==================================================================
	// 処理
	//==================================================================
	// 処理　定期的に実行してください
	// 再生中のサウンドの管理やリスナーの設定繁栄などを行っています
	void Update();
	void UpdatePos(Vector3 pos);
	void UpdateAngle(Vector3 angle);
	//==================================================================
	~AudioManager() {
		Release();
	}

private:

	// AudioEngine
	std::unique_ptr<DirectX::AudioEngine>				m_audioEng;

	// 3Dサウンド用リスナー
	DirectX::AudioListener								m_listener;

	// 現在再生中のサウンド管理リスト
	std::map<UINT, std::shared_ptr<SoundInstance>>		m_playList;

private:

	// 再生リスト追加。実際に再生されているものになる。
	void AddPlayList(const std::shared_ptr<SoundInstance>& snd) {
		if (snd == nullptr)return;
		m_playList[(UINT)snd.get()] = snd;
	}

	//本家大本
private:
	AudioManager() {}

public:
	static AudioManager& GetInstance()
	{
		static AudioManager	instance;
		return instance;
	}
};

#define AUDIO AudioManager::GetInstance()