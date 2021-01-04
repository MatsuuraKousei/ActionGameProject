#include "SoundComponent.h"


SoundComponent::SoundComponent(GameObject& owner) : m_owner(owner)
{

}



void SoundComponent::Desirealise() {

	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;

	m_audioEng = std::make_unique<DirectX::AudioEngine>(eflags);
	m_audioEng->SetReverb(DirectX::Reverb_Default);
}

void SoundComponent::Update()
{
	if (m_audioEng == nullptr)
	{
		m_audioEng->Update();
	}

	for (auto itr = m_Instances.begin(); itr != m_Instances.end();)
	{
		if (itr->get()->GetState() != DirectX::SoundState::PLAYING)
		{
			itr = m_Instances.erase(itr);
		}
		itr++;
	}

}

void SoundComponent::AudioSet(Track::Music music)
{
	std::shared_ptr<Track::MusicData>  m_spMusic=std::make_shared<Track::MusicData>();

	m_spMusic->music = music;

	m_spMusic->name=Conv(music);

	m_spMusic->m_SoundEffect = std::make_unique<DirectX::SoundEffect>(m_audioEng.get(), m_spMusic->name.c_str());

	//曲セット
	if (m_audioEng != nullptr)
	{
		try
		{
			m_SoundEffects.push_back(m_spMusic);
		}
		catch (...)
		{
			assert(0 && "Sound File Load Error");
		}
	}
}

std::wstring SoundComponent::Conv(Track::Music music)
{
	if (music == Track::TitleBGM)
	{
		return sjis_to_wide("Data/Sounds/BGM/Castle.wav");
	}
	if (music == Track::GameBGM)
	{
		return sjis_to_wide("Data/Sounds/BGM/Castle.wav");
	}
	if (music == Track::BossBGM)
	{

	}
	if (music == Track::GameOverBGM)
	{

	}
	if (music == Track::ItemGet)
	{
		return sjis_to_wide("Data/Sounds/SE/ItemGet.wav");
	}

	return nullptr;
}

void SoundComponent::Play(Track::Music music)
{
	for (auto pObject : m_SoundEffects)
	{
		if (pObject->music == music)
		{
			//再生オプション
			DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Default;		
			auto instance = (pObject->m_SoundEffect->CreateInstance(flags));
			if (instance)
			{
				instance->Play();
			}
			m_Instances.push_back(std::move(instance));

			m_canPlaySE = false;
			break;
		}

	}
}
;

void TitleSoundComponent::Desirealise()
{
	SoundComponent::Desirealise();

	AudioSet(Track::TitleBGM);

	Play(Track::TitleBGM);

}
