cmake_minimum_required(VERSION 3.26)

project ("TucanScript")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tucanlib SHARED
			TSVirtualMachine.cpp
			TSVirtualMachine.h
			TSLexer.h "TSLexer.cpp" "TSTypes.h" "TSCompiler.h" "TSCompiler.cpp")

add_executable(tucantest
	"test/main.cpp"
	TSVirtualMachine.cpp
	TSVirtualMachine.h
	TSLexer.h "TSLexer.cpp" "TSTypes.h" "TSCompiler.h" "TSCompiler.cpp")