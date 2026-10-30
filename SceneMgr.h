#pragma once

class Scene;

class SceneMgr
{
private:
	Scene* arrScene[(UINT)SCENE_TYPE::END];
	Scene* currScene;

public:
	SceneMgr();
	~SceneMgr();

	void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Update();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	Scene* GetCurScene() { return currScene; }
};

