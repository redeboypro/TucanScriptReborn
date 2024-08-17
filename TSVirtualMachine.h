#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <Windows.h>
#include <vector>

enum TSDataType : CHAR {
	CHAR_T,
	BYTE_T,
	UINT16_T,
	UINT32_T,
	UINT64_T,
	INT16_T,
	INT32_T,
	INT64_T,
	FLOAT32_T,
	FLOAT64_T,
	PTR_T
};

union TSData {
	CHAR      m_C;
	BYTE      m_UC;
	UINT16    m_U16;
	UINT32    m_U32;
	UINT64    m_U64;
	INT16     m_I16;
	INT32     m_I32;
	INT64     m_I64;
	FLOAT     m_F32;
	DOUBLE    m_F64;
	ULONG_PTR m_Ptr;
};

struct TSEntity {
	TSDataType m_Type;
	TSData     m_Data;
};

struct TSBuffer {
	VOID* m_Memory;
};

class TSStack {
	TSEntity* m_Data;
	INT       m_End;
public:
	TSStack (ULONG_PTR size);
	~TSStack ();

	void Push (TSEntity entity);
	TSEntity Pop ();
};

class TSVirtualMachine {
	TSStack               m_Stack;
	std::vector<TSBuffer> m_Heap;
public:
	TSVirtualMachine (ULONG_PTR stackSize);
};

#endif