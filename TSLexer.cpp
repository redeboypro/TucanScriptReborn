#include "TSLexer.h"

TSToken CreateToken (const TSTokenValue& value, const TSTokenType type) {
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

	TSTokens tokens;

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
				tokens.push_back (std::move (CreateToken (tokenStr, TSTokenType::UNDEFINED)));
				tokenStr.clear ();
			}
			continue;
		}

		if (IsTokenReservedSingleChar (curChar, curType)) {
			if (!tokenStr.empty ()) {
				tokens.push_back (std::move (CreateToken (tokenStr, TSTokenType::UNDEFINED)));
				tokenStr.clear ();
			}

			if (iChar < source.size () - 1) {
				auto nextChar = source[iChar + 1];
				if (IsTokenReservedSingleChar (nextChar, nextType)) {
					if (nextType == TSTokenType::CPY) {
						switch (curType) {
							case TSTokenType::CPY:
							tokens.push_back (std::move (CreateToken (NULL, TSTokenType::CMPE)));
							break;
							case TSTokenType::NOT:
							tokens.push_back (std::move (CreateToken (NULL, TSTokenType::NOT_EQ)));
							break;
							case TSTokenType::CMPG:
							tokens.push_back (std::move (CreateToken (NULL, TSTokenType::CMPGE)));
							break;
							case TSTokenType::CMPL:
							tokens.push_back (std::move (CreateToken (NULL, TSTokenType::CMPLE)));
							break;
							case TSTokenType::PLUS:
							tokens.push_back (std::move (CreateToken (NULL, TSTokenType::PLUS_EQ)));
							break;
							case TSTokenType::MINUS:
							tokens.push_back (std::move (CreateToken (NULL, TSTokenType::MINUS_EQ)));
							break;
							case TSTokenType::PERCENT:
							tokens.push_back (std::move (CreateToken (NULL, TSTokenType::PERCENT_EQ)));
							break;
							case TSTokenType::MUL:
							tokens.push_back (std::move (CreateToken (NULL, TSTokenType::MUL_EQ)));
							break;
							case TSTokenType::DIV:
							tokens.push_back (std::move (CreateToken (NULL, TSTokenType::DIV_EQ)));
							break;
						}
						iChar++;
						continue;
					}
				}
				tokens.push_back (std::move (CreateToken (NULL, curType)));
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
			tokens.push_back (std::move (CreateToken (tokenStr, curType)));
		}

		if (!std::iscntrl (static_cast<UInt8>(curChar))) {
			tokenStr += curChar;
		}

		if (!tokenStr.empty ()) {
			tokens.push_back (std::move (CreateToken (tokenStr, TSTokenType::UNDEFINED)));
		}
	}

	return tokens;
}

TSToken ParseToken (const String& source) {
	Dec64 ;
	Dec64 dec64val;

	if (IsDecimal (source)) {
		if (TryParse<Dec64> (source, dec64_v)) {

		}
	}
	else {

	}

	return CreateToken (source, TSTokenType::UNDEFINED);
}