#ifndef COMPILER_H
#define COMPILER_H

#include "TSVirtualMachine.h"
#include "TSLexer.h"

class TSPostProcessor {
	Undef DefineMembers (TSTokens rawTokens);
	Undef GenerateInstructions (TSTokens rawTokens);
	Undef ProcExpression (TSTokens expressionTokens);
};

#endif