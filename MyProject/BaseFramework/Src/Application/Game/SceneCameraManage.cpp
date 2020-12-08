#include"SceneCameraManage.h"
#include"Scene.h"
#include"SceneManage.h"
#include "../Component/CameraComponent.h"
#include "../Component/InputComponent.h"


void SceneCameraManage::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);
	switch (Scene::GetInstance().stageProcess)
	{
	case OPNING:
		if (!m_spCameraComponent) { return; }
		m_CameraTrans.x = 0.0f;
		m_CameraTrans.y = 0.0f;
		m_CameraTrans.z = -150.0f;
		m_spCameraComponent->OffsetMatrix().CreateTranslation(m_CameraTrans);
		m_spCameraComponent->OffsetMatrix().RotateX(0.5);

		Scene::GetInstance().SetTargetCamera(m_spCameraComponent);
		break;
	case FIELD:
		if (!m_spCameraComponent) { return; }
		break;
	case CLEAR:
		if (!m_spCameraComponent) { return; }
		m_CameraTrans.x = 0.0f;
		m_CameraTrans.y = -8.0f;
		m_CameraTrans.z = -6.0f;
		m_spCameraComponent->OffsetMatrix().CreateTranslation(m_CameraTrans);
		m_spCameraComponent->OffsetMatrix().RotateX(0.1);

		Scene::GetInstance().SetTargetCamera(m_spCameraComponent);
		break;
	case OVER:
		if (!m_spCameraComponent) { return; }
		m_CameraTrans.x = -0.5f;
		m_CameraTrans.y = 23.0f;
		m_CameraTrans.z = -22.0f;
		m_spCameraComponent->OffsetMatrix().CreateTranslation(m_CameraTrans);
		m_spCameraComponent->OffsetMatrix().RotateX(0.5);
		m_spCameraComponent->OffsetMatrix().RotateY(1.6);

		Scene::GetInstance().SetTargetCamera(m_spCameraComponent);
		break;
	}
}

void SceneCameraManage::Update()
{
	//Imguiコンポーネントの更新
	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}
	Scene::GetInstance().EditorCameraEnable = false;

	Matrix OpningMat;
	OpningMat.SetTranslation(Vector3(0, 0, 0));

	Matrix ClearMat;
	ClearMat.SetTranslation(Vector3(0, 0, 0));

	Matrix OverMat;
	OverMat.SetTranslation(Vector3(0, 10, 0));

	float f = 0.01;

	switch (Scene::GetInstance().stageProcess)
	{
	case OPNING:
		m_spCameraComponent->SetCameraMatrix(OpningMat);
		m_spCameraComponent->OffsetMatrix().RotateY(f);

		//カメラコンポーネントの更新
		if (m_spCameraComponent)
		{
			Matrix trans;
			trans.CreateTranslation(m_mWorld.GetTranslation());
			m_spCameraComponent->SetCameraMatrix(trans);
		}
		break;
	case FIELD:
		break;
	case CLEAR:
		m_spCameraComponent->SetCameraMatrix(ClearMat);

		//カメラコンポーネントの更新
		if (m_spCameraComponent)
		{
			Matrix trans;
			trans.CreateTranslation(m_mWorld.GetTranslation());
			m_spCameraComponent->SetCameraMatrix(trans);
		}
		break;
	case OVER:
		m_spCameraComponent->SetCameraMatrix(OverMat);
		
		//カメラコンポーネントの更新
		if (m_spCameraComponent)
		{
			Matrix trans;
			trans.CreateTranslation(m_mWorld.GetTranslation());
			m_spCameraComponent->SetCameraMatrix(trans);
		}
		break;
	}


}
