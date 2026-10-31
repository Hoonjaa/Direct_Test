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

	void Init(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	void Update();
	void Render(ID3D12GraphicsCommandList* CommandList);

	Scene* GetCurScene() { return currScene; }
};

