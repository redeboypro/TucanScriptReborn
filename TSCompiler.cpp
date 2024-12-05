#include "TSCompiler.h"

Undef TSPostProcessor::DefineMembers (TSTokens rawTokens) {
	
}

Undef TSPostProcessor::GenerateInstructions (TSTokens rawTokens) {
	//for (SInt32 iToken = 0; iToken < rawTokens.size(); iToken++) {
	//	auto& curToken = rawTokens[iToken];
	//	
	//	switch (curToken.m_Type) {
	//		case TSTokenType::UNDEFINED: {
	//			Pro
	//		}
	//	}
	//}
}

Undef TSPostProcessor::ProcExpression (TSTokens expressionTokens) {
	Stack<SInt32> rawTokenStack;
	TSTokens readyTokens;

	for (SInt32 iToken = 0; iToken < expressionTokens.size(); iToken++) {
		auto& token = expressionTokens[iToken];
		auto tokenType = token.m_Type;

		switch (tokenType) {
			CASE_OP: {
			readyTokens.push_back (token);
				break;
			}
			default: {
				if (INCLUDE (tokenType, TSTokenType::CPY, TSTokenType::OR)) {
					while (!rawTokenStack.empty()) {
						auto topId = rawTokenStack.top ();
						auto& topToken = expressionTokens[topId];

						if (!INCLUDE (topToken.m_Type, TSTokenType::CPY, TSTokenType::OR) ||
							Precedence (tokenType) > Precedence (topToken.m_Type))
							break;

						readyTokens.push_back (topToken);
						if (topToken.m_Type == TSTokenType::CPY)
							if (const SInt32* fixedPtr = std::get_if<SInt32> (&topToken.m_Value)) {
								readyTokens.push_back (TSToken {
									.m_Value = *fixedPtr,
									.m_Type = TSTokenType::VAR
								});
							}

						rawTokenStack.pop ();
					}

					rawTokenStack.push (iToken);
				}
				else {
					if (LPAREN(tokenType)) {
						rawTokenStack.push (iToken);
					}
					else if (RPAREN(tokenType)) {
						while (!rawTokenStack.empty ()) {
							auto topId = rawTokenStack.top ();
							auto& topToken = expressionTokens[topId];
							
							if (LPAREN (topToken.m_Type))
								break;

							readyTokens.push_back (topToken);
							rawTokenStack.pop ();
						}

						if (rawTokenStack.empty ()) {
							std::cerr << "Mismatched parentheses!" << std::endl;
						}

						rawTokenStack.pop ();
					}
				}
			}
		}
	}

	while (!rawTokenStack.empty ()) {
		auto topId = rawTokenStack.top ();
		auto& topToken = expressionTokens[topId];

		if (LPAREN (topToken.m_Type) || RPAREN (topToken.m_Type)) {
			std::cerr << "Mismatched parentheses!" << std::endl;
		}

		readyTokens.push_back (topToken);
		rawTokenStack.pop ();
	}

	for (auto& token : readyTokens) {
		if (token.m_Type == TSTokenType::INT32) {
			if (const SInt32* value = std::get_if<SInt32> (&token.m_Value)) {
				std::cout << *value << " INT" << std::endl;
			}
		}
		else {
			std::cout << static_cast<SInt32>(token.m_Type) << std::endl;
		}
	}
}

