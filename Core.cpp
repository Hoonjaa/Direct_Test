#include "stdafx.h"
#include "Core.h"

Core::Core()
{
	handle = NULL;
	hInstance = NULL;

	dxgiFactory = NULL;
	dxgiSwapChain = NULL;
	d3dDevice = NULL;

	d3dCommandAllocator = NULL;
	d3dCommandQueue = NULL;
	d3dPipelineState = NULL;
	d3dCommandList = NULL;

	for(int i = 0; i < SwapChainBuffers; i++)
		d3dRenderTargetBuffers[i] = NULL;
	d3dRtvDescriptorHeap = NULL;
	RtvDescriptorIncrementSize = 0;

	d3dDepthStencilBuffer = NULL;
	d3dDsvDescriptorHeap = NULL;
	DsvDescriptorIncrementSize = 0;

	SwapChainBufferIndex = 0;

	FenceEvent = NULL;
	d3dFence = NULL;
	for (int i = 0; i < SwapChainBuffers; i++)
		FenceValues[i] = 0;

	WndClientWidth = FRAME_BUFFER_WIDTH;
	WndClientHeight = FRAME_BUFFER_HEIGHT;

	_tcscpy_s(FrameRate, _T("FPS : "));
}

Core::~Core()
{
}

int Core::Init(HWND handle, HINSTANCE hInstance)
{
	this->handle = handle;
	this->hInstance = hInstance;
	
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateDepthStencilView();



	BuildObjects();
	return S_OK;
}

void Core::OnDestroy()
{
	WaitForGpuComplete();

	::CloseHandle(FenceEvent);

	for (int i = 0; i < SwapChainBuffers; i++)
		if (d3dRenderTargetBuffers[i]) d3dRenderTargetBuffers[i]->Release();
	if (d3dRtvDescriptorHeap) d3dRtvDescriptorHeap->Release();

	if (d3dDepthStencilBuffer) d3dDepthStencilBuffer->Release();
	if (d3dDsvDescriptorHeap) d3dDsvDescriptorHeap->Release();

	if (d3dCommandAllocator) d3dCommandAllocator->Release();
	if (d3dCommandQueue) d3dCommandQueue->Release();
	if (d3dPipelineState) d3dPipelineState->Release();
	if (d3dCommandList) d3dCommandList->Release();

	if (d3dFence) d3dFence->Release();

	dxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (dxgiSwapChain) dxgiSwapChain->Release();
	if (d3dDevice) d3dDevice->Release();
	if (dxgiFactory) dxgiFactory->Release();

#if defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void Core::Progress()
{
	


	HRESULT hResult = d3dCommandAllocator->Reset();
	hResult = d3dCommandList->Reset(d3dCommandAllocator, NULL);
	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = d3dRenderTargetBuffers[SwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	d3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = d3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (SwapChainBufferIndex * RtvDescriptorIncrementSize);
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = d3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, FALSE, &d3dDsvCPUDescriptorHandle);
	float pfClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	d3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor, 0, NULL);
	d3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	
	//Render

	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	d3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	hResult = d3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { d3dCommandList };
	d3dCommandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists),
		ppd3dCommandLists);
	WaitForGpuComplete();
	dxgiSwapChain->Present(0, 0);
	MoveToNextFrame();
	//m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(handle, FrameRate);
}

void Core::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(handle, &rcClient);
	WndClientWidth = rcClient.right - rcClient.left;
	WndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = SwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = WndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = WndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = handle;
	dxgiSwapChainDesc.SampleDesc.Count = (Msaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (Msaa4xEnable) ? (Msaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hResult = dxgiFactory->CreateSwapChain(d3dCommandQueue,
		&dxgiSwapChainDesc, (IDXGISwapChain**)&dxgiSwapChain);
	SwapChainBufferIndex = dxgiSwapChain->GetCurrentBackBufferIndex();

	hResult = dxgiFactory->MakeWindowAssociation(handle, DXGI_MWA_NO_ALT_ENTER);

#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();
#endif
}

void Core::CreateDirect3DDevice()
{
	HRESULT hResult;

	UINT nDXGIFactoryFlags = 0;
#if defined(_DEBUG)
	ID3D12Debug* pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)&dxgiFactory);

	IDXGIAdapter1* pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0,
			_uuidof(ID3D12Device), (void**)&d3dDevice))) break;
	}

	if (!pd3dAdapter)
	{
		dxgiFactory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), (void**)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0,
			_uuidof(ID3D12Device), (void**)&d3dDevice);
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	Msaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;

	Msaa4xEnable = (Msaa4xQualityLevels > 1) ? true : false;

	hResult = d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		__uuidof(ID3D12Fence), (void**)&d3dFence);

	FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	if (pd3dAdapter) pd3dAdapter->Release();
}

void Core::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = d3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void**)&d3dCommandQueue);

	hResult = d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&d3dCommandAllocator);

	hResult =d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&d3dCommandList);

	hResult = d3dCommandList->Close();
}

void Core::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = SwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = d3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&d3dRtvDescriptorHeap);

	RtvDescriptorIncrementSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = d3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&d3dDsvDescriptorHeap);

	DsvDescriptorIncrementSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void Core::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = d3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < SwapChainBuffers; i++)
	{
		dxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&d3dRenderTargetBuffers[i]);
		d3dDevice->CreateRenderTargetView(d3dRenderTargetBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += RtvDescriptorIncrementSize;
	}
}

void Core::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = WndClientWidth;
	d3dResourceDesc.Height = WndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (Msaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (Msaa4xEnable) ? (Msaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;
	d3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&d3dDepthStencilBuffer);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = d3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dDevice->CreateDepthStencilView(d3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
}

void Core::BuildObjects()
{
	d3dCommandList->Reset(d3dCommandAllocator, NULL);
	//씬 매니저 Init

	d3dCommandList->Close();
	ID3D12CommandList* d3dCommandLists[] = { d3dCommandList }; //리스트 지금은 하나라 굳이 배열로 안해도 된다
	d3dCommandQueue->ExecuteCommandLists(1, d3dCommandLists);

	WaitForGpuComplete();
}

void Core::WaitForGpuComplete()
{
	UINT64 nFenceValue = ++FenceValues[SwapChainBufferIndex];
	HRESULT hResult = d3dCommandQueue->Signal(d3dFence, nFenceValue);
	if (d3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = d3dFence->SetEventOnCompletion(nFenceValue, FenceEvent);
		::WaitForSingleObject(FenceEvent, INFINITE);
	}
}

void Core::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState = FALSE;
	dxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	dxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = WndClientWidth;
	dxgiTargetParameters.Height = WndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < SwapChainBuffers; i++) if (d3dRenderTargetBuffers[i])
		d3dRenderTargetBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	dxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	dxgiSwapChain->ResizeBuffers(SwapChainBuffers, WndClientWidth, WndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	SwapChainBufferIndex = dxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

void Core::MoveToNextFrame()
{
	SwapChainBufferIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
	UINT64 nFenceValue = ++FenceValues[SwapChainBufferIndex];
	HRESULT hResult = d3dCommandQueue->Signal(d3dFence, nFenceValue);
	if (d3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = d3dFence->SetEventOnCompletion(nFenceValue, FenceEvent);
		::WaitForSingleObject(FenceEvent, INFINITE);
	}
}