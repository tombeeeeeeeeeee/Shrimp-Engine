#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <set>
#include <regex>
#include <Windows.h>
#include <cmath>
#include <unordered_map>
#include <map>
#include <StringBuilder.h>

typedef std::string string;

enum Colour
{
	darkBlue = 1,
	darkGreen = 2,
	darkCyan = 3,
	darkRed = 4,
	darkMagenta = 5,
	darkYellow = 6,
	lightGrey = 7,
	grey = 8,
	blue = 9,
	green = 10,
	cyan = 11,
	red = 12,
	magenta = 13,
	yellow = 14,
	white = 15
};

/// <summary>
/// Converts a file into a string
/// </summary>
/// <param name="path"> file path for text</param>
/// <param name="file"> </param>
/// <param name="fileToConsole">whether the string should be outputted to the console</param>
/// <returns> string read from the file</returns>
bool FileToString(string path, string* file, bool fileToConsole = true);

/// <summary>
/// Converts a file into a string vector, seperating at each line;
/// </summary>
/// <param name="path"> file path for text</param>
/// <param name="fileToConsole">whether the string should be outputted to the console</param>
/// <returns> vector of strings read from the file</returns>
bool FileToStringVector(string path, std::vector<string>* file, bool fileToConsole = true);

/// <summary>
/// Splits a string at provided characters.
/// </summary>
/// <param name="toSplit">string to split</param>
/// <param name="splitAt">characters to split at</param>
/// <returns>vector of split strings</returns>
std::vector<string> StringSplit(string toSplit, string splitAt);

/// <summary>
/// Returns a number from a string, ignores all non digits
/// </summary>
/// <param name="str">string</param>
/// <returns>number from string</returns>
bool StringToNumber(string str, int* result);

/// <summary>
/// returns a vector of ints, converted from strings
/// </summary>
/// <param name="str">vector of strings</param>
/// <returns>a vector of ints, any string with no digits will be ignored</returns>
bool StringVectorToIntVector(std::vector<string> str, std::vector<int>* ints);

/// <summary>
/// Outputs string into the console with a colour
/// </summary>
/// <param name="output">Output string</param>
/// <param name="colour">Desired colour</param>
template <typename T> void ColouredOutput(T output, Colour colour, bool newLine = true)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, colour);
	std::cout << output;
	SetConsoleTextAttribute(console, 15);
	if (newLine) std::cout << std::endl;
};


