#pragma once

#include "../Game/GameObject.h"

namespace Track
{
	enum Music
	{
		TitleBGM,
		GameBGM,
		BossBGM,
		GameOverBGM
	};

	enum SE
	{
		ItemGet
	};

	struct MusicData
	{
		std::wstring name;
		std::unique_ptr<DirectX::SoundEffect> m_SoundEffect;
		Track::Music music;
	};
}

class SoundComponent
{
public:

	SoundComponent(GameObject& owner);
	virtual ~SoundComponent() {};

	virtual void Desirealise();
	virtual void Update();

	void Play(Track::Music music);
protected:

	std::unique_ptr<DirectX::AudioEngine>     m_audioEng = nullptr; //サウンドエンジン（全体の管理）
	std::vector<std::shared_ptr<Track::MusicData>>     m_SoundEffects;	//再生するサウンド一つの元データ
	std::list<std::unique_ptr<DirectX::SoundEffectInstance>>     m_Instances;	//サウンドの再生インスタンス（ならすたびに生成）

	void AudioSet(Track::Music music);
	std::wstring Conv(Track::Music music);

	bool m_canPlaySE = false;

	// 持ち主
	GameObject& m_owner;
};

class TitleSoundComponent : public SoundComponent
{
public:
	// 初期化の時にInputComponentの初期化を呼ぶ
	TitleSoundComponent(GameObject& owner) : SoundComponent(owner) {}

	virtual void Desirealise() override;
};

