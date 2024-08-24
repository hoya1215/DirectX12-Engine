#pragma once

// 물체 SRV 와 RTT 로 사용할 SRV 같은 Heap 에 담아놓음
// SRV 시작 핸들만 가져오면 자동으로 개수만큼 바인딩
// RTV 는 새로운 Heap
// SRV 다른 Heap 사용하려면 SetDescriptorHeap 따로 -> 지금 오류 , 이유 찾기

// 자원 상태 전이 설정 안해줘도 잘 나오긴 한다

// ------ Root Signature , Heap 관련
// 일반 물체의 Root Signature 와 Descriptor Heap을
// 후처리 Compute Root Signature 와 SRV Descriptor Heap 랑 분리






// ------ Shader 관련
// Control Patch 로 정점 보내주면 Index Buffer 필요 없으므로
// 그냥 DrawInstanced 호출

// 파이프라인의 primitive 와 물체 렌더 primitive 세팅 맞춰주는거 주의 

// Normal Map -> GS 빼고 적용



// SRV
// PostProcess 5 : ShadowMap 
// 6 : Combine
// 7 : Filter



// Multi Thread
// Resource Command List 잘 설정 -> 안해주면 meshbuffer nullptr 오류
// CommandQueue 하나만 Direct 로 / Compute 는 Draw 안됨
// 
// Single -> CommandManager 의 list , queue 이용
// 
// Multi -> threadManager 의 list , queue , thread 이용
// 
// Resource 용은 CommandManager list 이용
