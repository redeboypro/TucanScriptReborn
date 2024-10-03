#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include "TSTypes.h"
#include <iostream>
#include <vector>

#define INVALIDTYPETOCAST      "Invalid type to cast!"
#define INVALIDINSTRUCTIONTYPE "Invalid instruction value type!"
#define INVALIDSTACKVALUETYPE  "Invalid popped stack value type!"
#define LOGOUTOFBOUNDS(OBJ)    std::cerr << OBJ ": Element is out of bounds!" << std::endl
#define LOGINSTERR(INST, ERR)  std::cerr << INST ": " ERR << std::endl

union  TSData;
struct TSValue;
struct TSManagedMemory;

enum TSOperation : SInt8 {
	HALT,

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

enum TSDataType : SInt32 {
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
	SInt8            m_C;
	UInt8            m_UC;
	UInt16           m_U16;
	UInt32           m_U32;
	UInt64           m_U64;
	SInt16           m_I16;
	SInt32           m_I32;
	SInt64           m_I64;
	Dec32            m_F32;
	Dec64            m_F64;
	TSManagedMemory* m_ManagedPtr;
	Undef*           m_NativePtr;
};

struct TSValue {
	TSDataType m_Type;
	TSData     m_Data;
};

struct TSManagedMemory {
	TSValue*         m_Memory;
	UInt64           m_Size;
	SInt32           m_RefCount;
	TSManagedMemory* m_Next;
	TSManagedMemory* m_Previous;
};

struct TSInstruction {
	TSOperation m_Operation;
	TSValue		m_Value;
};

struct TSASM {
	TSInstruction* m_Instructions;
	UInt64         m_Size;
};

struct TSBSS {
	TSValue*  m_Variables;
	UInt64    m_Size;
};

class TSStack {
	TSValue* m_Data;
	SInt32   m_End;
public:
	TSStack (UInt64 size);
	~TSStack ();

	const UInt64 m_Size;

	template<typename СTYPE, TSDataType TYPE>
	inline void Push (СTYPE value, СTYPE TSData::* field) {
		TSValue result {};
		result.m_Type = TYPE;
		result.m_Data.*field = value;
		Push (result);
	}

	void Push (TSValue entity);
	void Push (Boolean value);
	void Push (SInt8 value);
	void Push (UInt8 value);
	void Push (UInt16 value);
	void Push (UInt32 value);
	void Push (UInt64 value);
	void Push (SInt16 value);
	void Push (SInt32 value);
	void Push (SInt64 value);
	void Push (Dec32 value);
	void Push (Dec64 value);
	TSValue Pop ();
};

class TSAllocator {
	TSManagedMemory* m_Begin;
	TSManagedMemory* m_End;
public:
	TSAllocator ();

	TSManagedMemory* Alloc (TSValue* memory, UInt64 size);
	void Free (TSManagedMemory* ptr);
	void RemoveRef (TSManagedMemory* ptr);
	void FreeRoot ();
};

class TSVirtualMachine {
	TSStack     m_Stack;
	TSAllocator m_Allocator;
	TSBSS       m_BSS;
	TSASM       m_ASM;

	Generic<Type TYPE>
	SInt32 TryCast (TSValue& value, TYPE TSData::* sourceField) {
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
				return 0;
			}
		}
		return 1;
	}

	Generic<Type TYPE>
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
	TSVirtualMachine (UInt64 stackSize, TSBSS bss, TSASM asm_);
	~TSVirtualMachine ();

	void Run (SInt32 entryPoint = 0);
	void Free ();
};

#endif