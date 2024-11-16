#include "TSVirtualMachine.h"

#define APPLY_OPERATION(AVAL, BVAL, OP)                             \
switch ((AVAL).m_Type) {                                            \
	case CHAR_T:                                                    \
	m_Stack.Push ((AVAL).m_Data.m_C OP (BVAL).m_Data.m_C);          \
	break;                                                          \
	case BYTE_T:                                                    \
	m_Stack.Push ((AVAL).m_Data.m_UC OP (BVAL).m_Data.m_UC);        \
	break;                                                          \
	case UINT16_T:                                                  \
	m_Stack.Push ((AVAL).m_Data.m_U16 OP (BVAL).m_Data.m_U16);      \
	break;                                                          \
	case UINT32_T:                                                  \
	m_Stack.Push ((AVAL).m_Data.m_U32 OP (BVAL).m_Data.m_U32);      \
	break;                                                          \
	case UINT64_T:                                                  \
	m_Stack.Push ((AVAL).m_Data.m_U64 OP (BVAL).m_Data.m_U64);      \
	break;                                                          \
	case INT16_T:                                                   \
	m_Stack.Push ((AVAL).m_Data.m_I16 OP (BVAL).m_Data.m_I16);      \
	break;                                                          \
	case INT32_T:                                                   \
	m_Stack.Push ((AVAL).m_Data.m_I32 OP (BVAL).m_Data.m_I32);      \
	break;                                                          \
	case INT64_T:                                                   \
	m_Stack.Push ((AVAL).m_Data.m_I64 OP (BVAL).m_Data.m_I64);      \
	break;                                                          \
	case FLOAT32_T:                                                 \
	m_Stack.Push ((AVAL).m_Data.m_F32 OP (BVAL).m_Data.m_F32);      \
	break;                                                          \
	case FLOAT64_T:                                                 \
	m_Stack.Push ((AVAL).m_Data.m_F64 OP (BVAL).m_Data.m_F64);      \
	break;                                                          \
}

TSStack::TSStack (UInt64 size) : m_End(NULL), m_Size(size) {
	m_Data = new TSValue[size];
}

TSStack::~TSStack () {
	delete[] m_Data;
}

Undef TSStack::Push (TSValue entity) {
	m_Data[m_End] = entity;
	m_End++;
}

Undef TSStack::Push (Boolean value) {
	Push (static_cast<SInt32>(value));
}

Undef TSStack::Push (SInt8 value) {
	Push <SInt8, CHAR_T> (value, &TSData::m_C);
}

Undef TSStack::Push (UInt8 value) {
	Push <UInt8, BYTE_T> (value, &TSData::m_UC);
}

Undef TSStack::Push (UInt16 value) {
	Push <UInt16, UINT16_T> (value, &TSData::m_U16);
}

Undef TSStack::Push (UInt32 value) {
	Push <UInt32, UINT32_T> (value, &TSData::m_U32);
}

Undef TSStack::Push (UInt64 value) {
	Push <UInt64, UINT64_T> (value, &TSData::m_U64);
}

Undef TSStack::Push (SInt16 value) {
	Push <SInt16, INT16_T> (value, &TSData::m_I16);
}

Undef TSStack::Push (SInt32 value) {
	Push <SInt32, INT32_T> (value, &TSData::m_I32);
}

Undef TSStack::Push (SInt64 value) {
	Push <SInt64, INT64_T> (value, &TSData::m_I64);
}

Undef TSStack::Push (Dec32 value) {
	Push <Dec32, FLOAT32_T> (value, &TSData::m_F32);
}

Undef TSStack::Push (Dec64 value) {
	Push <Dec64, FLOAT64_T> (value, &TSData::m_F64);
}

TSValue TSStack::Pop () {
	m_End--;
	return m_Data[m_End];
}

#define FIXEDPTR(VALUE) VALUE.m_Type == FIXEDPTR_T
TSValue TSVirtualMachine::PopUnpack () {
	auto poppedValue = m_Stack.Pop ();
	if (FIXEDPTR(poppedValue)) {
		auto* extractedMemory = &m_FixedMemory.m_Memory[poppedValue.m_Data.m_I32];
		poppedValue.m_Data = extractedMemory->m_Data;
		poppedValue.m_Type = extractedMemory->m_Type;
	}
	return poppedValue;
}
#undef OFTYPE

TSVirtualMachine::TSVirtualMachine (UInt64 stackSize, UInt64 fixedMemSize, SInt32 callDepth, TSASM asm_) :
	m_Stack (stackSize), m_ASM (std::move (asm_)), 
	m_JmpMemory {
		.m_Pointers = new SInt32[callDepth],
		.m_Depth = NULL
	},
	m_FixedMemory {
		.m_Memory = new TSValue[fixedMemSize],
		.m_Size = fixedMemSize
	} {}

TSVirtualMachine::~TSVirtualMachine () {
	m_Allocator.FreeRoot ();
	delete[] m_ASM.m_Instructions;
}

TSAllocator::TSAllocator () : m_Begin (nullptr), m_End (nullptr) {}

const TSManagedMemory& TSAllocator::Begin () {
	return *m_Begin;
}

const TSManagedMemory& TSAllocator::End () {
	return *m_End;
}

TSManagedMemory* TSAllocator::Alloc (TSValue* memory, UInt64 size) {
	for (SInt32 iValue = 0; iValue < size; iValue++) {
		auto& value = memory;
		if (value->m_Type == MANAGED_T) {
			value->m_Data.m_ManagedPtr->m_RefCount++;
		}
	}

	auto* allocated = new TSManagedMemory {
		.m_Memory = memory,
		.m_Size = size,
		.m_Next = nullptr,
		.m_Previous = m_End
	};

	if (m_End) {
		m_End->m_Next = allocated;
	}

	m_End = allocated;

	if (!m_Begin) {
		m_Begin = allocated;
	}

	return allocated;
}

Undef TSAllocator::Free (TSManagedMemory* ptr) {
	auto* previousPtr = ptr->m_Previous;
	auto* nextPtr = ptr->m_Next;

	for (SInt32 iValue = 0; iValue < ptr->m_Size; ++iValue) {
		auto& value = ptr->m_Memory[iValue];
		if (value.m_Type == MANAGED_T) {
			RemoveRef (value.m_Data.m_ManagedPtr);
		}
	}
	delete[] ptr->m_Memory;

	if (previousPtr) {
		previousPtr->m_Next = nextPtr;
	}
	else {
		m_Begin = nextPtr;
	}

	if (nextPtr) {
		nextPtr->m_Previous = previousPtr;
	} 
	else {
		m_End = previousPtr;
	}

	delete ptr;
}

Undef TSAllocator::RemoveRef (TSManagedMemory* ptr) {
	if ((--ptr->m_RefCount) == NULL) {
		Free (ptr);
	}
}

Undef TSAllocator::FreeRoot () {

	while (m_Begin) {
		auto* nextPtr = m_Begin->m_Next;
		delete m_Begin;
		m_Begin = nextPtr;
	}
}

Undef TSVirtualMachine::Jmp (SInt32& iInst, TSInstruction& instruction) {
	auto instructionValue = instruction.m_Value;
	if (instructionValue.m_Type == INT32_T) {
		iInst = instructionValue.m_Data.m_I32;
	}
	else {
		LOGINSTERR ("JMP", INVALIDINSTRUCTIONTYPE);
		Free ();
		return;
	}
}

Undef TSVirtualMachine::Run (SInt32 entryPoint) {
	for (SInt32 iInst = entryPoint; iInst < m_ASM.m_Size; ++iInst) {
		auto& instruction = m_ASM.m_Instructions[iInst];
		switch (instruction.m_Operation) {
			case HALT: {
				Free ();
				return;
			}
			case PUSH: {
				m_Stack.Push (instruction.m_Value);
				break;
			}
			case POP: {
				m_Stack.Pop ();
				break;
			}
			case JMP: {
				Jmp (iInst, instruction);
				break;
			}
			case JMPC: {
				auto instructionValue = instruction.m_Value;
				if (instructionValue.m_Type == INT32_T) {
					auto popped = m_Stack.Pop ();
					if ((popped.m_Type == BYTE_T  && popped.m_Data.m_UC) ||
						(popped.m_Type == CHAR_T  && popped.m_Data.m_C)  || 
						(popped.m_Type == INT32_T && popped.m_Data.m_I32)) {
						iInst = instructionValue.m_Data.m_I32;
					}
				}
				else {
					LOGINSTERR ("JMPC", INVALIDINSTRUCTIONTYPE);
					Free ();
					return;
				}
				break;
			}
			case JMPR: {
				m_JmpMemory.m_Pointers[m_JmpMemory.m_Depth++] = NEXT (iInst);
				Jmp (iInst, instruction);
				break;
			}
			case RETURN: {
				iInst = m_JmpMemory.m_Pointers[--m_JmpMemory.m_Depth];
				break;
			}
			case MEMALLOC: {
				UInt64 size = instruction.m_Value.m_Data.m_I32;
				auto* memoryValues = new TSValue[size];

				for (SInt32 iValue = 0; iValue < size; ++iValue) {
					memoryValues[iValue] = m_Stack.Pop ();
				}

				m_Stack.Push (TSValue {
					.m_Type = MANAGED_T,
					.m_Data = 
						TSData {
							.m_ManagedPtr = m_Allocator.Alloc(memoryValues, size)
						}
				});
				break;
			}
			case MEMDEALLOC: {
				auto poppedValue = m_Stack.Pop ();

				if (poppedValue.m_Type != FIXEDPTR_T) {
					LOGINSTERR ("MEMDEALLOC", INVALIDSTACKVALUETYPE);
					Free ();
					return;
				}

				auto* variable = &m_FixedMemory.m_Memory[poppedValue.m_Data.m_I32];

				if (variable->m_Type == MANAGED_T) {
					m_Allocator.Free (variable->m_Data.m_ManagedPtr);
				}
				else {
					variable->m_Data.m_U64 = NULL;
				}
				break;
			}
			case MEMCPY: {
				auto src = m_Stack.Pop ();
				auto dest = m_Stack.Pop ();

				if (dest.m_Type != FIXEDPTR_T) {
					LOGINSTERR ("MEMCPY", INVALIDSTACKVALUETYPE);
					Free ();
					return;
				}

				auto* validDestination = &m_FixedMemory.m_Memory[dest.m_Data.m_I32];

				if (validDestination->m_Type == MANAGED_T) {
					m_Allocator.RemoveRef (validDestination->m_Data.m_ManagedPtr);
				}

				if (src.m_Type == FIXEDPTR_T) {
					auto* srcVariable = &m_FixedMemory.m_Memory[src.m_Data.m_I32];
					std::memcpy (validDestination, srcVariable, sizeof (TSValue));
				}
				else {
					std::memcpy (validDestination, &src, sizeof (TSValue));
				}
				break;
			}
			case TOC: {
				Cast <SInt8> (CHAR_T, &TSData::m_C);
				break;
			}
			case TOUC: {
				Cast <UInt8> (BYTE_T, &TSData::m_UC);
				break;
			}
			case TOU16: {
				Cast <UInt16> (UINT16_T, &TSData::m_U16);
				break;
			}
			case TOU32: {
				Cast <UInt32> (UINT32_T, &TSData::m_U32);
				break;
			}
			case TOU64: {
				Cast <UInt64> (UINT64_T, &TSData::m_U64);
				break;
			}
			case TOI16: {
				Cast <SInt16> (INT16_T, &TSData::m_I16);
				break;
			}
			case TOI32: {
				Cast <SInt32> (INT32_T, &TSData::m_I32);
				break;
			}
			case TOI64: {
				Cast <SInt64> (INT64_T, &TSData::m_I64);
				break;
			}
			case TOF32: {
				Cast <Dec32> (FLOAT32_T, &TSData::m_F32);
				break;
			}
			case TOF64: {
				Cast <Dec64> (FLOAT64_T, &TSData::m_F64);
				break;
			}
			case ADD: {
				auto a = PopUnpack ();
				auto b = PopUnpack ();
				APPLY_OPERATION (a, b, +);
				break;
			}
			case SUB: {
				auto a = PopUnpack ();
				auto b = PopUnpack ();
				APPLY_OPERATION (a, b, -);
				break;
			}
			case MUL: {
				auto a = PopUnpack ();
				auto b = PopUnpack ();
				APPLY_OPERATION (a, b, *);
				break;
			}
			case DIV: {
				auto a = PopUnpack ();
				auto b = PopUnpack ();
				APPLY_OPERATION (a, b, /);
				break;
			}
			case CMPE: {
				auto a = PopUnpack ();
				auto b = PopUnpack ();
				APPLY_OPERATION (a, b, ==);
				break;
			}
			case CMPNE: {
				auto a = PopUnpack ();
				auto b = PopUnpack ();
				APPLY_OPERATION (a, b, !=);
				break;
			}
			case CMPG: {
				auto a = PopUnpack ();
				auto b = PopUnpack ();
				APPLY_OPERATION (a, b, >);
				break;
			}
			case CMPL: {
				auto a = PopUnpack ();
				auto b = PopUnpack ();
				APPLY_OPERATION (a, b, <);
				break;
			}
			case CMPGE: {
				auto a = PopUnpack ();
				auto b = PopUnpack ();
				APPLY_OPERATION (a, b, >=);
				break;
			}
			case CMPLE: {
				auto a = PopUnpack ();
				auto b = PopUnpack ();
				APPLY_OPERATION (a, b, <=);
				break;
			}
			case PRINTF: {
				PrintF (PopUnpack ());
			}
		}
	}
}

void TSVirtualMachine::Free () {
	m_Allocator.FreeRoot ();
	delete[] m_FixedMemory.m_Memory;
	delete[] m_ASM.m_Instructions;
}