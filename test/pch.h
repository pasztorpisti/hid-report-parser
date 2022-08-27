// precompiled header file
#pragma once


// enabling the debug heap functions of Visual C++
#ifdef _MSC_VER
#  define _CRTDBG_MAP_ALLOC
#  include <stdlib.h>
#  include <crtdbg.h>
#endif

#include <stdio.h>


#ifdef _MSC_VER


// Memory leak detector built on top of the Microsoft Visual C/C++ runtime libs.
class MemoryLeakDetector {
	const char* _name;
	_CrtMemState _start_state;
public:
	// Create a MemoryLeakDetector instance at the top of the function
	// (or any other smaller block/scope) that you want to check for leaks.
	// When the MemoryLeakDetector instance goes out of scope it dumps any
	// memory leaks created since the instantiation of the leak detector object.
	// Leak logs look like this:
	//
	//     {34757} normal block at 0x00000202B23545E0, 8 bytes long.
	//     Data : <        > CD CD CD CD CD CD CD CD
	//
	// The number between the curly brackets (34757 in this example) can be used
	// as a break_alloc parameter. If you pass it to the constructor of the
	// MemoryLeakDetector and re-run the program it creates a debug breakpoint
	// where the leaky allocation happens. This of course works only when the
	// execution and allocation order are deterministic but that's often the
	// case. The amount of executed code and the number of allocations can be
	// reduced by running only the leaky test with the help of the
	// --gtest_filter commandline parameter.
	MemoryLeakDetector(long break_alloc=-1, const char* name=nullptr) {
		_name = name;
		_CrtMemCheckpoint(&_start_state);
		if (break_alloc >= 0)
			_CrtSetBreakAlloc(break_alloc);
	}
	~MemoryLeakDetector() {
		_CrtMemState finish_state, diff;
		_CrtMemCheckpoint(&finish_state);
		if (!_CrtMemDifference(&diff, &_start_state, &finish_state))
			return;

		// By default the _CrtMemDumpStatistics and _CrtMemDumpAllObjectsSince
		// functions send the output to the debugger's output console.
		// The lines below switch the output to the console of the application
		// where google test is also logging. This way it's easy to tell which
		// test the leak dump belongs to.
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);

		// Naming a leak detector instance can help when you are checking a
		// smaller scope/code-block for leaks.
		if (_name)
			fprintf(stderr, "=== %s === begin memory leak dump\n", _name);

		_CrtMemDumpStatistics(&diff);
		_CrtMemDumpAllObjectsSince(&_start_state);

		if (_name)
			fprintf(stderr, "=== %s === end memory leak dump\n", _name);
	}
};


#else // #ifdef _MSC_VER


class MemoryLeakDetector {
public:
	MemoryLeakDetector(long break_alloc=-1, const char* name=nullptr) {}
};


#endif // #ifdef _MSC_VER


#include "gtest/gtest.h"
