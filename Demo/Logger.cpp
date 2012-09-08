/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose:
/*
/****************************************************************************/
#include "Logger.h"

#include <time.h>

std::string NowTime( void );

static std::string s_logFileName;

class C_OutStream : public OutStream {
public:
	static void OutputFile(const char* msg);
	static void OutputStdErr(const char* msg);
};

typedef void (*fOutput)(const char*);

// use stderr as default
fOutput pfOut = &C_OutStream::OutputStdErr;

static const char* const g_cLevelString[] = 
{
	"ERROR", "WARNING", "INFO",
	"DEBUG", "DEBUG1", "DEBUG2", "DEBUG3"
};

inline void C_OutStream::OutputFile(const char* msg)
{   
	FILE* pStream = fopen(s_logFileName.c_str(), "a");

	if (!pStream)
	{
		OutputStdErr("\n\tINIT_FILE_LOG !\n");
		return;
	}

	fprintf(pStream, "%s", msg);
	fflush(pStream);
	fclose(pStream);
}

inline void C_OutStream::OutputStdErr(const char* msg)
{   
	FILE* pStream = stderr;
	if (!pStream)
		return;

	fprintf(pStream, "%s", msg);
	fflush(pStream);
}

bool OutStream::InitLogFile(const char* logName)
{
	FILE* pFile = fopen(logName, "w");

	if (!pFile)
		return false;

	time_t t;
	time ( &t );
	struct tm * timeinfo = localtime ( &t );
	fprintf(pFile, "%s", "\tLog created: ");
	char result[100] = {0};
	std::sprintf(result,"%s",asctime( timeinfo ));
	fprintf(pFile, "%s", result);
	fclose(pFile);

	pfOut = &C_OutStream::OutputFile;

	s_logFileName.clear();
	s_logFileName = logName;

	return true;
}

bool OutStream::UseFile( void )
{
	pfOut = &C_OutStream::OutputFile;

	return true;
}

bool OutStream::UseStderr( void )
{
	pfOut = &C_OutStream::OutputStdErr;

	return true;
}

bool& OutStream::IsOutputTime( void )
{
	static bool s_bIsOutTime = false;
	return s_bIsOutTime;
}

template<>
Logger<OutStream>::Logger( void )
{   }

template<>
Logger<OutStream>::~Logger( void )
{
	m_oss << std::endl;

	(*pfOut)(m_oss.str().c_str());
}

template <>
std::ostringstream& Logger<OutStream>::GetMsg( LogLevel_t level )
{
	m_oss << " > " << g_cLevelString[level];

	if (OutStream::IsOutputTime() == true)
		m_oss << "(" << NowTime() << ")";

	m_oss << " : ";

	return m_oss;
}

template <>
LogLevel_t& Logger<OutStream>::ReportingLevel()
{
	static LogLevel_t reportingLevel = logDebug3;
	return reportingLevel;
}

template <>
const char* Logger<OutStream>::LevelToString( LogLevel_t level )
{
	return g_cLevelString[level];
}

template <>
LogLevel_t Logger<OutStream>::LevelFromString(const char* ch_level)
{
	std::string cmp_str = ch_level;

	if (cmp_str == "DEBUG3")
		return logDebug3;
	if (cmp_str == "DEBUG2")
		return logDebug2;
	if (cmp_str == "DEBUG1")
		return logDebug1;
	if (cmp_str == "DEBUG")
		return logDebug;
	if (cmp_str == "INFO")
		return logInfo;
	if (cmp_str == "WARNING")
		return logWarning;
	if (cmp_str == "ERROR")
		return logError;

	Logger<OutStream>().GetMsg(logWarning) << "Unknown logging level '" <<
						ch_level << "'. Using INFO level as default.";
	return logInfo;
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <windows.h>

inline std::string NowTime( void )
{
	const int MAX_LEN = 200;
	char buffer[MAX_LEN];

	if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, 
		"HH':'mm':'ss", buffer, MAX_LEN) == 0)
		return "Error in NowTime()";

	char result[100] = {0};
	static DWORD first = GetTickCount();

	std::sprintf(result, "%s.%03ld", buffer, (GetTickCount() - first) % 1000); 

	return result;
}

#else

#include <sys/time.h>

inline std::string NowTime()
{
	char buffer[11];

	time_t t;
	time(&t);
	tm r = {0};
	strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
	struct timeval tv;
	gettimeofday(&tv, 0);
	char result[100] = {0};
	std::sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000); 
	return result;
}
#endif //WIN32
