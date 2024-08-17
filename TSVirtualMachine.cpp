#include "TSVirtualMachine.h"

TSStack::TSStack (ULONG_PTR size) : m_End(NULL) {
	m_Data = new TSEntity[size];
}

TSStack::~TSStack () {
	delete[] m_Data;
}

void TSStack::Push (TSEntity entity) {
	m_Data[m_End] = entity;
	m_End++;
}

TSEntity TSStack::Pop () {
	m_End--;
	return m_Data[m_End];
}

TSVirtualMachine::TSVirtualMachine (ULONG_PTR stackSize) : m_Stack(stackSize) {

}
