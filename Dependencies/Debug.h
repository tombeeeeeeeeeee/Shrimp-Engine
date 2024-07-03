#pragma once

// Author: Laochra Murray

#include <string>

#include <map>

#include <fstream>

#include "GeneralMacros.h"

#include "LineDrawer.h"

#include "StackTimer.h"

#include "StringBuilder.h"

class Debug;
extern Debug* debug;

template<typename T> concept MsgType = requires(T t) { std::same_as<decltype(ValueAsString(t)), std::string>; };
	 
class Debug
{
public:
	LineDrawer lines;

	Debug() noexcept;
	~Debug() noexcept;
	uncopyable(Debug)
	immovable(Debug)
	
	mappedenumi(LogType, unsigned char, public,
		Message,
		Warning,
		Error,
		Subtle,
	)
	
	struct DebugLog
	{
		std::string message;
		LogType type;
		DebugLog(std::string messageInit, LogType typeInit = LogType::Message) :
			message(messageInit),
			type(typeInit) {}
	};



	/// <summary>
	/// <para>Logs a message.</para>
	/// <para>As many parameters as is required can be used.</para>
	/// <param name='message/messageContinued:'>All primitive types and other types with ToString(x) overridden can be used and will be converted to strings nicely.</param>
	/// </summary>
	template<MsgType Req, MsgType... Opt>
	static DebugLog Log(Req message, Opt... messageContinued)
	{
		return debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Message);
	}

	/// <summary>
	/// <para>Logs a message subtly (certain contexts won't show subtle logs to avoid spam, but this will always show in the log file).</para>
	/// <para>As many parameters as is required can be used.</para>
	/// <param name='message/messageContinued:'>All primitive types and other types with ToString(x) overridden can be used and will be converted to strings nicely.</param>
	/// </summary>
	template<MsgType Req, MsgType... Opt>
	static DebugLog LogSubtle(Req message, Opt... messageContinued)
	{
		return debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Subtle);
	}

	/// <summary>
	/// <para>Logs a warning which appears yellow.</para>
	/// <para>As many parameters as is required can be used.</para>
	/// <para>The first parameter can optionally be a LogID which inserts a generic message based on the LogID.</para>
	/// <para>All primitive types and other types with ToString(x) overridden can be used and will be converted to strings nicely.</para>
	/// </summary>
	template<MsgType Req, MsgType... Opt>
	static DebugLog LogWarning(Req message, Opt... messageContinued)
	{
		return debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Warning);
	}

	
	/// <summary>
	/// <para>Logs an error which appears red.</para>
	/// <para>As many parameters as is required can be used.</para>
	/// <para>The first parameter can optionally be a LogID which inserts a generic message based on the LogID.</para>
	/// <para>All primitive types and other types with ToString(x) overridden can be used and will be converted to strings nicely.</para>
	/// </summary>
	template<MsgType Req, MsgType... Opt>
	static DebugLog LogError(Req message, Opt... messageContinued)
	{
		return debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Error);
	}

	static std::string GetLogAsString(const DebugLog& log) noexcept;

protected:
	std::ofstream* outputFile;

	virtual DebugLog LogImplementation(const std::string& message, const LogType type = Message);
};
