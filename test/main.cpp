#include "..\TSVirtualMachine.h"

constexpr auto NUM_INSTRUCTIONS = 4;
constexpr auto A = 3; 
constexpr auto B = 2;

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
					.m_I32 = A
				}
			}
		},
		TSInstruction {
			.m_Operation = PUSH,
			.m_Value = TSValue {
				.m_Type = INT32_T,
				.m_Data = TSData {
					.m_I32 = B
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
	vmInstance->Run (); //Should print "5"
	delete vmInstance;
}