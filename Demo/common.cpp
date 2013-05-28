// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: 
//
#include "common.h"

#include <stdio.h>
#include <time.h>

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

extern GLenum g_GLError = GL_NO_ERROR;

bool InitGLEW( void )
{
	glewExperimental = GL_TRUE;

	if( glewInit() != GLEW_OK )
		return false;

	return true;
}

void PrintVideoInfo()
{
	GLint majorGL,minorGL;
	GLint max_buffer,max_color_tex,max_depth_tex,max_samples,max_color_attachment;

	fprintf(stderr,"\n : OpenGL Render Context Info : \n");
	fprintf(stderr,"\n  Renderer....... %s", (const char*)glGetString( GL_RENDERER ));
	fprintf(stderr,"\n  Vendor......... %s", (const char*)glGetString( GL_VENDOR ));
	fprintf(stderr,"\n  Version........ %s", (const char*)glGetString( GL_VERSION ));
	fprintf(stderr,"\n  GLSL version... %s", (const char*)glGetString( GL_SHADING_LANGUAGE_VERSION ));

	glGetIntegerv(GL_MAJOR_VERSION, &majorGL);
	glGetIntegerv(GL_MINOR_VERSION, &minorGL);

	fprintf(stderr,"\n  OpenGL version. %d.%d", majorGL, minorGL);

	if ( majorGL < 3 ) {
		fprintf(stderr,"\n Sorry, it seems, your hardware does not support OpenGL 3 or you need update the video driver.");
		return;
	}

	fprintf(stderr,"\n\n  : Video card capabilities : ");
	glGetIntegerv( GL_MAX_DRAW_BUFFERS, &max_buffer );
	fprintf(stderr,"\n  Max draw buffers.......... %d", max_buffer);
	glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &max_color_attachment);
	fprintf(stderr,"\n  Max color attachments..... %d", max_color_attachment);
	glGetIntegerv( GL_MAX_SAMPLES, &max_samples );
	fprintf(stderr,"\n  Max samples............... %d", max_samples);
	glGetIntegerv( GL_MAX_COLOR_TEXTURE_SAMPLES, &max_color_tex);
	fprintf(stderr,"\n  Max color texture samples. %d", max_color_tex); 
	glGetIntegerv( GL_MAX_COLOR_TEXTURE_SAMPLES, &max_depth_tex);
	fprintf(stderr,"\n  Max depth texture samples. %d", max_depth_tex);
}

char* const ReadWholeFileIntoString(const char *fileName, int *lenFile)
{
	if (!fileName)
		return nullptr;
	FILE* pFile = fopen(fileName, "rb");

	if (!pFile)
	{
		fclose(pFile);
		return nullptr;
	}

	// size of the file
	fseek(pFile, 0, SEEK_END);
	const int _size = ftell(pFile);

	char* pBuff = (char*)malloc(_size + 1);
	assert(pBuff);

	// file pointer at the beginning of the file
	rewind(pFile);
	fread(pBuff, sizeof(char), _size, pFile);
	fclose(pFile);
	pBuff[_size] = '\0';

	if (lenFile)
		*lenFile = _size;

	return pBuff;
}

// only for the Mac OS
bool SetWorkingDirectory(void)
{
#ifdef __APPLE__
	CFBundleRef mainBundle = CFBundleGetMainBundle();
   
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	
	char path[PATH_MAX];
	
	if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
	{
		LOG_USE_STDERR;
		LOG_ERROR << "/nWrong working directory !/n";
		return false;
	}
	
	CFRelease(resourcesURL);
	
	chdir(path);
#endif
	return true;
}

float GetFPS(float interval)
{
	static float time0 = 0;
	static int frameCount = 0;
	static float fps = 0.0;

	const float currTime = glfwGetTime();
	if ((currTime - time0) > interval)
	{
		fps = (float)frameCount / (currTime - time0);
		frameCount = 0;
		time0 = glfwGetTime();
	}
	else
	{
		++frameCount;
	}

	return fps;
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <windows.h>

void NowTime( std::string& str )
{
	const int MAX_LEN = 200;
	char buffer[MAX_LEN];

	if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, 
		"HH':'mm':'ss", buffer, MAX_LEN) == 0)
	{
		str = "Error in NowTime()";
	}

	char result[100] = {0};
	static DWORD first = GetTickCount();
	sprintf(result, "%s.%03ld", buffer, (GetTickCount() - first) % 1000);
	str = result;
}

#else

#include <sys/time.h>

void NowTime( std::string& str )
{
	char buffer[11];

	time_t t;
	time(&t);
	tm r = {0};
	strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
	struct timeval tv;
	gettimeofday(&tv, 0);
	char result[100] = {0};
	sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000);

	str = result;
}

#endif