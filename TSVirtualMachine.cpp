#include "TSVirtualMachine.h"

TSStack::TSStack (ULONG_PTR size) : m_End(NULL) {
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

void TSManagedMemory::Free () {
	if (m_Size > 1) { 
		delete[] m_Memory; 
	}
	else {
		delete m_Memory; 
	}
}

TSVirtualMachine::TSVirtualMachine (ULONG_PTR stackSize, TSASM asmSet) : 
	m_Stack(stackSize), m_ASM(std::move(asmSet)) {}

TSVirtualMachine::~TSVirtualMachine () {
	m_Allocator.FreeRoot ();
	delete[] m_ASM.m_Instructions;
}

TSAllocator::TSAllocator () : m_Begin (nullptr), m_End (nullptr) {}

TSManagedMemory* TSAllocator::Alloc (TSValue* memory, ULONG_PTR size) {
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

	ptr->Free ();

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

void TSAllocator::FreeRoot () {

	while (m_Begin) {
		auto* nextPtr = m_Begin->m_Next;
		delete m_Begin;
		m_Begin = nextPtr;
	}
}

void TSVirtualMachine::Run () {

}