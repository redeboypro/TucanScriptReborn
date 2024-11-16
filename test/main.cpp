#include "..\TSVirtualMachine.h"

constexpr auto NUM_INSTRUCTIONS = 4;

constexpr TSValue TSNULL {
	.m_Type = INT32_T,
	.m_Data = TSData {
		.m_I32 = NULL
	}
};

TSInstruction CreateInstruction (TSOperation operation) {
	return TSInstruction {
		.m_Operation = operation,
		.m_Value = TSNULL
	};
}

SInt32 main () {

	TSASM instr {
		.m_Instructions = new TSInstruction[NUM_INSTRUCTIONS] {
			TSInstruction {
				.m_Operation = PUSH,
				.m_Value = TSValue {
					.m_Type = INT32_T,
					.m_Data = TSData {
						.m_I32 = 2
					}
				}
			},
			TSInstruction {
				.m_Operation = PUSH,
				.m_Value = TSValue {
					.m_Type = INT32_T,
					.m_Data = TSData {
						.m_I32 = 3
					}
				}
			},
			CreateInstruction(ADD),
			CreateInstruction(PRINTF)
		},
		.m_Size = NUM_INSTRUCTIONS
	};

	auto* vm = new TSVirtualMachine (128ULL, 1ULL, 1, instr);
	vm->Run ();
	delete vm;

	return 0;
}