#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <Windows.h>
#include <iostream>
#include <vector>

enum TSOperation : CHAR {
	PUSH,
	JMP,

	MEMALLOC,
	MEMDEALLOC,
	MEMSET,
	MEMCPY,

	TOC,
	TOUC,
	TOU16,
	TOU32,
	TOU64,
	TOI16,
	TOI32,
	TOI64,
	TOF32,
	TOF64,

	ADD,
	SUB,
	MUL,
	DIV,

	CMPE,
	CMPNE,
	CMPG,
	CMPL,
	CMPGE,
	CMPLE,
};

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
	MANAGED_T,
	MANAGEDARR_T,
	NATIVEPTR_T
};

union TSData {
	CHAR             m_C;
	BYTE             m_UC;
	UINT16           m_U16;
	UINT32           m_U32;
	UINT64           m_U64;
	INT16            m_I16;
	INT32            m_I32;
	INT64            m_I64;
	FLOAT            m_F32;
	DOUBLE           m_F64;
	TSManagedMemory* m_ManagedPtr;
	VOID*            m_NativePtr;
};

struct TSValue {
	TSDataType m_Type;
	TSData     m_Data;
};

struct TSManagedMemory {
	TSValue*         m_Memory;
	ULONG_PTR        m_Size;
	INT              m_NumBindings;
	TSManagedMemory* m_Next;
	TSManagedMemory* m_Previous;

	void Free ();
};

struct TSInstruction {
	TSOperation m_Operation;
	TSValue m_Value;
};

struct TSASM {
	TSInstruction* m_Instructions;
	ULONG_PTR      m_Size;
};

class TSStack {
	TSValue* m_Data;
	INT      m_End;
public:
	TSStack (ULONG_PTR size);
	~TSStack ();

	void Push (TSValue entity);
	TSValue Pop ();
};

class TSAllocator {
	TSManagedMemory* m_Begin;
	TSManagedMemory* m_End;
public:
	TSAllocator ();

	TSManagedMemory* Alloc (TSValue* memory, ULONG_PTR size);
	void Free (TSManagedMemory* ptr);
	void FreeRoot ();
};

class TSVirtualMachine {
	TSStack     m_Stack;
	TSAllocator m_Allocator;
	TSASM       m_ASM;
public:
	TSVirtualMachine (ULONG_PTR stackSize, TSASM asmSet);
	~TSVirtualMachine ();

	void Run ();
};

#endif