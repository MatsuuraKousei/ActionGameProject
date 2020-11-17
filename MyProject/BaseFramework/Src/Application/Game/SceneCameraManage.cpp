#include"SceneCameraManage.h"
#include "../Component/CameraComponent.h"
#include "../Component/InputComponent.h"


void SceneCameraManage::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);
	if (!m_spCameraComponent) { return; }
	m_CameraTrans.x = 0.0f;
	m_CameraTrans.y = 0.0f;
	m_CameraTrans.z = -150.0f;
	m_spCameraComponent->OffsetMatrix().CreateTranslation(m_CameraTrans);
	m_spCameraComponent->OffsetMatrix().RotateX(0.5);

	Scene::GetInstance().SetTargetCamera(m_spCameraComponent);
}

void SceneCameraManage::Update()
{
	//Imguiコンポーネントの更新
	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}
	Scene::GetInstance().EditorCameraEnable = false;

	Matrix m;
	m.SetTranslation(Vector3(0, 0, 0));

	m_spCameraComponent->SetCameraMatrix(m);

	float f = 0.01;

	m_spCameraComponent->OffsetMatrix().RotateY(f);

	//カメラコンポーネントの更新
	if (m_spCameraComponent)
	{
		Matrix trans;
		trans.CreateTranslation(m_mWorld.GetTranslation());
		m_spCameraComponent->SetCameraMatrix(trans);
	}
	
}
