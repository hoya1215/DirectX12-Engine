#pragma once

// ��ü SRV �� RTT �� ����� SRV ���� Heap �� ��Ƴ���
// SRV ���� �ڵ鸸 �������� �ڵ����� ������ŭ ���ε�
// RTV �� ���ο� Heap
// SRV �ٸ� Heap ����Ϸ��� SetDescriptorHeap ���� -> ���� ���� , ���� ã��

// �ڿ� ���� ���� ���� �����൵ �� ������ �Ѵ�

// ------ Root Signature , Heap ����
// �Ϲ� ��ü�� Root Signature �� Descriptor Heap��
// ��ó�� Compute Root Signature �� SRV Descriptor Heap �� �и�






// ------ Shader ����
// Control Patch �� ���� �����ָ� Index Buffer �ʿ� �����Ƿ�
// �׳� DrawInstanced ȣ��

// ������������ primitive �� ��ü ���� primitive ���� �����ִ°� ���� 

// Normal Map -> GS ���� ����



// SRV
// PostProcess 5 : ShadowMap 
// 6 : Combine
// 7 : Filter



// Multi Thread
// Resource Command List �� ���� -> �����ָ� meshbuffer nullptr ����
// CommandQueue �ϳ��� Direct �� / Compute �� Draw �ȵ�
// 
// Single -> CommandManager �� list , queue �̿�
// 
// Multi -> threadManager �� list , queue , thread �̿�
// 
// Resource ���� CommandManager list �̿�
