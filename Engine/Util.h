#pragma once
#include "pch.h"
#include "Engine.h"
#include "DescriptorHeap.h"

class Util
{
public:
    template<typename T>
    static void CreateConstBuffer(Microsoft::WRL::ComPtr<ID3D12Device>& device, const T& cpuData, Microsoft::WRL::ComPtr<ID3D12Resource>& gpuBuffer, int count)
    {
        UINT elementByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(T));
        auto buffer = CD3DX12_RESOURCE_DESC::Buffer(elementByteSize * count);
        device->CreateCommittedResource(
            &m_uploadType,
            D3D12_HEAP_FLAG_NONE,
            &buffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&gpuBuffer)
        );

    }

    template<typename T>
    static void UpdateConstBuffer(const T& cpuData, Microsoft::WRL::ComPtr<ID3D12Resource>& gpuBuffer)
    {
        BYTE* MappedData = nullptr;
        gpuBuffer->Map(0, nullptr, reinterpret_cast<void**>(&MappedData));

        memcpy(MappedData, &cpuData, sizeof(T));

        if (gpuBuffer != nullptr)
            gpuBuffer->Unmap(0, nullptr);
        MappedData = nullptr;
    }

    static void CreateResource(ComPtr<ID3D12Resource> buffer[], int num)
    {
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R16G16B16A16_FLOAT, g_engine->m_width, g_engine->m_height);
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

        D3D12_CLEAR_VALUE optimizedClearValue = {};
        D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

        //if (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
        //{
        //    resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE;
        //    optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
        //}
        //else if (resFlags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
        //{
            resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
            float arrFloat[4] = { 0.f, 0.f, 0.f, 0.f };
            optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R16G16B16A16_FLOAT, arrFloat);
        //}

         D3D12_HEAP_PROPERTIES heapProperty= CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
         D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;


        // Create Texture2D
         for (int i = 0; i < num; ++i)
         {
             HRESULT hr = DEVICE->CreateCommittedResource(
                 &heapProperty,
                 heapFlags,
                 &desc,
                 resourceStates,
                 &optimizedClearValue,
                 IID_PPV_ARGS(&buffer[i]));

             assert(SUCCEEDED(hr));
         }

    }

    static void CreateResource(ComPtr<ID3D12Resource> buffer, int flag = 0)
    {
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R16G16B16A16_FLOAT, g_engine->m_width, g_engine->m_height);
        
        switch (flag)
        {
        case 0:
            desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            break;
        case 1:
            desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            break;
        }
        

        D3D12_CLEAR_VALUE optimizedClearValue = {};
        D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

        //if (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
        //{
        //    resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE;
        //    optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
        //}
        //else if (resFlags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
        //{
        resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
        float arrFloat[4] = { 0.f, 0.f, 0.f, 0.f };
        optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R16G16B16A16_FLOAT, arrFloat);
        //}

        D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;


        // Create Texture2D

            HRESULT hr = DEVICE->CreateCommittedResource(
                &heapProperty,
                heapFlags,
                &desc,
                resourceStates,
                &optimizedClearValue,
                IID_PPV_ARGS(&buffer));

            assert(SUCCEEDED(hr));
   

    }

    template <typename T>
    static void CreateUploadResource(ComPtr<ID3D12Resource>& buffer, vector<T>& data)
    {
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R16G16B16A16_FLOAT, g_engine->m_width, g_engine->m_height);
        desc.Width = sizeof(T) * data.size();


        D3D12_CLEAR_VALUE optimizedClearValue = {};
        D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

        //if (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
        //{
        //    resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE;
        //    optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
        //}
        //else if (resFlags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
        //{
        resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;
        float arrFloat[4] = { 0.f, 0.f, 0.f, 0.f };
        optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R16G16B16A16_FLOAT, arrFloat);
        //}

        D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;


        // Create Texture2D

        HRESULT hr = DEVICE->CreateCommittedResource(
            &heapProperty,
            heapFlags,
            &desc,
            resourceStates,
            &optimizedClearValue,
            IID_PPV_ARGS(&buffer));

        assert(SUCCEEDED(hr));

        void* pData = nullptr;
        D3D12_RANGE readRange = { 0, 0 }; 
        buffer->Map(0, &readRange, &pData);

        // 인스턴스 데이터 복사
        memcpy(pData, data.data(), sizeof(T) * data.size());

        // 맵핑 해제
        buffer->Unmap(0, nullptr);

    }

    static void CreateRTV(ComPtr<ID3D12Resource>& buffer, ComPtr<ID3D12DescriptorHeap>& rtvHeap)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.NumDescriptors = 1;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.NodeMask = 0;
        DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap));

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = rtvHeap->GetCPUDescriptorHandleForHeapStart();
        DEVICE->CreateRenderTargetView(buffer.Get(), nullptr, rtvHeapBegin);
    }

    static void CreateRTV(ComPtr<ID3D12Resource> buffer[], ComPtr<ID3D12DescriptorHeap>& rtvHeap, int num)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.NumDescriptors = num;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.NodeMask = 0;
        DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap));

        UINT rtvDescriptorSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

       
        for (int i = 0; i < num; ++i)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
            rtvHandle.ptr += i * rtvDescriptorSize;
            DEVICE->CreateRenderTargetView(buffer[i].Get(), nullptr, rtvHandle);

        }

    }

    static void CreateSRV(ComPtr<ID3D12Resource>& buffer, D3D12_CPU_DESCRIPTOR_HANDLE& handle)
    {

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        DEVICE->CreateShaderResourceView(buffer.Get(), &srvDesc, handle);
    }

    template <typename T>
    static void CreateSRV(ComPtr<ID3D12Resource>& buffer, D3D12_CPU_DESCRIPTOR_HANDLE& handle, vector<T> data)
    {

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Buffer.NumElements = max(data.size(), 1);
        srvDesc.Buffer.StructureByteStride = sizeof(T);

        DEVICE->CreateShaderResourceView(buffer.Get(), &srvDesc, handle);
    }

    static void CreateUAV(ComPtr<ID3D12Resource>& buffer, D3D12_CPU_DESCRIPTOR_HANDLE& handle, int count, int size)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = count;
        uavDesc.Buffer.StructureByteStride = size;
        uavDesc.Buffer.CounterOffsetInBytes = 0;
        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

        DEVICE->CreateUnorderedAccessView(buffer.Get(), nullptr, &uavDesc, handle );
    }

    //static void CreateSRV(ComPtr<ID3D12Resource> buffer[], ComPtr<ID3D12DescriptorHeap>& srvHeap, int num)
    //{
    //    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    //    srvHeapDesc.NumDescriptors = num;
    //    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    //    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    //    DEVICE->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));

    //    D3D12_CPU_DESCRIPTOR_HANDLE srvHeapBegin = srvHeap->GetCPUDescriptorHandleForHeapStart();

    //    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    //    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    //    srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    //    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    //    srvDesc.Texture2D.MipLevels = 1;

    //    UINT srvDescriptorSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    //   
    //    for (int i = 0; i < num; ++i)
    //    {
    //        int32 index = OBJ_HEAP->GetSRVIndex();
    //        D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = OBJ_HEAP->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart();
    //        //D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
    //        srvHandle.ptr += index * srvDescriptorSize;
    //        DEVICE->CreateShaderResourceView(buffer[i].Get(), &srvDesc, srvHandle);

    //    }

    //}

    //static void CreateRTVAndSRV(ComPtr<ID3D12Resource>& buffer, ComPtr<ID3D12DescriptorHeap>& rtvHeap,
    //    ComPtr<ID3D12DescriptorHeap>& srvHeap)
    //{
    //    CreateRTV(buffer, rtvHeap);
    //    //CreateSRV(buffer, srvHeap);
    //}

    //static void CreateRTVAndSRV(ComPtr<ID3D12Resource> buffer[], ComPtr<ID3D12DescriptorHeap>& rtvHeap,
    //    ComPtr<ID3D12DescriptorHeap>& srvHeap, int num)
    //{
    //    CreateRTV(buffer, rtvHeap, num);
    //    CreateSRV(buffer, srvHeap, num);
    //}

    static void ResourceStateTransition(ComPtr<ID3D12Resource>& Resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
    {
        auto toRenderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Resource.Get(),
            before,
            after);
        CMD_LIST->ResourceBarrier(
            1, &toRenderTargetBarrier
        );
    }

    static void ResourceStateTransition(ComPtr<ID3D12Resource> Resource[], D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, int num)
    {
        for (int i = 0; i < num; ++i)
        {
            auto toRenderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Resource[i].Get(),
                before,
                after);
            CMD_LIST->ResourceBarrier(
                1, &toRenderTargetBarrier
            );
        }

    }

    static Matrix CreateMatrix(Vector3 position, Vector3 scale = Vector3(1.f, 1.f, 1.f), Vector3 rotation = Vector3(0.f, 0.f, 0.f))
    {
        Matrix mat = Matrix::CreateScale(scale) * Matrix::CreateRotationX(rotation.x)
            * Matrix::CreateRotationY(rotation.y) * Matrix::CreateRotationZ(rotation.z)
            * Matrix::CreateTranslation(position);
        return mat.Transpose();
    }
};

