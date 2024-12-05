#ifndef LEXER_H
#define LEXER_H

#include "TSTypes.h"
#include <iostream>

struct TSToken;

typedef std::vector<TSToken> TSTokens;

enum class TSTokenType : SInt32 {
	NONE,
	UNDEFINED,
	
	STR,
	CHAR,
	BYTE,
	UINT16,
	UINT32,
	UINT64,
	INT16,
	INT32,
	INT64,
	FLOAT32,
	FLOAT64,

	STR_C,
	CHAR_C,
	BYTE_C,
	UINT16_C,
	UINT32_C,
	UINT64_C,
	INT16_C,
	INT32_C,
	INT64_C,
	FLOAT32_C,
	FLOAT64_C,

	LPAREN,
	RPAREN,

	CPY,
	CMPE,
	CMPG,
	CMPL,
	CMPGE,
	CMPLE,
	NOT,
	NOT_EQ,

	PLUS_EQ,
	MINUS_EQ,
	MUL_EQ,
	DIV_EQ,
	PERCENT_EQ,
	PLUS,
	MINUS,
	PERCENT,
	MUL,
	DIV,

	AND,
	OR,

	IF,
	WHILE,
	FOR,
	FROM,

	DEF,
	IMP,
	REF,

	SEMICOLON,
	COMMA,

	LBRACE,
	RBRACE,

	BREAK,
	CONTINUE,
	RETURN,

	VAR,
	ARRAY,
};

static const Dictionary<SInt8, TSTokenType> ReservedSingleCharMap {
	{ '=', TSTokenType::CPY },
	{ '*', TSTokenType::MUL },
	{ '/', TSTokenType::DIV },
	{ '%', TSTokenType::PERCENT },
	{ '+', TSTokenType::PLUS },
	{ '-', TSTokenType::MINUS },
	{ '>', TSTokenType::CMPG },
	{ '<', TSTokenType::CMPL },
	{ '(', TSTokenType::LPAREN },
	{ ')', TSTokenType::RPAREN },
	{ '{', TSTokenType::LBRACE },
	{ '}', TSTokenType::RBRACE },
	{ ';', TSTokenType::SEMICOLON },
	{ ',', TSTokenType::COMMA },
	{ '&', TSTokenType::REF },
	{ '!', TSTokenType::NOT }
};

static const Dictionary<String, TSTokenType> ReservedWordMap =
{
	{ "and",      TSTokenType::AND },
	{ "or",       TSTokenType::OR },
	{ "if",       TSTokenType::IF },
	{ "while",    TSTokenType::WHILE },
	{ "def",      TSTokenType::DEF },
	{ "imp",      TSTokenType::IMP },
	{ "for",      TSTokenType::FOR },
	{ "from",     TSTokenType::FROM },
	{ "break",    TSTokenType::BREAK },
	{ "continue", TSTokenType::CONTINUE },
	{ "return",   TSTokenType::RETURN },

	{ "uchar",    TSTokenType::BYTE_C },
	{ "ushort",   TSTokenType::UINT16_C },
	{ "uint",     TSTokenType::UINT32_C },
	{ "ulong",    TSTokenType::UINT64_C },

	{ "char",     TSTokenType::CHAR_C },
	{ "short",    TSTokenType::INT16_C },
	{ "int",      TSTokenType::INT32_C },
	{ "long",     TSTokenType::INT64_C },

	{ "float",    TSTokenType::FLOAT32_C},
	{ "double",   TSTokenType::FLOAT64_C}
};

using TSTokenValue = Variant<
	String,
	SInt8, SInt16, SInt32, SInt64,
	UInt8, UInt16, UInt32, UInt64,
	Dec32, Dec64>;

struct TSToken {
	TSTokenValue m_Value;
	TSTokenType  m_Type;
};


Boolean StartsWithDigit (const String& str);
Undef ProcNumericSuffix (SInt8 source, TSTokenType& type);

Generic<Type T>
Undef ParseAndApplyNumeric (const String& tokenStr, TSTokenValue& tokenValue) {
	T result {};
	if (TryParse<T> (tokenStr, result)) {
		tokenValue = result;
	} else {
		std::cerr << "Cannot parse not numeric token!" << std::endl;
	}
}

TSToken CreateToken (const TSTokenValue& value, const TSTokenType type);

Boolean IsTokenReservedSingleChar (SInt8 source, TSTokenType& type);
Boolean IsTokenReservedWord (const String& source, TSTokenType& type);

TSTokens Tokenize (const String& source);

#endif