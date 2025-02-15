#pragma once
#include "Core/Vector/Vector3.h"
#include "Core/Platform/PlatformCore.h"
#include "Render/DirectX/Heap/DescriptorHeapHandle.h"

class Texture
{
public:
	Texture();
	~Texture();

	void SetDescriptorHeapHandle(DescriptorHeapHandle handle) { mDescriptorHandle = handle; }
	DescriptorHeapHandle GetDescriptorHeapHandle() { return mDescriptorHandle; }

	void SetTextureResource(ID3D12Resource *resource) { mTextureResource = resource; }
	ID3D12Resource *GetTextureResource() { return mTextureResource; }

	void SetFormat(DXGI_FORMAT format) { mFormat = format; }
	DXGI_FORMAT GetFormat() { return mFormat; }
	void SetDimensions(Vector3 dimensions) { mDimensions = dimensions; }
	Vector3 GetDimensions() { return mDimensions; }
	void SetMipCount(uint32 mipCount) { mMipCount = mipCount; }
	uint32 GetMipCount() { return mMipCount; }
	void SetArraySize(uint32 arraySize) { mArraySize = arraySize; }
	uint32 GetArraySize() { return mArraySize; }
	void SetIsCubeMap(bool isCubeMap) { mIsCubeMap = isCubeMap; }
	bool GetIsCubeMap() { return mIsCubeMap; }

private:
	ID3D12Resource* mTextureResource;
	DescriptorHeapHandle mDescriptorHandle;

	DXGI_FORMAT mFormat;
	Vector3		mDimensions;
	uint32		mMipCount;
	uint32		mArraySize;
	bool		mIsCubeMap;
};