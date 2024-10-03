#include "..\TSVirtualMachine.h"

constexpr auto NUM_INSTRUCTIONS = 9;

int main () {
	TSBSS variables {
		.m_Variables = new TSValue[NULL],
		.m_Size = 0
	};

	auto* instructions = new TSInstruction[NUM_INSTRUCTIONS] {
		TSInstruction {
			.m_Operation = PUSH,
			.m_Value = TSValue {
				.m_Type = INT32_T,
				.m_Data = TSData {
					.m_I32 = 3
				}
			}
		},
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
			.m_Operation = ADD
		},
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
			.m_Operation = MUL
		},
		TSInstruction {
			.m_Operation = TOF32
		},
		TSInstruction {
			.m_Operation = PUSH,
			.m_Value = TSValue {
				.m_Type = FLOAT32_T,
				.m_Data = TSData {
					.m_F32 = 0.5F
				}
			}
		},
		TSInstruction {
			.m_Operation = ADD
		},
		TSInstruction {
			.m_Operation = PRINTF
		},
	};

	TSASM instructionSet {
		.m_Instructions = instructions,
		.m_Size = NUM_INSTRUCTIONS
	};

	auto* vmInstance = new TSVirtualMachine (512UL, variables, instructionSet);
	vmInstance->Run (); //Should print "10.5"
	delete vmInstance;
}