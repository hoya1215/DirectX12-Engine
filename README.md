## 2024 / 07 / 14

### 오브젝트 추가 , 카메라 추가 , 월드 행렬
---

## 2024 / 07 / 15

### 카메라 이동, 회전 , 뷰 프로젝션 행렬 추가
---

## 2024 / 07 / 16

### Behavior , Component 추가 


### Component 로 관리, 마우스로 화면 회전
---

## 2024 / 07 / 20

### 여러개의 물체를 위한 CBV , SRV Heap 분리 및 정리
---

## 2024 / 07 / 21

### 기존 PipeLine -> 분리 ObjectManager, GlobalData , Skybox 추가
---

## 2024 / 07 / 24

### Light 추가
---

## 2024 / 08 / 03

### MRT , RTT , Deferred Rendering 추가
---


## 2024 / 08 / 04

### Material Constant , 카메라 방향 업데이트 , Object Bounding Shape , Frustum Culling 추가
---

## 2024 / 08 / 07

### Instancing 추가
---

## 2024 / 08 / 09

### ComputeShader , UAV 추가 , 후처리 및 정보 저장용 텍스처 filter 추가
---

## 2024 / 08 / 10

### HullShader , DomainShader , Grid , LOD 추가



### Compute RootSignature , 후처리 SRV DescriptorHeap 분리
---

## 2024 / 08 / 11

### GeometryShader 추가 , ImGui 연동
---

## 2024 / 08 / 13

### Shadow Pass 추가 
---

## 2024 / 08 / 15

### 기존 : Deferred Render -> PostProcess
### 변경 후 : Deferred Render -> Combine ( Deferred 합쳐서 빛 , 그림자 계산 ) -> PostProcess ( 후처리 )
![DX12 중간](https://github.com/user-attachments/assets/b3cd6ea1-97f7-4055-8a19-919aa07bc4de)
---

## 2024 / 08 / 20

### CommandManager 에서 다중 CommandList , CommandQueue 관리 
### MultiThread 추가
---

## 2024 / 08 / 22

### Thread + CommandList 합치기 
### Single Thread -> CommandManager 사용 , Multi Thread -> ThreadManager 사용
---

## 2024 / 08 / 23

### Multi Thread Excute 여러번

![스레딩 표](https://github.com/user-attachments/assets/88796919-6bf3-4f46-973e-07e956d5ecd2)

![싱글스레딩](https://github.com/user-attachments/assets/7b90b632-866c-47bc-89e8-22c2e3568b0e)|![멀티쓰레딩](https://github.com/user-attachments/assets/4e5b8180-7df6-432f-8933-83b7021156b5)
---|---|
---

## 2024 / 08 / 24

### Normal Mapping 추가

---

## 2024 / 08 / 25

### PBR ( Deferred ) 추가
---

