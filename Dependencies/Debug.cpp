
// Author: Laochra Murray

#include "Debug.h"

#include <fstream>

#ifdef _DEBUG
#include <iostream>
#endif

#if defined(_WIN32) || defined(_WIN64)
#ifdef APIENTRY
#undef APIENTRY
#endif
#include "Windows.h"
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
#define LIGHTGREY 7
#define YELLOW 14
#define RED 12
#define SetConsoleTextColour(colour) SetConsoleTextAttribute(console, colour);
#endif

Debug* debug;

Debug::Debug() noexcept
{
	outputFile = new std::ofstream("Debug.log");
	outputFile->close();
	outputFile->open("Debug.log", std::ios::app);
}

Debug::~Debug() noexcept
{
	outputFile->close();
	delete outputFile;
}

Debug::DebugLog Debug::LogImplementation(const std::string& message, const LogType type)
{
	DebugLog log(message, type);

	const std::string logString = GetLogAsString(log) + '\n';

#ifdef _DEBUG
#if defined(_WIN32) || defined(_WIN64)
	switch (type)
	{
	case Warning:	SetConsoleTextColour(YELLOW);	break;
	case Error:		SetConsoleTextColour(RED);		break;
	}
#endif

	if (type != Subtle) std::cout << logString;

#if defined(_WIN32) || defined(_WIN64)
	SetConsoleTextColour(LIGHTGREY);
#endif
#endif

	*outputFile << logString;
	outputFile->flush();

	return log;
}

std::string Debug::GetLogAsString(const Debug::DebugLog& log) noexcept
{
	return log.message;
}
