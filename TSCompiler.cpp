#include "TSCompiler.h"

Undef TSPostProcessor::DefineMembers (TSTokens rawTokens) {
	
}

Undef TSPostProcessor::GenerateInstructions (TSTokens rawTokens) {
	//for (SInt32 iToken = 0; iToken < rawTokens.size(); iToken++) {
	//	auto& curToken = rawTokens[iToken];
	//	if ()
	//}
}

Undef TSPostProcessor::ProcExpression (TSTokens expressionTokens) {
	Stack<SInt8> rawTokenStack;
	Deque<SInt8> procTokenDeque;

	for (SInt32 iToken = 0; iToken < expressionTokens.size(); iToken++) {
		auto& token = expressionTokens[iToken];
		auto tokenType = token.m_Type;

	}
}

