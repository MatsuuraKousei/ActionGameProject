#include "ActionGameProcess.h"
#include "../Scene.h"

void ActionGameProcess::Update()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		// このやり方はダメ！！！キャラクタリストを回している最中にリストをリセットしようとしている
		//Scene::GetInstance().LoadScene("Data/Scene/ActionGame.json");
		m_sceneChanger = true;
		if (m_sceneChanger)
		{
			// シーンの切り替え
			Scene::GetInstance().RequestChangeScene("Data/Scene/ShootingGame.json");
		}
	}
	else
	{
		m_sceneChanger = false;
	}
}
