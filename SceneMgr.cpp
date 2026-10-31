#include "stdafx.h"
#include "SceneMgr.h"

SceneMgr::SceneMgr()
	:arrScene()
	, currScene(nullptr)
{
}

SceneMgr::~SceneMgr()
{
}

void SceneMgr::Init(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	//Scene 생성
	// arrScene[(UINT)SCENE_TYPE::TEST] = new TestScene;
	// arrScene[(UINT)SCENE_TYPE::TEST]->SetName(L"Test Scene");

	//현재 씬 지정
	//currScene = arrScene[(UINT)SCENE_TYPE::TEST];
	//currScene->Enter(pd3dDevice, pd3dCommandList);
}

void SceneMgr::Update()
{
	// currScene->Update();
}

void SceneMgr::Render(ID3D12GraphicsCommandList* CommandList)
{
	//currScene->Render(pd3dCommandList);
}