#pragma once
#include "pch.h"

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

    static Matrix CreateMatrix(Vector3 position, Vector3 scale = Vector3(1.f, 1.f, 1.f), Vector3 rotation = Vector3(0.f, 0.f, 0.f))
    {
        Matrix mat = Matrix::CreateScale(scale) * Matrix::CreateRotationX(rotation.x)
            * Matrix::CreateRotationY(rotation.y) * Matrix::CreateRotationZ(rotation.z)
            * Matrix::CreateTranslation(position);
        return mat.Transpose();
    }
};

