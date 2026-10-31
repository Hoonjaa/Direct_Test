#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
{
	ViewMatrix = Matrix4x4::Identity();
	ProjectionMatrix = Matrix4x4::Identity();
	Viewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	ScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
}

Camera::~Camera()
{
}

void Camera::CreateShaderVariables(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList) 
{
// 추후 상수 버퍼 연결용
}

void Camera::ReleaseShaderVariables() 
{
// 추후 상수 버퍼 해제용
}

void Camera::UpdateShaderVariables(ID3D12GraphicsCommandList* CommandList)
{
	XMFLOAT4X4 View;
	XMStoreFloat4x4(&View, XMMatrixTranspose(XMLoadFloat4x4(&ViewMatrix)));
	CommandList->SetGraphicsRoot32BitConstants(1, 16, &View, 0);

	XMFLOAT4X4 Projection;
	XMStoreFloat4x4(&Projection,XMMatrixTranspose(XMLoadFloat4x4(&ProjectionMatrix)));
	CommandList->SetGraphicsRoot32BitConstants(1, 16, &Projection, 16);
}

void Camera::GenerateLookAtViewMatrix(XMFLOAT3 Position, XMFLOAT3 LookAt, XMFLOAT3 Up)
{
	ViewMatrix = Matrix4x4::LookAtLH(Position, LookAt, Up);
}

void Camera::GenerateLookToViewMatrix(XMFLOAT3 Position, XMFLOAT3 LookTo, XMFLOAT3 Up)
{
	ViewMatrix = Matrix4x4::LookToLH(Position, LookTo, Up);
}

void Camera::GenerateProjectionMatrix(float NearPlaneDistance, float FarPlaneDistance, float AspectRatio, float FOVAngle)
{
	ProjectionMatrix = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(FOVAngle), AspectRatio, NearPlaneDistance, FarPlaneDistance);
}

void Camera::SetViewport(int xTopLeft, int yTopLeft, int Width, int Height, float MinZ, float MaxZ) {
	Viewport.TopLeftX = float(xTopLeft);
	Viewport.TopLeftY = float(yTopLeft);
	Viewport.Width = float(Width);
	Viewport.Height = float(Height);
	Viewport.MinDepth = MinZ;
	Viewport.MaxDepth = MaxZ;
}

void Camera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	ScissorRect.left = xLeft;
	ScissorRect.top = yTop;
	ScissorRect.right = xRight;
	ScissorRect.bottom = yBottom;
}

void Camera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* CommandList)
{
	CommandList->RSSetViewports(1, &Viewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);
}