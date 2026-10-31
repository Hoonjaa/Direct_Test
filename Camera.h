#pragma once
#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

class Camera
{
protected:
	XMFLOAT4X4 ViewMatrix;
	XMFLOAT4X4 ProjectionMatrix;

	D3D12_VIEWPORT Viewport;
	D3D12_RECT ScissorRect;

public:
	Camera();
	~Camera();

	void CreateShaderVariables(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	void ReleaseShaderVariables();
	void UpdateShaderVariables(ID3D12GraphicsCommandList* CommandList);

	void GenerateLookAtViewMatrix(XMFLOAT3 Position, XMFLOAT3 LookAt, XMFLOAT3 Up);
	void GenerateLookToViewMatrix(XMFLOAT3 Position, XMFLOAT3 LookTo, XMFLOAT3 Up);
	void GenerateProjectionMatrix(float NearPlaneDistance, float FarPlaneDistance, float AspectRatio, float FOVAngle);
	void SetViewport(int xTopLeft, int yTopLeft, int Width, int Height, float MinZ = 0.0f, float MaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* CommandList);
};

