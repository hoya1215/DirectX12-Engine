#pragma once

// 이미지, 일반 텍스처 버퍼
class Texture
{
public:
	void CreateTexture(const wstring& path, ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& resourceCmdList);
	void CreateTexture(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& resourceCmdList);
	void CreateView(ComPtr<ID3D12Device>& device);

	//ScratchImage GetImage() { return m_image; }
	ComPtr<ID3D12Resource> GetTextureBuffer() { return m_textureBuffer; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return m_srvHandle; }
	ScratchImage m_image;

private:
	
	ComPtr<ID3D12Resource> m_textureBuffer;

	ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_srvHandle;
};

