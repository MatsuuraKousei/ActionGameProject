#include "StageEdit.h"
#include "Scene.h"

void StageEdit::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	for (int i = 0; i < 4; i++)
	{
		std::string string = "MapName";
		string += std::to_string(i);
		if (jsonObj[string].is_string())
		{
			m_sMapName[i] = jsonObj[string].string_value();
		}

		m_spMap = std::make_shared<GameObject>();

		m_spMap->Deserialize(ResFac.GetJSON(m_sMapName[i]));

		m_spMap->SetTag(TAG_StageObject);

		Scene::GetInstance().AddObject(m_spMap);
	}
}

void StageEdit::Update()
{
	StageEditor();
	Destroy();
}

void StageEdit::StageEditor()
{
	for (int i = 0; i < 4; i++)
	{
		
	}
}
