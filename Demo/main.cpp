/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: Font System OpenGL 3 (FSGL3) a demo program 
/*
/****************************************************************************/

#include "common.h"

// FSGL3 header
#include "FontSystem.h"

#include <locale>
#include <codecvt>

// font descriptors
HFont	g_hDejaVuSans_10	= INVALID_FONT,
		g_hFreeSans_14		= INVALID_FONT,

		g_hVerdana_11		= INVALID_FONT,
		g_hVerdanaB_11		= INVALID_FONT,
		g_hVerdanaI_11		= INVALID_FONT,

		g_hFontCurrent		= INVALID_FONT;

//#define INIT_FROM_CACHE

#define DIR_FONT	"../base/fonts/"
#define DIR_CACHE	"../base/fontCache/"

const int g_Width = 800;
const int g_Height = 600;

const int g_halfWidth = g_Width / 2;
const int g_halfHeight = g_Height / 2;

bool g_running = true;
bool g_bChangePage = false;

int g_fileID = 0;

void events(int, int);

bool Init( void )
{
	LOG_FILE_INIT("logApp.txt");

	if(!SetWorkingDirectory()) {
		return false;
	}

	if ( !glfwInit() )
	{
		LOG_ERROR << "Init a library GLFW";
		return false;
	}

	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwOpenWindowHint( GLFW_WINDOW_NO_RESIZE, GL_TRUE );

	int is_ok = glfwOpenWindow(g_Width, g_Height, 8, 8, 8, 8, 24, 0, GLFW_WINDOW);

	if ( !is_ok )
	{
		glfwTerminate();
		LOG_ERROR << "glfwOpenWindow ";
		return false;
	}

	glfwEnable(GLFW_KEY_REPEAT);

	glfwSetWindowTitle( "Font System OpenGL 3 (FSGL3) Demo" );

	glfwSetWindowPos(200, 100);

	if( !InitGLEW() ) {
		return false;
	}

	PrintVideoInfo();

	// specifies the maximum number of characters for a static text
	SetMaxStaticChars(2048);

	// initialize FSGL3
	if ( !FontSystem().Initialize() )
	{
		LOG_ERROR << "FSGL3 Initialize";
		return false;
	}
	
	FontSystem().SetScreenSize(g_Width, g_Height);
	
	glViewport(0, 0, g_Width, g_Height);

	bool isBuild = false;

#ifndef INIT_FROM_CACHE

	// Load and create fonts
	
	g_hFreeSans_14 = FontSystem().Create_Font(DIR_FONT"FreeSans.ttf", 14);
	assert(g_hFreeSans_14 != INVALID_FONT);

	g_hDejaVuSans_10 = FontSystem().Create_Font(DIR_FONT"DejaVuSans.ttf", 10);
	assert(g_hDejaVuSans_10 != INVALID_FONT);

	g_hVerdana_11 = FontSystem().Create_Font(DIR_FONT"verdana.ttf", 11);
	assert(g_hVerdana_11 != INVALID_FONT);

	g_hVerdanaB_11 = FontSystem().Create_Font(DIR_FONT"verdanab.ttf", 11);
	assert(g_hVerdanaB_11 != INVALID_FONT);

	g_hVerdanaI_11 = FontSystem().Create_Font(DIR_FONT"verdanai.ttf", 11);
	assert(g_hVerdanaI_11 != INVALID_FONT);

	// Add character ranges
	
	// 0-127 + 1024-1279
	FontSystem().AddGlyphSetToFont(g_hFreeSans_14, 31, 127);
	FontSystem().AddGlyphSetToFont(g_hFreeSans_14, 1024, 1104);

	// only ASCI 0-127
	FontSystem().AddGlyphSetToFont(g_hDejaVuSans_10, BASIC_LATIN);

	// 0-127 + 1024-1279
	FontSystem().AddGlyphSetToFont(g_hVerdana_11, BASIC_LATIN);
	FontSystem().AddGlyphSetToFont(g_hVerdana_11, CYRILLIC);
	

	// 0-127 + 1024-1279
	FontSystem().AddGlyphSetToFont(g_hVerdanaB_11, BASIC_LATIN);
	FontSystem().AddGlyphSetToFont(g_hVerdanaB_11, CYRILLIC);

	// 1024-1279 + 0-127
	FontSystem().AddGlyphSetToFont(g_hVerdanaI_11, CYRILLIC);
	FontSystem().AddGlyphSetToFont(g_hVerdanaI_11, BASIC_LATIN);

	SetFontTextureWidth(512);

	isBuild = FontSystem().BuildAllFonts();

	if ( !isBuild )
		return false;

	FontSystem().DumpFontCache( g_hFreeSans_14, DIR_CACHE );
	FontSystem().DumpFontCache( g_hDejaVuSans_10, DIR_CACHE );
	FontSystem().DumpFontCache( g_hVerdana_11, DIR_CACHE );
	FontSystem().DumpFontCache( g_hVerdanaB_11, DIR_CACHE );
	FontSystem().DumpFontCache( g_hVerdanaI_11, DIR_CACHE );

#else

	// Load cache font data

	g_hFreeSans_14		= FontSystem().LoadFontCache(DIR_CACHE"FreeSans_14.cfnt");
	assert(g_hFreeSans_14 != INVALID_FONT);

	g_hDejaVuSans_10	= FontSystem().LoadFontCache(DIR_CACHE"DejaVuSans_10.cfnt");
	assert(g_hDejaVuSans_10 != INVALID_FONT);

	g_hVerdana_11		= FontSystem().LoadFontCache(DIR_CACHE"verdana_11.cfnt");
	assert(g_hVerdana_11 != INVALID_FONT);

	g_hVerdanaB_11		= FontSystem().LoadFontCache(DIR_CACHE"verdanab_11.cfnt");
	assert(g_hVerdanaB_11 != INVALID_FONT);

	g_hVerdanaI_11		= FontSystem().LoadFontCache(DIR_CACHE"verdanai_11.cfnt");
	assert(g_hVerdanaI_11 != INVALID_FONT);

	isBuild = FontSystem().BuildCache();

	if ( !isBuild )
		return false;

#endif

	g_hFontCurrent = g_hVerdana_11;

	return true;
}

int InitStaticTextFromFile( const char* nameFile, BBox_t &bbox )
{
	const char* pText = ReadWholeFileIntoString( nameFile );

	if ( !pText )
	{
		return -1;
	}

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	auto wstr = conv.from_bytes(pText);
	delete[] pText;

	FontSystem().BindFont(g_hFontCurrent);

	FontSystem().SetTextPos(2, 75);

	// gets the bounding box for the text from a file
	FontSystem().GetWTextBBox( wstr.c_str(), wstr.length(), bbox );

	FontSystem().SetTextColor(255, 0, 0);

	// transmit the static text
	int idText = FontSystem().SetStaticWText( wstr.c_str(), wstr.length() );

	FontSystem().BindFont(0);

	wstr.clear();

	return idText;
}

int main( int argc, char* argv[] )
{
	if ( !Init() )
	{
		exit(1);
	}

	char buffer[1024];

	int VertexPerFrame = 0;

	const char NameFile [] = "../base/text/GF_Info_";

	sprintf(buffer, "%s%d.txt", NameFile, g_fileID);

	BBox_t bboxTextFromFile;

	int idTextFromFile = 0;
	if ( (idTextFromFile = InitStaticTextFromFile(buffer, bboxTextFromFile)) == -1 ) {
		return -1;
	}

	std::string time;

	int mouseX = 0, mouseY = 0;
	int currLen = 0;

	FontSystem().BindFont(g_hFreeSans_14);

	BBox_t bboxTextLink;
	const char pLinkBlog[] = " shaman-code.blogspot.com ";

	FontSystem().SetTextPos(1, 1);
	FontSystem().GetTextBBox( pLinkBlog, strlen(pLinkBlog), bboxTextLink );
	FontSystem().SetTextColor(255, 255, 255);
	int idTextLink = FontSystem().SetStaticText( pLinkBlog, strlen(pLinkBlog) );

	NowTime(time);

	BBox_t bboxTime;

	FontSystem().SetTextPos( bboxTextLink.xMax + 2, 1 );
	FontSystem().GetTextBBox(time.c_str(), time.length(), bboxTime);

	const char pHelp[] = "F1 - F3 - change font for the text\n" \
						 "\nKey Right/Left Space - show the next/prev block of text\n" \
						 "\nCtrl - use kerning";

	FontSystem().BindFont(g_hDejaVuSans_10);

	FontSystem().SetTextPos( bboxTime.xMax + 25, 1 );
	FontSystem().SetTextColor(0, 0, 255);
	int idTextHelp = FontSystem().SetStaticText( pHelp, strlen(pHelp) );

	FontSystem().BindFont(0);

	while ( g_running )
	{
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// Begins painting (bind shader program, clear all state etc.)
		FontSystem().BeginDraw();


		// (Dynamic text) Debug info
		FontSystem().BindFont( g_hDejaVuSans_10 );

		currLen = sprintf(buffer, "Debug Info:\nFPS : %.3f\nVertexPerFrame : %i", GetFPS(1.0), VertexPerFrame);

		FontSystem().SetTextPos(1, bboxTextLink.yMax + 1);
		FontSystem().SetTextColor(255, 0, 255);
		FontSystem().PrintText(buffer, currLen);

		FontSystem().BindFont(0);


		// (Static  text) Text from File
		FontSystem().PrintStaticText( idTextFromFile );


		// (Static  text) link

		FontSystem().PrintStaticText( idTextLink );


		// (Static  text) Help 

		FontSystem().PrintStaticText( idTextHelp );


		// (Dynamic text) Current time

		// font to display the time
		FontSystem().BindFont( g_hFreeSans_14 );

		static float time0 = 0;

		float currTime = glfwGetTime();

		// time is updated once per second
		if ( (currTime - time0) > 1.0 )
		{
			NowTime(time);

			time0 = glfwGetTime();
		}

		// color of the rectangle

		FontSystem().SetTextPos( bboxTime.xMin, bboxTime.yMin );

		FontSystem().SetTextColor(0, 0, 0);
		FontSystem().PrintText(time.c_str(), time.length());

		FontSystem().BindFont(0);


		// (Dynamic text) Cursor position
		 
		FontSystem().BindFont( g_hFreeSans_14 );

		glfwGetMousePos(&mouseX, &mouseY);

		if ((mouseX < g_halfWidth) && (mouseY < g_halfHeight))
		{
			FontSystem().SetTextPos( mouseX+15, mouseY+10 );
		} 
		else if ((mouseX > g_halfWidth) && (mouseY < g_halfHeight))
		{
			FontSystem().SetTextPos( mouseX-100, mouseY+10 );
		} 
		else if ((mouseX < g_halfWidth) && (mouseY > g_halfHeight))
		{
			FontSystem().SetTextPos( mouseX+15, mouseY-20 );
		}
		else
		{
			FontSystem().SetTextPos( mouseX-100, mouseY-20 );
		}

		currLen = sprintf(buffer, "x:%i y:%i ", mouseX, mouseY);

		FontSystem().SetTextColor(255, 0, 255);
		FontSystem().PrintText(buffer, currLen);

		if ( g_bChangePage )
		{
			// Reinitializing static text

			FontSystem().ResetStaticText(); 

			if ( (g_fileID >= 3) || (g_fileID < 0))
			{
				g_fileID = 0;
			}
			
			sprintf(buffer, "%s%d.txt", NameFile, g_fileID);

			if ( (idTextFromFile = InitStaticTextFromFile(buffer, bboxTextFromFile)) == -1 )
				return -1;

			FontSystem().BindFont(g_hFreeSans_14);
			
			FontSystem().SetTextPos(1, 1);
			FontSystem().SetTextColor(255, 255, 255);
			idTextLink = FontSystem().SetStaticText( pLinkBlog, strlen(pLinkBlog) );

			FontSystem().BindFont(g_hDejaVuSans_10);

			FontSystem().SetTextPos( bboxTime.xMax + 25, 1 );
			FontSystem().SetTextColor(0, 0, 255);
			idTextHelp = FontSystem().SetStaticText( pHelp, strlen(pHelp) );

			g_bChangePage = false;
		}

		FontSystem().EndDraw();

		VertexPerFrame = FontSystem().VertexPerFrame();

		glfwSetKeyCallback( events );

		glfwSwapBuffers();
		
		g_running = !glfwGetKey(GLFW_KEY_ESC) &&
					glfwGetWindowParam( GLFW_OPENED );
	}

	glfwCloseWindow();

	FontSystem().Shutdown();

	glfwTerminate();

	return 0;
}

void events( int key, int action )
{
	switch( key )
	{
	case GLFW_KEY_SPACE:
		if ( glfwGetKey(GLFW_KEY_SPACE) == GLFW_RELEASE )
		{
			g_bChangePage = true;
			++g_fileID;
		}
		break;
	case GLFW_KEY_RIGHT:
		if ( glfwGetKey(GLFW_KEY_RIGHT) == GLFW_RELEASE )
		{
			g_bChangePage = true;
			++g_fileID;
		}
		break;
	case GLFW_KEY_LEFT:
		if ( glfwGetKey(GLFW_KEY_LEFT) == GLFW_RELEASE )
		{
			g_bChangePage = true;
			--g_fileID;
		}
		break;
	case GLFW_KEY_F1:
		if ( glfwGetKey(GLFW_KEY_F1) == GLFW_RELEASE )
		{
			g_hFontCurrent = g_hVerdana_11;
			g_bChangePage = true;
		}
		break;
	case GLFW_KEY_F2:
		if ( glfwGetKey(GLFW_KEY_F2) == GLFW_RELEASE )
		{
			g_hFontCurrent = g_hVerdanaB_11;
			g_bChangePage = true;
		}
		break;
	case GLFW_KEY_F3:
		if ( glfwGetKey(GLFW_KEY_F3) == GLFW_RELEASE )
		{
			g_hFontCurrent = g_hVerdanaI_11;
			g_bChangePage = true;
		}
		break;
	case GLFW_KEY_LCTRL:
		if ( glfwGetKey(GLFW_KEY_LCTRL) == GLFW_RELEASE )
		{
			static bool useKerning = false;
			FontSystem().UseKerning(useKerning ? useKerning = false : useKerning = true);
			g_bChangePage = true;
		}
		break;
	default:
		{
			g_hFontCurrent = g_hVerdana_11;
			g_bChangePage = false;
		}
		break;
	}
}