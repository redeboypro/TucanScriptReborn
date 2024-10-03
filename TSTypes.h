#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <type_traits>

#define Generic template
#define Requires requires
#define Type typename

constexpr auto DOT_CHAR = '.';
constexpr auto QUOTE_CHAR = '"';
constexpr auto SHARP_CHAR = '#';
constexpr auto SLASH_CHAR = '\\';

struct TSToken;

typedef std::string String;

typedef signed char      SInt8;
typedef signed short     SInt16;
typedef signed int       SInt32;
typedef signed long long SInt64;

typedef unsigned char      UInt8;
typedef unsigned short     UInt16;
typedef unsigned int       UInt32;
typedef unsigned long long UInt64;

typedef float  Dec32;
typedef double Dec64;

typedef void Undef;
typedef bool Boolean;

typedef std::istringstream IStrStream;

Generic <Type KEY, Type VALUE>
using Dictionary = std::unordered_map<KEY, VALUE>;

Generic <Type ...T>
using Variant = std::variant<T...>;

Generic <Type T>
using Vector = std::vector<T>;

Generic<Type T>
Boolean TryParse (const String& str, T& value) {
	IStrStream iStrStream (str);
	iStrStream >> value;
	return !iStrStream.fail ();
}

Generic<Type KEY, Type VALUE>
Boolean TryGetDictionaryValue (const Dictionary<KEY, VALUE>& dictionary, const KEY& key, VALUE& value) {
	auto it = dictionary.find (key);
	if (it != dictionary.end ()) {
		value = it->second;
		return true;
	}
	return false;
}

Generic <Type TYPE>
Requires (std::is_floating_point<TYPE>::value)
Boolean IsDecimal (const String& str, TYPE& value) {
	return TryParse (str, value) && str.find (DOT_CHAR) != std::string::npos;
}

#endif