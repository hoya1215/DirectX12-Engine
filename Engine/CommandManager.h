#pragma once

enum class COMMANDLIST_TYPE
{
	MAIN,
	RESOURCE,
	SHADOW
};

struct COMMANDLIST_DESC
{
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;

	int m_renderOrder;
};

class CommandManager
{
public:
	CommandManager();

	void Create();
	void Reset();
	void SetViewPort(D3D12_VIEWPORT& viewPort, RECT& rect);
	//void SetDescriptorHeaps(ID3D12DescriptorHeap* heaps[]);
	void Clear();
	void Close();
	void ExecuteCommandLists();

	ComPtr<ID3D12GraphicsCommandList>& GetCmdList(COMMANDLIST_TYPE cmdListType) 
	{
		return m_commandLists[cmdListType]->m_commandList;
	}

	ComPtr<ID3D12CommandAllocator>& GetCmdAlloc(COMMANDLIST_TYPE cmdListType)
	{
		return m_commandLists[cmdListType]->m_commandAllocator;
	}

	ComPtr<ID3D12GraphicsCommandList>& GetResourceCmdList() { return m_resourceCmdList; }
	ComPtr<ID3D12CommandAllocator>& GetResourceCmdAlloc() { return m_resourceCmdAllocator; }

	ComPtr<ID3D12CommandQueue>& GetCmdQueue() { return m_commandQueue; }

private:
	unordered_map<COMMANDLIST_TYPE, shared_ptr<COMMANDLIST_DESC>> m_commandLists;



	// Resource
	ComPtr<ID3D12GraphicsCommandList> m_resourceCmdList;
	ComPtr<ID3D12CommandAllocator> m_resourceCmdAllocator;

	ComPtr<ID3D12CommandQueue> m_commandQueue;


};

