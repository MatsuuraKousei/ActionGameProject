#include "../../System/System.h"

#include "AudioComponent.h"

std::shared_ptr<SoundInstance> SoundInstance::Deserialize(Track::Music music)
{
	std::shared_ptr<SoundEffect> haveSound = std::make_shared<SoundEffect>();
	bool b3D;
	haveSound->Load(Conv(music,b3D));
	std::shared_ptr<SoundInstance> a= haveSound->CreateInstance(b3D);
	return a;

}

std::string SoundInstance::Conv(Track::Music music,bool &b3D)
{
	// BGM群////////////////////////////////////////////////////
	// タイトル
	if (music == Track::Title_BGM)
	{
		b3D = false;
		return std::string("Data/Sounds/BGM/MainBGM2.wav");
	}
	// ゲーム
	if (music == Track::Game_BGM)
	{
		b3D = false;
		return std::string("Data/Sounds/BGM/MainBGM.wav");
	}
	// クリア
	if (music == Track::Game_Clear)
	{
		b3D = false;
		return std::string("Data/Sounds/BGM/GameClear.wav");
	}
	// オーバー
	if (music == Track::Game_Over)
	{
		b3D = false;
		return std::string("Data/Sounds/BGM/GameOver.wav");
	}
	///////////////////////////////////////////////////////////

	// SE群////////////////////////////////////////////////////
	if (music == Track::ItemGet)
	{
		b3D = false;
		return std::string("Data/Sounds/SE/ItemGet.wav");
	}
	if (music == Track::Get)
	{
		b3D = false;
		return std::string("Data/Sounds/SE/Get.wav");
	}
	if (music == Track::Boar)
	{
		b3D = false;
		return std::string("Data/Sounds/SE/Boar.wav");
	}
	if (music == Track::Bat)
	{
		b3D = false;
		return std::string("Data/Sounds/SE/Bat.wav");
	}
	if (music == Track::Bullet)
	{
		b3D = false;
		return std::string("Data/Sounds/SE/Bullet.wav");
	}
	if (music == Track::Jump)
	{
		b3D = false;
		return std::string("Data/Sounds/SE/Jump.wav");
	}
	if (music == Track::Door1)
	{
		b3D = false;
		return std::string("Data/Sounds/SE/Door1.wav");
	}
	if (music == Track::Needle)
	{
		b3D = false;
		return std::string("Data/Sounds/SE/Needle.wav");
	}
	if (music == Track::Bow)
	{
		b3D = false;
		return std::string("Data/Sounds/SE/Bow.wav");
	}
	if (music == Track::System)
	{
		b3D = false;
		return std::string("Data/Sounds/SE/System.wav");
	}
	if (music == Track::Hit1)
	{
		b3D = false;
		return std::string("Data/Sounds/SE/Hit.wav");
	}
	if (music == Track::Hit2)
	{
		b3D = false;
		return std::string("Data/Sounds/SE/Hit2.wav");
	}
	if (music == Track::Hit3)
	{
		b3D = false;
		return std::string("Data/Sounds/SE/Hit3.wav");
	}
	///////////////////////////////////////////////////////////

	return nullptr;
}

void SoundInstance::Play(bool loop, bool manage)
{
	if (m_instance == nullptr)return;

	// 設定
	m_instance->Stop();
	//m_Inst->SetVolume(1);

	// 再生
	m_instance->Play(loop);

	// 再生リストへ追加
	if (manage)
	{
		AUDIO.AddPlayList(shared_from_this());
	}
}

// 3D Sound Ver
void SoundInstance::Play3D(const Math::Vector3& pos, bool loop, bool manage)
{
	if (m_instance == nullptr)return;

	// 設定
	m_instance->Stop();
	m_instance->SetVolume(1);

	// 再生
	m_instance->Play(loop);

	// 座標設定
	m_emitter.SetPosition(pos);
		//m_emitter.CurveDistanceScaler = 0.5;

	Apply3D();


	// 再生リストへ追加
	if (manage)
	{
		AUDIO.AddPlayList(shared_from_this());
	}
}

void SoundInstance::Apply3D()
{
	if (m_instance == nullptr)return;

	m_instance->Apply3D(AUDIO.GetListener(), m_emitter, false);
}

//void SoundInstance::SetVolume(float vol)
//{
//	if (m_instance == nullptr)return;
//
//	m_instance->SetVolume(vol);
//}
//
void SoundInstance::SetPos(const Math::Vector3& pos)
{
	if (m_instance == nullptr)return;

	// 座標設定
	m_emitter.SetPosition(pos);

	m_instance->Apply3D(AUDIO.GetListener(), m_emitter, false);
}

void SoundInstance::SetCurveDistanceScaler(float val)
{
	if (m_instance == nullptr)return;

	m_emitter.CurveDistanceScaler = val;

	m_instance->Apply3D(AUDIO.GetListener(), m_emitter, false);
}

bool SoundInstance::IsPlay()
{
	if (m_instance == nullptr)return false;

	if (m_instance->GetState() == DirectX::SoundState::PLAYING)return true;

	return false;
}

//bool KdSoundInstance::IsPause()
//{
//	if (m_instance == nullptr)return false;
//
//	if (m_instance->GetState() == DirectX::SoundState::PAUSED)return true;
//
//	return false;
//}
//
SoundInstance::~SoundInstance()
{
	Stop();
	m_instance = nullptr;
	m_srcData = nullptr;
}

bool SoundEffect::Load(const std::string& fileName)
{
	if (!AUDIO.IsInit())return false;


		// wstringに変換
		std::wstring wFilename = sjis_to_wide(fileName);
		AUDIO.GetAudioEngine().get();
		// 読み込み

		
		m_soundEffect = std::make_unique<DirectX::SoundEffect>(AUDIO.GetAudioEngine().get(), wFilename.c_str());
	

		if(!m_soundEffect)
		return false;
	

	// ファイル名記憶
	m_fileName = fileName;

	return true;
}

std::shared_ptr<SoundInstance> SoundEffect::CreateInstance(bool b3D)
{
	if (AUDIO.IsInit() == false)return nullptr;
	if (m_soundEffect == nullptr)return nullptr;

	std::shared_ptr<SoundInstance> inst = std::shared_ptr<SoundInstance>(new SoundInstance());

	DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Default;
	if (b3D) {
		flags = flags | DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters;
	}

	inst->Init(m_soundEffect->CreateInstance(flags), shared_from_this(), b3D);

	return inst;
}

//
////==================================================================
////
////
//// AudioManager
////
////
////==================================================================
//
bool AudioManager::Init()
{
	Release();

	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;
#ifdef _DEBUG
	eflags = eflags | DirectX::AudioEngine_Debug;
#endif

	m_audioEng = std::make_unique<DirectX::AudioEngine>(eflags);
	m_audioEng->SetReverb(DirectX::Reverb_Default);

	m_listener.OrientFront = { 0, 0, 1 };

	return true;
}


void AudioManager::Release()
{
	m_playList.clear();

	m_audioEng = nullptr;

}

void AudioManager::StopAll()
{
	if (m_audioEng == nullptr)return;

	// 再生リスト
	{
		auto it = m_playList.begin();
		while (it != m_playList.end()) {
			(*it).second->Stop();
			++it;
		}
	}

}

void AudioManager::Update()
{
	if (m_audioEng == nullptr)return;

	if (!m_audioEng->Update()) {
		// No audio device is active
		if (m_audioEng->IsCriticalError())
		{

		}
	}

	// 再生リスト処理
	{
		auto it = m_playList.begin();
		while (it != m_playList.end())
		{
			// 再生中でないときは、リストから削除(どこからも参照されていなければ解放される)
			if (!(*it).second->IsPlay())
			{
				it = m_playList.erase(it);
				continue;
			}

			++it;
		}
	}

}

void AudioManager::UpdatePos(Vector3 pos)
{
	m_listener.Position = pos;
}

void AudioManager::UpdateAngle(Vector3 angle)
{
	m_listener.OrientFront = angle;

}
