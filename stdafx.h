// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>	// XMCOLOR 처럼 데이터를 줄여주는 자료형들이 들어있음
#include <DirectXColors.h>			// 그냥 대표색 저장되어있음. 없어도 됨
#include <DirectXCollision.h>

#include <dxgidebug.h>

#include "define.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")

namespace Vector3
{
	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& Vector)
	{
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, Vector);
		return(Result);
	}
	inline XMFLOAT3 ScalarProduct(XMFLOAT3& Vector, float Scalar, bool Normalize = true)
	{
		XMFLOAT3 Result;
		if (Normalize)
			XMStoreFloat3(&Result, XMVector3Normalize(XMLoadFloat3(&Vector)) * Scalar);
		else
			XMStoreFloat3(&Result, XMLoadFloat3(&Vector) * Scalar);
		return(Result);
	}
	inline XMFLOAT3 Add(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2)
	{
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, XMLoadFloat3(&Vector1) + XMLoadFloat3(&Vector2));
		return(Result);
	}
	inline XMFLOAT3 Add(XMFLOAT3& Vector1, XMFLOAT3& Vector2, float Scalar)
	{
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, XMLoadFloat3(&Vector1) + (XMLoadFloat3(&Vector2) * Scalar));
		return(Result);
	}
	inline XMFLOAT3 Subtract(XMFLOAT3& Vector1, XMFLOAT3& Vector2)
	{
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, XMLoadFloat3(&Vector1) - XMLoadFloat3(&Vector2));
		return(Result);
	}
	inline float DotProduct(XMFLOAT3& Vector1, XMFLOAT3& Vector2)
	{
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, XMVector3Dot(XMLoadFloat3(&Vector1), XMLoadFloat3(&Vector2)));
		return(Result.x);
	}
	inline XMFLOAT3 CrossProduct(XMFLOAT3& Vector1, XMFLOAT3& Vector2, bool Normalize = true)
	{
		XMFLOAT3 Result;
		if (Normalize)
			XMStoreFloat3(&Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&Vector1), XMLoadFloat3(&Vector2))));
		else
			XMStoreFloat3(&Result, XMVector3Cross(XMLoadFloat3(&Vector1), XMLoadFloat3(&Vector2)));
		return(Result);
	}
	inline XMFLOAT3 Normalize(XMFLOAT3& Vector)
	{
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, XMVector3Normalize(XMLoadFloat3(&Vector)));
		return(Result);
	}
	inline float Length(XMFLOAT3& Vector)
	{
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, XMVector3Length(XMLoadFloat3(&Vector)));
		return(Result.x);
	}
	inline float Angle(const XMVECTOR& Vector1, const XMVECTOR& Vector2)
	{
		XMVECTOR Angle = XMVector3AngleBetweenNormals(Vector1, Vector2);
		return(XMConvertToDegrees(XMVectorGetX(Angle)));
	}
	inline float Angle(XMFLOAT3& Vector1, XMFLOAT3& Vector2)
	{
		return(Angle(XMLoadFloat3(&Vector1), XMLoadFloat3(&Vector2)));
	}
	inline XMFLOAT3 TransformNormal(XMFLOAT3& Vector, const XMMATRIX& Transform)
	{
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, XMVector3TransformNormal(XMLoadFloat3(&Vector), Transform));
		return(Result);
	}
	inline XMFLOAT3 TransformCoord(XMFLOAT3& Vector, const XMMATRIX& Transform)
	{
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, XMVector3TransformCoord(XMLoadFloat3(&Vector),Transform));
		return(Result);
	}
	inline XMFLOAT3 TransformCoord(XMFLOAT3& Vector, XMFLOAT4X4& Matrix)
	{
		return(TransformCoord(Vector, XMLoadFloat4x4(&Matrix)));
	}
}

namespace Vector4
{
	inline XMFLOAT4 Add(XMFLOAT4& Vector1, XMFLOAT4& Vector2)
	{
		XMFLOAT4 Result;
		XMStoreFloat4(&Result, XMLoadFloat4(&Vector1) + XMLoadFloat4(&Vector2));
		return(Result);
	}
	inline XMFLOAT4 Multiply(XMFLOAT4& Vector1, XMFLOAT4& Vector2)
	{
		XMFLOAT4 Result;
		XMStoreFloat4(&Result, XMLoadFloat4(&Vector1) * XMLoadFloat4(&Vector2));
		return(Result);
	}
	inline XMFLOAT4 Multiply(float Scalar, XMFLOAT4& Vector)
	{
		XMFLOAT4 Result;
		XMStoreFloat4(&Result, Scalar * XMLoadFloat4(&Vector));
		return(Result);
	}
}

namespace Martix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 Result;
		XMStoreFloat4x4(&Result, XMMatrixIdentity());
		return(Result);
	}
	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& Matrix1, XMFLOAT4X4& Matrix2)
	{
		XMFLOAT4X4 Result;
		XMStoreFloat4x4(&Result, XMLoadFloat4x4(&Matrix1) * XMLoadFloat4x4(&Matrix2));
		return(Result);
	}
	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& Matrix1, XMMATRIX& Matrix2)
	{
		XMFLOAT4X4 Result;
		XMStoreFloat4x4(&Result, XMLoadFloat4x4(&Matrix1) * Matrix2);
		return(Result);
	}
	inline XMFLOAT4X4 Multiply(XMMATRIX& Matrix1, XMFLOAT4X4& Matrix2)
	{
		XMFLOAT4X4 Result;
		XMStoreFloat4x4(&Result, Matrix1 * XMLoadFloat4x4(&Matrix2));
		return(Result);
	}
	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& Matrix)
	{
		XMFLOAT4X4 Result;
		XMStoreFloat4x4(&Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&Matrix)));
		return(Result);
	}
	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& Matrix)
	{
		XMFLOAT4X4 Result;
		XMStoreFloat4x4(&Result, XMMatrixTranspose(XMLoadFloat4x4(&Matrix)));
		return(Result);
	}
	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
	{
		XMFLOAT4X4 Result;
		XMStoreFloat4x4(&Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
		return(Result);
	}
	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& EyePosition, XMFLOAT3& LookAtPosition, XMFLOAT3& UpDirection)
	{
		XMFLOAT4X4 Result;
		XMStoreFloat4x4(&Result, XMMatrixLookAtLH(XMLoadFloat3(&EyePosition), XMLoadFloat3(&LookAtPosition), XMLoadFloat3(&UpDirection)));
		return(Result);
	}
	inline XMFLOAT4X4 LookToLH(XMFLOAT3& EyePosition, XMFLOAT3& LookToDirection, XMFLOAT3& UpDirection)
	{
		XMFLOAT4X4 Result;
		XMStoreFloat4x4(&Result, XMMatrixLookToLH(XMLoadFloat3(&EyePosition), XMLoadFloat3(&LookToDirection), XMLoadFloat3(&UpDirection)));
		return(Result);
	}
}