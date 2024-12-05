#ifndef COMPILER_H
#define COMPILER_H

#include "TSVirtualMachine.h"
#include "TSLexer.h"

#define INCLUDE(VALUE, A, B) ((VALUE) >= (A) && (VALUE) <= (B))

#define LOWER_PRECEDENCE         0x0
#define OR_PRECEDENCE            0x1
#define AND_PRECEDENCE           0x2
#define CMP_PRECEDENCE           0x3
#define ARITHMETIC_PRECEDENCE_F1 0x4
#define ARITHMETIC_PRECEDENCE_F2 0x5

#define LPAREN(TYPE) ((TYPE) == TSTokenType::LPAREN)
#define RPAREN(TYPE) ((TYPE) == TSTokenType::RPAREN)

#define CASE_OP case TSTokenType::STR:     \
				case TSTokenType::CHAR:    \
				case TSTokenType::BYTE:    \
				case TSTokenType::UINT16:  \
				case TSTokenType::UINT32:  \
				case TSTokenType::UINT64:  \
				case TSTokenType::INT16:   \
				case TSTokenType::INT32:   \
				case TSTokenType::INT64:   \
				case TSTokenType::FLOAT32: \
				case TSTokenType::FLOAT64

inline SInt32 Precedence (TSTokenType type) {
	switch (type) {
		case TSTokenType::CMPE:
		case TSTokenType::CMPG:
		case TSTokenType::CMPL:
		case TSTokenType::CMPGE:
		case TSTokenType::CMPLE:
		case TSTokenType::NOT:
		case TSTokenType::NOT_EQ:
		return CMP_PRECEDENCE;

		case TSTokenType::PLUS:
		case TSTokenType::MINUS:
		return ARITHMETIC_PRECEDENCE_F1;

		case TSTokenType::PERCENT:
		case TSTokenType::MUL:
		case TSTokenType::DIV:
		return ARITHMETIC_PRECEDENCE_F2;

		case TSTokenType::AND:
		return AND_PRECEDENCE;

		case TSTokenType::OR:
		return OR_PRECEDENCE;

		default:
		return LOWER_PRECEDENCE;
	}
}

struct TSPostProcessor {
	

	Undef DefineMembers (TSTokens rawTokens);
	Undef GenerateInstructions (TSTokens rawTokens);
	Undef ProcExpression (TSTokens expressionTokens);
};

#endif