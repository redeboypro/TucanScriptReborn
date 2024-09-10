#include "TSVirtualMachine.h"

TSStack::TSStack (ULONG_PTR size) : m_End(NULL), m_Size(size) {
	m_Data = new TSValue[size];
}

TSStack::~TSStack () {
	delete[] m_Data;
}

void TSStack::Push (TSValue entity) {
	m_Data[m_End] = entity;
	m_End++;
}

TSValue TSStack::Pop () {
	m_End--;
	return m_Data[m_End];
}

TSVirtualMachine::TSVirtualMachine (ULONG_PTR stackSize, TSBSS bss, TSASM asm_) :
	m_Stack(stackSize), m_BSS (std::move (bss)), m_ASM (std::move (asm_)) {}

TSVirtualMachine::~TSVirtualMachine () {
	m_Allocator.FreeRoot ();
	delete[] m_ASM.m_Instructions;
}

TSAllocator::TSAllocator () : m_Begin (nullptr), m_End (nullptr) {}

TSManagedMemory* TSAllocator::Alloc (TSValue* memory, ULONG_PTR size) {
	for (INT iValue = 0; iValue < size; iValue++) {
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

void TSAllocator::Free (TSManagedMemory* ptr) {
	auto* previousPtr = ptr->m_Previous;
	auto* nextPtr = ptr->m_Next;

	for (INT iValue = 0; iValue < ptr->m_Size; ++iValue) {
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

void TSAllocator::RemoveRef (TSManagedMemory* ptr) {
	if ((--ptr->m_RefCount) == NULL) {
		Free (ptr);
	}
}

void TSAllocator::FreeRoot () {

	while (m_Begin) {
		auto* nextPtr = m_Begin->m_Next;
		delete m_Begin;
		m_Begin = nextPtr;
	}
}

void TSVirtualMachine::Run (INT entryPoint) {
	for (INT iInst = entryPoint; iInst < m_ASM.m_Size; ++iInst) {
		auto& instruction = m_ASM.m_Instructions[iInst];
		switch (instruction.m_Operation) {
			case PUSH: {
				m_Stack.Push (instruction.m_Value);
				break;
			}
			case POP: {
				m_Stack.Pop ();
				break;
			}
			case JMP: {
				auto instructionValue = instruction.m_Value;
				if (instructionValue.m_Type == INT32_T) {
					iInst = instructionValue.m_Data.m_I32;
				}
				else {
					LOGINSTERR ("JMP", INVALIDINSTRUCTIONTYPE);
					Free ();
					return;
				}
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
			case MEMALLOC: {
				ULONG_PTR size = instruction.m_Value.m_Data.m_I32;
				auto* memoryValues = new TSValue[size];

				for (INT iValue = 0; iValue < size; ++iValue) {
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

				if (poppedValue.m_Type != VAR_T) {
					LOGINSTERR ("MEMDEALLOC", INVALIDSTACKVALUETYPE);
					Free ();
					return;
				}

				auto* variable = &m_BSS.m_Variables[poppedValue.m_Data.m_I32];

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

				if (dest.m_Type != VAR_T) {
					LOGINSTERR ("MEMSET", INVALIDSTACKVALUETYPE);
					Free ();
					return;
				}

				auto* validDestination = &m_BSS.m_Variables[dest.m_Data.m_I32];

				if (validDestination->m_Type == MANAGED_T) {
					m_Allocator.RemoveRef (validDestination->m_Data.m_ManagedPtr);
				}

				if (src.m_Type == VAR_T) {
					auto* srcVariable = &m_BSS.m_Variables[src.m_Data.m_I32];
					std::memcpy (validDestination, srcVariable, sizeof (TSValue));
				}
				else {
					std::memcpy (validDestination, &src, sizeof (TSValue));
				}
				break;
			}
			case TOC: {
				Cast <CHAR> (CHAR_T, &TSData::m_C);
				break;
			}
			case TOUC: {
				Cast <BYTE> (BYTE_T, &TSData::m_UC);
				break;
			}
			case TOU16: {
				Cast <UINT16> (UINT16_T, &TSData::m_U16);
				break;
			}
			case TOU32: {
				Cast <UINT32> (UINT32_T, &TSData::m_U32);
				break;
			}
			case TOU64: {
				Cast <UINT64> (UINT64_T, &TSData::m_U64);
				break;
			}
			case TOI16: {
				Cast <INT16> (INT16_T, &TSData::m_I16);
				break;
			}
			case TOI32: {
				Cast <INT32> (INT32_T, &TSData::m_I32);
				break;
			}
			case TOI64: {
				Cast <INT64> (INT64_T, &TSData::m_I64);
				break;
			}
			case TOF32: {
				Cast <FLOAT> (FLOAT32_T, &TSData::m_F32);
				break;
			}
			case TOF64: {
				Cast <DOUBLE> (FLOAT64_T, &TSData::m_F64);
				break;
			}
		}
	}
}

void TSVirtualMachine::Free () {
	m_Allocator.FreeRoot ();
	delete[] m_BSS.m_Variables;
	delete[] m_ASM.m_Instructions;
}