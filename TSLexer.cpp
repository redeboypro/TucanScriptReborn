#include "TSLexer.h"

#define IS_UNSIGNED_SUFIX(CHAR) ((CHAR) == 'u' or (CHAR) == 'U')
#define IS_LONG_SUFIX(CHAR) ((CHAR) == 'l' or (CHAR) == 'L')
#define IS_FLOAT_SUFIX(CHAR)  ((CHAR) == 'f' or (CHAR) == 'F')
#define IS_DOUBLE_SUFIX(CHAR) ((CHAR) == 'd' or (CHAR) == 'D')

Boolean StartsWithDigit (const String& str) {
	if (str.empty ()) {
		return false;
	}
	return std::isdigit (First (str));
}

Undef ProcNumericSuffix (SInt8 source, TSTokenType& type) {
	if (IS_FLOAT_SUFIX(source)) {
		type = TSTokenType::FLOAT32;
	} else if (IS_DOUBLE_SUFIX (source)) {
		type = TSTokenType::FLOAT64;
	} else if (IS_LONG_SUFIX (source)) {
		if (type == TSTokenType::UINT32) {
			type = TSTokenType::UINT64;
		} else {
			type = TSTokenType::INT64;
		}
	} else if (IS_UNSIGNED_SUFIX (source)) {
		type = TSTokenType::UINT32;
	}
}

SInt32 GetPrecedence (const TSTokenType& type) {
	switch (type) {
		//Logic
		case TSTokenType::AND:
		case TSTokenType::OR:
		return 1;

		//Comparison
		case TSTokenType::CMPE:
		case TSTokenType::CMPGE:
		case TSTokenType::CMPLE:
		case TSTokenType::CMPG:
		case TSTokenType::CMPL:
		return 2;

		//Math lower priority
		case TSTokenType::PLUS:
		case TSTokenType::MINUS:
		return 3;

		//Math higher priority
		case TSTokenType::MUL:
		case TSTokenType::DIV:
		case TSTokenType::PERCENT:
		return 4;

		//Other
		default:
		return 0;
	}
}

TSToken CreateToken (const TSTokenValue& value, const TSTokenType type) {
	if (const String* variantStr = std::get_if<String> (&value)) {
		static TSTokenType givenType;
		auto& tokenStr = *variantStr;

		if (IsTokenReservedWord (tokenStr, givenType)) {
			return TSToken {
				.m_Value = TSTokenValue(),
				.m_Type = givenType
			};
		}

		Boolean isDecimal = false;
		TSTokenType numericType = TSTokenType::INT32;
		String serializedToken;
		if (StartsWithDigit (tokenStr)) {
			for (auto curChar : tokenStr) {
				if (std::isdigit (curChar)) {
					serializedToken += curChar;
				} else if (IsDot (curChar) and !isDecimal) {
					serializedToken += curChar;
					numericType = TSTokenType::FLOAT64;
					isDecimal = true;
				} else {
					ProcNumericSuffix (curChar, numericType);
				}
			}

			TSTokenValue tokenValue;
			switch (numericType) {
				case TSTokenType::INT32: {
					ParseAndApplyNumeric<SInt32> (tokenStr, tokenValue);
				}
				case TSTokenType::INT64: {
					ParseAndApplyNumeric<SInt64> (tokenStr, tokenValue);
				}
				case TSTokenType::UINT32: {
					ParseAndApplyNumeric<UInt32> (tokenStr, tokenValue);
				}
				case TSTokenType::UINT64: {
					ParseAndApplyNumeric<UInt64> (tokenStr, tokenValue);
				}
				case TSTokenType::FLOAT32: {
					ParseAndApplyNumeric<Dec32> (tokenStr, tokenValue);
				}
				case TSTokenType::FLOAT64: {
					ParseAndApplyNumeric<Dec64> (tokenStr, tokenValue);
				}
			}

			return TSToken {
				.m_Value = value,
				.m_Type = type
			};
		}
	}

	return TSToken {
		.m_Value = value,
		.m_Type = type
	};
}

Boolean IsTokenReservedSingleChar (SInt8 source, TSTokenType& type) {
	return TryGetDictionaryValue (ReservedSingleCharMap, source, type);
}

Boolean IsTokenReservedWord (const String& source, TSTokenType& type) {
	return TryGetDictionaryValue (ReservedWordMap, source, type);
}

TSTokens Tokenize (const String& source) {
	String tokenStr;

	auto curType  = TSTokenType::NONE;
	auto nextType = TSTokenType::NONE;

	TSTokens rawTokens;

	for (UInt64 iChar = 0; iChar < source.size(); ++iChar) {
		SInt8 curChar = source[iChar];

		if (curChar == SHARP_CHAR) {
			while (curChar != '\n' && curChar != '\r') {
				curChar = source[++iChar];
			}
			continue;
		}

		if (std::isspace (curChar)) {
			if (!tokenStr.empty ()) {
				rawTokens.push_back (std::move (CreateToken (tokenStr, TSTokenType::UNDEFINED)));
				tokenStr.clear ();
			}
			continue;
		}

		if (IsTokenReservedSingleChar (curChar, curType)) {
			if (!tokenStr.empty ()) {
				rawTokens.push_back (std::move (CreateToken (tokenStr, TSTokenType::UNDEFINED)));
				tokenStr.clear ();
			}

			if (iChar < source.size () - 1) {
				auto nextChar = source[iChar + 1];
				if (IsTokenReservedSingleChar (nextChar, nextType)) {
					if (nextType == TSTokenType::CPY) {
						switch (curType) {
							case TSTokenType::CPY:
							rawTokens.push_back (std::move (CreateToken (NULL, TSTokenType::CMPE)));
							break;
							case TSTokenType::NOT:
							rawTokens.push_back (std::move (CreateToken (NULL, TSTokenType::NOT_EQ)));
							break;
							case TSTokenType::CMPG:
							rawTokens.push_back (std::move (CreateToken (NULL, TSTokenType::CMPGE)));
							break;
							case TSTokenType::CMPL:
							rawTokens.push_back (std::move (CreateToken (NULL, TSTokenType::CMPLE)));
							break;
							case TSTokenType::PLUS:
							rawTokens.push_back (std::move (CreateToken (NULL, TSTokenType::PLUS_EQ)));
							break;
							case TSTokenType::MINUS:
							rawTokens.push_back (std::move (CreateToken (NULL, TSTokenType::MINUS_EQ)));
							break;
							case TSTokenType::PERCENT:
							rawTokens.push_back (std::move (CreateToken (NULL, TSTokenType::PERCENT_EQ)));
							break;
							case TSTokenType::MUL:
							rawTokens.push_back (std::move (CreateToken (NULL, TSTokenType::MUL_EQ)));
							break;
							case TSTokenType::DIV:
							rawTokens.push_back (std::move (CreateToken (NULL, TSTokenType::DIV_EQ)));
							break;
						}
						iChar++;
						continue;
					}
				}
				rawTokens.push_back (std::move (CreateToken (NULL, curType)));
				continue;
			}
		}

		if (curChar == QUOTE_CHAR) {
			iChar++;
			auto strValueUnit = source[iChar];
			while (strValueUnit != QUOTE_CHAR) {
				if (strValueUnit == SLASH_CHAR) {
					switch (source[++iChar]) {
						case SLASH_CHAR:
						tokenStr += SLASH_CHAR;
						break;
						case QUOTE_CHAR:
						tokenStr += QUOTE_CHAR;
						break;
						case 'n':
						tokenStr += '\n';
						break;
					}
				}
				else {
					tokenStr += strValueUnit;
				}
				strValueUnit = source[++iChar];
			}
			rawTokens.push_back (std::move (CreateToken (tokenStr, TSTokenType::STR)));
		}

		if (!std::iscntrl (static_cast<UInt8>(curChar))) {
			tokenStr += curChar;
		}
	}

	if (!tokenStr.empty ()) {
		rawTokens.push_back (std::move (CreateToken (tokenStr, TSTokenType::UNDEFINED)));
	}

	return rawTokens;
}