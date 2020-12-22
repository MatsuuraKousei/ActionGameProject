#include "GameObject.h"

class StageEdit :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;// 初期化

	virtual void Update()override;								  // 更新

private:

	void StageEditor();

	std::shared_ptr<GameObject> m_spMap;

	std::string					m_sMapName[4];
};