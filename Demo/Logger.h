// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: 
//
#pragma once

#include <sstream>

// levels of logging
enum LogLevel_t
{
	logError = 0,
	logWarning,
	logInfo,
	logDebug,
	logDebug1,
	logDebug2,
	logDebug3,
};

template<typename T>
class Logger {
public:
	Logger( void );
	virtual ~Logger( void );

	std::ostringstream&	GetMsg(LogLevel_t level = logInfo);
	// sets the level of logging
	static LogLevel_t&	ReportingLevel( void );
	
	static const char*	LevelToString(LogLevel_t level);
	static LogLevel_t	LevelFromString(const char* ch_level);

protected:
	std::ostringstream m_oss;
private:
	Logger(const Logger&);
	Logger& operator = (const Logger&);
};

/* 
	This class is responsible for implementing the output messages
	and their parameters
*/
class OutStream {
public:
	// initializes the log file
	static bool		InitLogFile(const char* nameFileLog);
	// outputs message to a file
	static bool		UseFile( void );
	// outputs message to a console
	static bool		UseStderr( void );

	static bool&	IsOutputTime( void );
};

typedef Logger<OutStream> Log;


#define LOG_FILE_INIT( x )		OutStream::InitLogFile(x)

#define LOG_IS_OUTPUT_TIME( x )	OutStream::IsOutputTime() = x

#define LOG_USE_FILE			OutStream::UseFile()

#define LOG_USE_STDERR			OutStream::UseStderr()

#define STRINGIFY( X ) #X
#define TOSTRING( X ) STRINGIFY( X )
#define AT __FUNCTION__ << " : " << __FILE__ << " : " << TOSTRING( __LINE__ )

#define TO_LOG(level) \
	if (level > logDebug3); \
	else if(level  > Log::ReportingLevel()); \
	else Log().GetMsg(level)

#define LOG_ERROR		TO_LOG( logError ) << AT << "\n   "
#define LOG_WARNING		TO_LOG( logWarning ) << AT << "\n   "
#define LOG_INFO		TO_LOG( logInfo )

#define LOG_DEBUG		TO_LOG( logDebug )
#define LOG_DEBUG_1		TO_LOG( logDebug1 )
#define LOG_DEBUG_2		TO_LOG( logDebug2 )
#define LOG_DEBUG_3		TO_LOG( logDebug3 )
