#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <wtypes.h>
#include <iostream>
#include <vector>

#define INVALIDTYPETOCAST      "Invalid type to cast!"
#define INVALIDINSTRUCTIONTYPE "Invalid instruction value type!"
#define INVALIDSTACKVALUETYPE  "Invalid popped stack value type!"
#define LOGOUTOFBOUNDS(OBJ)    std::cerr << OBJ ": Element is out of bounds!" << std::endl
#define LOGINSTERR(INST, ERR)  std::cerr << INST ": " ERR << std::endl

typedef int RESULT;

union  TSData;
struct TSValue;
struct TSManagedMemory;

enum TSOperation : CHAR {
	PUSH,
	POP,
	JMP,
	JMPC,		//Jump if true (Conditional jump)

	MEMALLOC,
	MEMDEALLOC,
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

	PRINTF
};

enum TSDataType : INT {
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
	VAR_T,
	MANAGED_T,
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
	INT              m_RefCount;
	TSManagedMemory* m_Next;
	TSManagedMemory* m_Previous;
};

struct TSInstruction {
	TSOperation m_Operation;
	TSValue		m_Value;
};

struct TSASM {
	TSInstruction* m_Instructions;
	ULONG_PTR      m_Size;
};

struct TSBSS {
	TSValue*  m_Variables;
	ULONG_PTR m_Size;
};

class TSStack {
	TSValue* m_Data;
	INT      m_End;
public:
	TSStack (ULONG_PTR size);
	~TSStack ();

	const ULONG_PTR m_Size;

	template<typename СTYPE, TSDataType TYPE>
	inline void Push (СTYPE value, СTYPE TSData::* field) {
		TSValue result {};
		result.m_Type = TYPE;
		result.m_Data.*field = value;
		Push (result);
	}

	void Push (TSValue entity);
	void Push (bool value);
	void Push (CHAR value);
	void Push (BYTE value);
	void Push (UINT16 value);
	void Push (UINT32 value);
	void Push (UINT64 value);
	void Push (INT16 value);
	void Push (INT32 value);
	void Push (INT64 value);
	void Push (FLOAT value);
	void Push (DOUBLE value);
	TSValue Pop ();
};

class TSAllocator {
	TSManagedMemory* m_Begin;
	TSManagedMemory* m_End;
public:
	TSAllocator ();

	TSManagedMemory* Alloc (TSValue* memory, ULONG_PTR size);
	void Free (TSManagedMemory* ptr);
	void RemoveRef (TSManagedMemory* ptr);
	void FreeRoot ();
};

class TSVirtualMachine {
	TSStack     m_Stack;
	TSAllocator m_Allocator;
	TSBSS       m_BSS;
	TSASM       m_ASM;

	template<typename TYPE>
	RESULT TryCast (TSValue& value, TYPE TSData::* sourceField) {
		switch (value.m_Type) {
			case CHAR_T:
			value.m_Data.*sourceField = static_cast<TYPE>(value.m_Data.m_C);
			break;
			case BYTE_T:
			value.m_Data.*sourceField = static_cast<TYPE>(value.m_Data.m_UC);
			break;
			case UINT16_T:
			value.m_Data.*sourceField = static_cast<TYPE>(value.m_Data.m_U16);
			break;
			case UINT32_T:
			value.m_Data.*sourceField = static_cast<TYPE>(value.m_Data.m_U32);
			break;
			case UINT64_T:
			value.m_Data.*sourceField = static_cast<TYPE>(value.m_Data.m_U64);
			break;
			case INT16_T:
			value.m_Data.*sourceField = static_cast<TYPE>(value.m_Data.m_I16);
			break;
			case INT32_T:
			value.m_Data.*sourceField = static_cast<TYPE>(value.m_Data.m_I32);
			break;
			case INT64_T:
			value.m_Data.*sourceField = static_cast<TYPE>(value.m_Data.m_I64);
			break;
			case FLOAT32_T:
			value.m_Data.*sourceField = static_cast<TYPE>(value.m_Data.m_F32);
			break;
			case FLOAT64_T:
			value.m_Data.*sourceField = static_cast<TYPE>(value.m_Data.m_F64);
			break;
			default: {
				LOGINSTERR ("CAST", INVALIDTYPETOCAST);
				Free ();
				return FALSE;
			}
		}
		return TRUE;
	}

	template<typename TYPE>
	void Cast (TSDataType type, TYPE TSData::* sourceField) {
		auto poppedValue = m_Stack.Pop ();
		if (poppedValue.m_Type == VAR_T) {
			auto variable = m_BSS.m_Variables[poppedValue.m_Data.m_I32];
			if (TryCast<TYPE> (variable, sourceField)) {
				variable.m_Type = type;
				m_Stack.Push (variable);
			}
		}
		else {
			if (TryCast<TYPE> (poppedValue, sourceField)) {
				poppedValue.m_Type = type;
				m_Stack.Push (poppedValue);
			}
		}
	}

	void PrintF (const TSValue& value) {
		switch (value.m_Type) {
			case CHAR_T:
			std::cout << value.m_Data.m_C;
			break;
			case BYTE_T:
			std::cout << value.m_Data.m_UC;
			break;
			case UINT16_T:
			std::cout << value.m_Data.m_U16;
			break;
			case UINT32_T:
			std::cout << value.m_Data.m_U32;
			break;
			case UINT64_T:
			std::cout << value.m_Data.m_U64;
			break;
			case INT16_T:
			std::cout << value.m_Data.m_I16;
			break;
			case INT32_T:
			std::cout << value.m_Data.m_I32;
			break;
			case INT64_T:
			std::cout << value.m_Data.m_I64;
			break;
			case FLOAT32_T:
			std::cout << value.m_Data.m_F32;
			break;
			case FLOAT64_T:
			std::cout << value.m_Data.m_F64;
			break;
		}
		std::flush (std::cout);
	}

public:
	TSVirtualMachine (ULONG_PTR stackSize, TSBSS bss, TSASM asm_);
	~TSVirtualMachine ();

	void Run (INT entryPoint = 0);
	void Free ();
};

#endif