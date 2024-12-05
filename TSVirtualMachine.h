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
#define PREV(INDEX) INDEX - 1
#define NEXT(INDEX) INDEX + 1

union  TSData;
struct TSValue;
struct TSManagedMemory;

enum TSOperation : SInt8 {
	HALT,

	PUSH,
	POP,
	JMP,
	JMPC,		//Jump if false (Conditional jump)
	JMPCV,		//Conditional jump variant
	JMPR,       //Jump with recording
	RETURN,

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
	FIXEDPTR_T,
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

struct TSMemoryView {
	TSValue* m_Memory;
	UInt64   m_Size;
};

struct TSJmpMemory {
	SInt32* m_Pointers;
	SInt32  m_Depth;
};

class TSStack {
	TSValue* m_Data;
	SInt32   m_End;
public:
	TSStack (UInt64 size);
	~TSStack ();

	const UInt64 m_Size;

	Generic<Type СTYPE, TSDataType TYPE>
	inline Undef Push (СTYPE value, СTYPE TSData::* field) {
		TSValue result {
			.m_Type = TYPE
		};
		result.m_Data.*field = value;
		Push (result);
	}

	Undef Push (TSValue entity);
	Undef Push (Boolean value);
	Undef Push (SInt8 value);
	Undef Push (UInt8 value);
	Undef Push (UInt16 value);
	Undef Push (UInt32 value);
	Undef Push (UInt64 value);
	Undef Push (SInt16 value);
	Undef Push (SInt32 value);
	Undef Push (SInt64 value);
	Undef Push (Dec32 value);
	Undef Push (Dec64 value);
	TSValue Pop ();
};

class TSAllocator {
	TSManagedMemory* m_Begin;
	TSManagedMemory* m_End;
public:
	TSAllocator ();

	const TSManagedMemory& Begin ();
	const TSManagedMemory& End ();

	TSManagedMemory* Alloc (TSValue* memory, UInt64 size);
	Undef Free (TSManagedMemory* ptr);
	Undef RemoveRef (TSManagedMemory* ptr);
	Undef FreeRoot ();
};

class TSVirtualMachine {
	TSStack      m_Stack;
	TSAllocator  m_Allocator;
	TSASM        m_ASM;
	TSMemoryView m_FixedMemory;
	TSJmpMemory  m_JmpMemory;

	TSValue PopUnpack ();
	Undef Jmp (SInt32& iInst, TSInstruction& instruction);

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
		if (poppedValue.m_Type == FIXEDPTR_T) {
			auto variable = m_FixedMemory.m_Memory[poppedValue.m_Data.m_I32];
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

	inline Boolean IsTrue (const TSValue& value) {
		return static_cast<Boolean>(value.m_Type == BYTE_T  && value.m_Data.m_UC) ||
			                       (value.m_Type == CHAR_T  && value.m_Data.m_C)  ||
                                   (value.m_Type == INT32_T && value.m_Data.m_I32);
	}

public:
	TSVirtualMachine (UInt64 stackSize, UInt64 fixedMemSize, SInt32 callDepth, TSASM asm_);
	~TSVirtualMachine ();

	void Run (SInt32 entryPoint = 0);
	void Free ();
};

constexpr TSValue TSNULL {
	.m_Type = INT32_T,
	.m_Data = TSData {
		.m_I32 = NULL
	}
};

#endif