#include "../TSVirtualMachine.h"
#include "../TSLexer.h"
#include "../TSCompiler.h"

constexpr auto NUM_INSTRUCTIONS = 15;

TSInstruction _Instr_Void (TSOperation operation) {
	return TSInstruction {
		.m_Operation = operation,
		.m_Value = TSNULL
	};
}

TSInstruction _Instr_I32 (TSOperation operation, TSDataType type, SInt32 value) {
	return TSInstruction {
		.m_Operation = operation,
		.m_Value = TSValue {
			.m_Type = type,
			.m_Data = TSData {
				.m_I32 = value
			}
		}
	};
}

SInt32 main () {
	auto tokens = Tokenize ("5 = (6 + 2 * 3) / 7");
	TSPostProcessor postProcessor;
	postProcessor.ProcExpression (tokens);

	TSASM instr {
		.m_Instructions = new TSInstruction[NUM_INSTRUCTIONS] {
			_Instr_I32  (PUSH, FIXEDPTR_T, NULL),
			_Instr_I32  (PUSH, INT32_T, 0x1),
			_Instr_Void (MEMCPY),
			_Instr_I32  (PUSH, INT32_T, 0x3E8),
			_Instr_I32  (PUSH, FIXEDPTR_T, NULL),
			_Instr_Void (CMPL),
			_Instr_I32  (JMPC, INT32_T, 0xD),
			_Instr_I32  (PUSH, FIXEDPTR_T, NULL),
			_Instr_I32  (PUSH, FIXEDPTR_T, NULL),
			_Instr_I32  (PUSH, INT32_T, 0x1),
			_Instr_Void (ADD),
			_Instr_Void (MEMCPY),
			_Instr_I32  (JMP, INT32_T, 0x3),
			_Instr_I32  (PUSH, FIXEDPTR_T, NULL),
			_Instr_Void (PRINTF)
		},
		.m_Size = NUM_INSTRUCTIONS
	};

	auto* vm = new TSVirtualMachine (128ULL, 1ULL, 1, instr);
	vm->Run ();
	delete vm;

	return 0;
}