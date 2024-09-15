cmake_minimum_required(VERSION 3.26)

project ("TucanScript")

set(CMAKE_CXX_STANDARD 20)

add_library(tucanlib SHARED
			TSVirtualMachine.cpp
			TSVirtualMachine.h)

add_executable(tucantest
	"test/main.cpp"
	TSVirtualMachine.cpp
	TSVirtualMachine.h
)