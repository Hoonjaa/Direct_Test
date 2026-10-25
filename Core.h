#pragma once
class Core
{
private:
	HINSTANCE hInstance;
	HWND handle;

	int WndClientWidth;
	int WndClientHeight;

	IDXGIFactory4* dxgiFactory;
	IDXGISwapChain3* dxgiSwapChain;
	ID3D12Device* d3dDevice;

	bool Msaa4xEnable = false;
	UINT Msaa4xQualityLevels = 0;

	static const UINT SwapChainBuffers = 2;
	UINT SwapChainBufferIndex;

	ID3D12Resource* d3dRenderTargetBuffers[SwapChainBuffers];
	ID3D12DescriptorHeap* d3dRtvDescriptorHeap;
	UINT RtvDescriptorIncrementSize;

	ID3D12Resource* d3dDepthStencilBuffer;
	ID3D12DescriptorHeap* d3dDsvDescriptorHeap;
	UINT DsvDescriptorIncrementSize;

	ID3D12CommandQueue* d3dCommandQueue;
	ID3D12CommandAllocator* d3dCommandAllocator;
	ID3D12GraphicsCommandList* d3dCommandList;

	ID3D12PipelineState* d3dPipelineState;

	ID3D12Fence* d3dFence;
	UINT64 FenceValues[SwapChainBuffers];
	HANDLE FenceEvent;

	_TCHAR FrameRate[50];

public:
	Core();
	~Core();

	int Init(HWND handle, HINSTANCE hInstance);
	void Progress();

	HWND GetMainHandle() { return handle; }

	void OnDestroy();

	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void BuildObjects();

	void WaitForGpuComplete();

	void ChangeSwapChainState();

	void MoveToNextFrame();
};

