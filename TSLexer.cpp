#include "TSLexer.h"

#define IS_FLOAT_SUFIX(CHAR)  (CHAR == 'f') or (CHAR == 'F')
#define IS_DOUBLE_SUFIX(CHAR) (CHAR == 'd') or (CHAR == 'D')

Boolean IsNumericSuffix (SInt8 source, TSTokenType& type) {
	switch (source) {
		case 'f':
		case 'F':
		type = TSTokenType::FLOAT32;
		return true;
	}
}

TSToken CreateToken (const TSTokenValue& value, const TSTokenType type) {
	if (const String* variantStr = std::get_if<String> (&value)) {
		static TSTokenType givenType;
		if (IsTokenReservedWord (*variantStr, givenType)) {
			return TSToken {
				.m_Value = TSTokenValue(),
				.m_Type = givenType
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

		if (tokenStr.empty ()) {
			if (std::isdigit (curChar)) {
				tokenStr += curChar;
				TSTokenType numericType;
				while () {

				}
			}
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

	TSTokens tokens;
	for (auto& token : rawTokens) {
		if (token.m_Type == TSTokenType::UNDEFINED) {

		}
		else {

		}
	}

	return tokens;
}