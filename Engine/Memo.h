#pragma once

// 물체 SRV 와 RTT 로 사용할 SRV 같은 Heap 에 담아놓음
// SRV 시작 핸들만 가져오면 자동으로 개수만큼 바인딩
// RTV 는 새로운 Heap
// SRV 다른 Heap 사용하려면 SetDescriptorHeap 따로 -> 지금 오류 , 이유 찾기

// 자원 상태 전이 설정 안해줘도 잘 나오긴 한다
