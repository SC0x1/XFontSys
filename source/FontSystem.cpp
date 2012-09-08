/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: Implementation of the IFontSystem interface
/*
/****************************************************************************/

#include "FontSystem.h"
#include "FontManager.h"
#include "Font.h"
#include "FT_Lib.h"

#include "public/utlvector.h"

#include "shaders/ShaderSystem.h"
#include "shaders/gl_local.h"

#include "public/common.h"

#include "FontGlobal.h"

// TODO: remove globals var
extern int MAX_LENGTH_STRING = 256;
extern int STATIC_CHARS = 4096;
extern int FONT_TEXTURE_WIDTH = 1024;

static const UnicodeCharRange_t g_FontRange[] =
{
	{"Base Latin", 0, 127},
	{"Latin-1 Supplement", 128, 255},
	{"Latin Extended-A", 256, 383},
	{"Latin Extended-B", 384, 591},
	{"IPA Extensions", 592, 687},
	{"Spacing Modifier Letters", 688, 767},
	{"Combining Diacritical Marks", 768, 879},
	{"Greek and Coptic", 880, 1023},
	{"Cyrillic", 1024, 1279},
	{"Cyrillic Supplementary", 1280, 1327},

	/*	...	add more */

	{"", 0, 0},
};

//-----------------------------------------------------------------------------
// Structure that store informations about the static text
//-----------------------------------------------------------------------------
struct TextInfo
{
	// first vertex in the VBO
	unsigned short	firstVertex;	
	// total number of vertices for a string
	unsigned short	countVertex;
	// text color
	float	color[4];
};

class CFontSystem : public IFontSystem
{
public:
	CFontSystem();
	~CFontSystem();

	bool Initialize( void );

	void SetScreenSize(int sWidth, int sHeight);

	void Shutdown( void );

	HFont Create_Font(const char* fontName, int size);

	bool AddGlyphSetToFont(HFont handle, CharacterSets flags);
	bool AddGlyphSetToFont(HFont handle, int lowerRange, int upperRange);
	bool BuildAllFonts( void );

	bool DumpFontCache(HFont handle, const char* path);
	HFont LoadFontCache(const char *fontName);
	bool BuildCache( void );

	int GetFontSize(HFont handle) const;

	void GetTextBBox(const char *text, const int textLen, BBox_t &bbox)
	{
		if ((m_TextLen = textLen) > STATIC_CHARS)
			return;

		GetTextArea<char>(text, bbox);
	}

	void GetWTextBBox(const wchar_t *text, const int textLen, BBox_t &bbox)
	{
		if ((m_TextLen = textLen) > STATIC_CHARS)
			return;

		GetTextArea<wchar_t>(text, bbox);
	}

	void BindFont(const HFont handle)
	{
		m_hFont = handle;
	}

	void SetColor(uint8 r, uint8 g, uint8 b, uint8 a)
	{
		m_DrawColor[0] = (float)r;
		m_DrawColor[1] = (float)g;
		m_DrawColor[2] = (float)b;
		m_DrawColor[3] = (float)a;
	}

	void SetTextColor(uint8 r, uint8 g, uint8 b, uint8 a)
	{
		m_DrawTextColor[0] = (float)r;
		m_DrawTextColor[1] = (float)g;
		m_DrawTextColor[2] = (float)b;
		m_DrawTextColor[3] = (float)a;
	}

	void SetTextPos(int posX, int posY)
	{
		m_DrawTextPos[0] = posX;
		m_DrawTextPos[1] = posY;
	}

	void SetTextPosX(int posX)
	{
		m_DrawTextPos[0] = posX;
	}

	void SetTextPosY(int posY)
	{
		m_DrawTextPos[1] = posY;
	}

	void GetTextPos(int &posX, int &posY) const
	{
		posX = m_DrawTextPos[0];
		posY = m_DrawTextPos[1];
	}

	void GetTextPosX(int &posX) const
	{
		posX = m_DrawTextPos[0];
	}

	void GetTextPosY(int &posY) const
	{
		posY = m_DrawTextPos[1];
	}

	int SetStaticText(const char *text, const int textLen)
	{
		if ((m_TextLen = textLen) > STATIC_CHARS)
			return -1;

		return BuildStaticText<char>(text);
	}

	int SetStaticWText(const wchar_t *text, const int textLen)
	{
		if ((m_TextLen = textLen) > STATIC_CHARS)
			return -1;

		return BuildStaticText<wchar_t>(text);
	}

	void ResetStaticText( void );

	void PrintStaticText(int idText);
	void PrintText(const char *text, const int textLen)
	{
		if (!text || ((m_TextLen = textLen) > MAX_LENGTH_STRING))
			return;

		if ((m_hFont <= 0) || (m_DrawTextColor[3] <= 0.1))
			return;

		pBaseVertex = m_BufferVertices;

		BuildTextVertices<char>(text);

		Draw2DText();
	}

	void PrintWText(const wchar_t *text, const int textLen)
	{
		if (!text || ((m_TextLen = textLen) > MAX_LENGTH_STRING))
			return;

		if ((m_hFont <= 0) || (m_DrawTextColor[3] <= 0.1))
			return;

		pBaseVertex = m_BufferVertices;

		BuildTextVertices<wchar_t>(text);

		Draw2DText();
	}

	// displays a rectangle specified in screen space
	void DrawOutLinedRect(const BBox_t& bbox)
	{
		DrawOutLinedRect(bbox.xMin, bbox.yMin, bbox.xMax, bbox.yMax);
	}

	void DrawFilledRect(const BBox_t& bbox)
	{
		DrawFilledRect(bbox.xMin, bbox.yMin, bbox.xMax, bbox.yMax);
	}

	void DrawFilledRect(int x0, int y0, int x1, int y1);
	void DrawOutLinedRect(int x0,int y0, int x1, int y1);

	void DrawLine(int pX0, int pY0, int pX1, int pY1);

	void EnableStateDraw( void );
	void DisableStateDraw( void );

	int VertexPerFrame( void ) const
	{
		return m_VertexPerFrame;
	}

private:

	void InitBuffers( void );

	template<typename T>
	void BuildTextVertices(const T* text);

	template<typename T>
	int BuildStaticText(const T* text);

	template<typename T>
	BBox_t GetTextArea(const T* text, BBox_t &bbox) const;

	void ClearAllState( void );

	void Draw2DText( void );

	CShaderOGL m_fontShader;

	float m_scaleX;
	float m_scaleY;

	unsigned int	m_VertexArrayId[2];		// Vertex Array Object ID
	unsigned int	m_VertexBufferId[2];	// Vertex Buffer Object ID

	// uniform id - color
	int		m_UnifColor;

	unsigned int m_TexID;

	// current number vertex for a static text
	unsigned short m_CurrStaticVertex;

	CUtlVector<TextInfo> m_StaticTextInfo;

	// amount of free space in the video memory for the vertices of the static text in bytes
	int		m_StaticFreeVMem;
	int		m_StaticTotalVMem;

	float	m_DrawColor[4];

	// color for text
	float	m_DrawTextColor[4];

	// current position in screen space
	int		m_DrawTextPos[2];

	// descriptor (id) of the current font
	HFont	m_hFont;

	// current length of the text
	int		m_TextLen;

	float	m_BufferVertices[1024];

	float	*pBaseVertex;

	int		m_VertexCount;

	// debug info
	int		m_VertexPerFrame;

	bool	m_bIsInit;
};

static CFontSystem s_FontSystem;

extern IFontSystem& FontSystem()
{
	return s_FontSystem;
}

CFontSystem::CFontSystem()
{
	pBaseVertex = nullptr;

	m_VertexCount = 0;
	m_TextLen = 0;

	m_StaticTotalVMem = 0;
	m_StaticFreeVMem = 0;

	m_VertexPerFrame = 0;

	m_bIsInit = false;

	ClearAllState();
}

CFontSystem::~CFontSystem()
{ }

void CFontSystem::InitBuffers(void)
{
	// Vertex Buffer Object 
	glGenBuffers(2, m_VertexBufferId);

	// Vertex Array Object
	glGenVertexArrays(2, m_VertexArrayId);

	glBindVertexArray(m_VertexArrayId[0]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId[0]);
		glVertexAttribPointer(ATTRIB_POSITION, 4, GL_FLOAT, GL_FALSE, 0, (char*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(ATTRIB_POSITION);
	glBindVertexArray(0);

	glBindVertexArray(m_VertexArrayId[1]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId[1]);
		glVertexAttribPointer(ATTRIB_POSITION, 4, GL_FLOAT, GL_FALSE, 0, (char*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(ATTRIB_POSITION);
	glBindVertexArray(0);

	m_StaticTotalVMem = STATIC_CHARS * 96;

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId[0]);

	// 6 vertices (to draw 1 char) * float4(x,y,s,t) * sizeof(float) = 96
	glBufferData( GL_ARRAY_BUFFER, m_StaticTotalVMem, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CFontSystem::PrintStaticText(int idText)
{
	const int numString = m_StaticTextInfo.Num();

	if (numString <= 0)
		return;

	g_pFontManager.GetTextureID( m_TexID );

	glBindTexture( GL_TEXTURE_2D, m_TexID );

	glBindVertexArray( m_VertexArrayId[0] );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if (idText == 0)
	{
		for (int i = 0; i < numString; ++i)
		{
			TextInfo &dti = m_StaticTextInfo[i];

			m_fontShader.Set_Float4v(dti.color, m_UnifColor);

			m_VertexPerFrame += dti.countVertex;
			glDrawArrays( GL_TRIANGLES, dti.firstVertex, dti.countVertex );
		}
	} 
	else {
		const int index = idText - 1;

		if((numString - 1) < index)
			return;

		TextInfo &dti = m_StaticTextInfo[index];

		m_fontShader.Set_Float4v(dti.color, m_UnifColor);

		m_VertexPerFrame += dti.countVertex;
		glDrawArrays( GL_TRIANGLES, dti.firstVertex, dti.countVertex );
	}

	glDisable( GL_BLEND );

	glBindVertexArray(0);
}

inline void PointerInc(float *& pData, const float &pX, const float &pY, const float &u, const float &v)
{
	float *p = pData;
	*p++ = pX;
	*p++ = pY;
	*p++ = u;
	*p   = v;

	pData += 4;
}

template<typename T>
void CFontSystem::BuildTextVertices(const T* text)
{
	//int inc = 0;
	//auto toSet = [](const float &pX, const float &pY, const float &u, const float &v)
	//{
	//	pBaseVertex[inc++] = pX;
	//	pBaseVertex[inc++] = pY;
	//	pBaseVertex[inc++] = u;
	//	pBaseVertex[inc++] = v;
	//};
	CFont &font = *g_pFontManager.GetFontByID(m_hFont);

	const int height = font.Height();

	int x = 0, y = 0;
	int posX = m_DrawTextPos[0];
	int posY = m_DrawTextPos[1];

	m_VertexPerFrame += m_VertexCount;
	m_VertexCount = 0;

	for (int i = 0; i < m_TextLen; ++i)
	{
		const T &Ch = text[i];

		if ( !font.AssignCacheForChar(Ch) )
			continue;

		const GlyphDesc_t &g = *font.GetGlyphDesc();

		x = posX + g.bitmapLeft;
		y = (posY + height) - g.bitmapTop;

		posX += g.advanceX;
		posY += g.advanceY;

		if (iswspace(Ch))
		{
			if (Ch == '\n') {
				posX = m_DrawTextPos[0];
				posY += height;
			}
			continue;
		}

		const int w = g.bitmapWidth;
		const int h = g.bitmapHeight;

		const float *pTex = font.GetTexCoords();

		//toSet(x,     y,     pTex[ 0], pTex[1]);
		//toSet(x,     y + h, pTex[ 2], pTex[3]);
		//toSet(x + w, y,     pTex[ 4], pTex[5]);
		//toSet(x + w, y,     pTex[ 6], pTex[7]);
		//toSet(x,     y + h, pTex[ 8], pTex[9]);
		//toSet(x + w, y + h, pTex[10], pTex[11]);

		PointerInc(pBaseVertex, x,     y,     pTex[ 0], pTex[1]);
		PointerInc(pBaseVertex, x,     y + h, pTex[ 2], pTex[3]);
		PointerInc(pBaseVertex, x + w, y,     pTex[ 4], pTex[5]);
		PointerInc(pBaseVertex, x + w, y,     pTex[ 6], pTex[7]);
		PointerInc(pBaseVertex, x,     y + h, pTex[ 8], pTex[9]);
		PointerInc(pBaseVertex, x + w, y + h, pTex[10], pTex[11]);

		m_VertexCount += 6;
	}
}

template<typename T>
int CFontSystem::BuildStaticText(const T* text)
{
	if ( !text || (m_StaticFreeVMem < (m_TextLen * 96)))
		return -1;

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId[0]);

	void* pVM = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	if ( !pVM ) {
		fprintf(stderr, "OpenGL Error glMapBuffer");
		return -1;
	}

	pBaseVertex = (float*)pVM + (m_CurrStaticVertex * 4);

	BuildTextVertices<T>(text);

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	TextInfo ti;

	ti.countVertex = m_VertexCount;
	ti.firstVertex = m_CurrStaticVertex;
	ti.color[0] = m_DrawTextColor[0];
	ti.color[1] = m_DrawTextColor[1];
	ti.color[2] = m_DrawTextColor[2];
	ti.color[3] = m_DrawTextColor[3];

	m_CurrStaticVertex += m_VertexCount;

	// 4float(x,y,s,t) * sizeof(float)
	m_StaticFreeVMem -= m_VertexCount * (4 * sizeof(float)); 

	m_StaticTextInfo.Append( ti );

	return  m_StaticTextInfo.Num();
}

template<typename T>
BBox_t CFontSystem::GetTextArea(const T *text, BBox_t &bbox) const
{
	assert(text);

	int posX = m_DrawTextPos[0], posY = m_DrawTextPos[1];

	int maxW = 0, maxH = 0;

	CFont &font = *g_pFontManager.GetFontByID(m_hFont);

	const int strHeight = font.Height();

	for (int i = 0; i < m_TextLen; ++i)
	{
		const T &Ch = text[i];

		if (iswspace(Ch))
		{
			if (Ch == '\n')
			{
				posY += strHeight;
				posX = m_DrawTextPos[0];
				continue;
			}

			if( (Ch == ' ') || (Ch == '\t') );
			else {
				continue;
			}
		}

		if ( !font.AssignCacheForChar(Ch) ) {
			continue;
		}

		GlyphDesc_t const *pDesc = font.GetGlyphDesc();

		posX += pDesc->advanceX;
		posY += pDesc->advanceY;

		if (posX > maxW) {
			maxW = posX;
		}
	}

	maxH = posY + strHeight + (strHeight - font.GetAbsoluteValue());

	bbox.xMin = m_DrawTextPos[0];
	bbox.yMin = m_DrawTextPos[1];
	bbox.xMax = maxW;
	bbox.yMax = maxH;

	return bbox;
}

void CFontSystem::Draw2DText( void )
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId[1]);

	glBufferData(GL_ARRAY_BUFFER, m_VertexCount * 16, m_BufferVertices, GL_DYNAMIC_DRAW);

	g_pFontManager.GetTextureID(m_TexID);

	glBindTexture(GL_TEXTURE_2D, m_TexID);

	glBindVertexArray(m_VertexArrayId[1]);

	glVertexAttribPointer(ATTRIB_POSITION, 4, GL_FLOAT, GL_FALSE, 0, (char*)0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_fontShader.Set_Float4v(m_DrawTextColor, m_UnifColor);

	glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);

	glDisable(GL_BLEND);

	glBindVertexArray(0);
}

#include "shaders/shaders.inl"

bool CFontSystem::Initialize( void )
{
	if (m_bIsInit)
		return false;

	if (!ftLib::InitFT2Lib())
		return false;
	
	if (!m_fontShader.BuildShaderProgramMem(VertexShader, FragmentShader))
		return false;

	m_fontShader.Begin_Use();

	m_fontShader.Set_Float2v(m_DrawTextColor, "u_Color", &m_UnifColor);

	InitBuffers();

	return m_bIsInit = true;
}

void CFontSystem::SetScreenSize(int sWidth, int sHeight)
{
	m_fontShader.Begin_Use();

	m_scaleX = 2.0 / (float)sWidth;
	m_scaleY = 2.0 / (float)sHeight;

	m_fontShader.Set_Float2(m_scaleX, m_scaleY, "u_Scale");
}

void CFontSystem::Shutdown( void )
{
	g_pFontManager.ClearAllFonts();

	m_fontShader.End_Use();

	m_fontShader.DestroyVertexShader();
	m_fontShader.DestroyFragmentShader();

	m_fontShader.DestroyShaderProgram();

	glDeleteVertexArrays(2, m_VertexArrayId);
	glDeleteBuffers(2, m_VertexBufferId);

	ftLib::DoneFT2Lib();

	m_bIsInit = false;
}

HFont CFontSystem::Create_Font(const char *fontName, int size)
{
	return g_pFontManager.Create_Font(fontName, size);
}

bool CFontSystem::AddGlyphSetToFont(HFont handle, CharacterSets flags)
{
	if ((handle <= 0) || (flags >= NUM_CHARACTER_SETS))
		return false;

	m_hFont = handle;

	int l = g_FontRange[flags].lowerRange;
	int h = g_FontRange[flags].upperRange;

	return g_pFontManager.AddGlyphSetToFont(handle, l, h);
}

bool CFontSystem::AddGlyphSetToFont(HFont handle, int lowerRange, int upperRange)
{
	if ((handle <= 0) || ((upperRange - lowerRange) <= 0))
		return false;

	m_hFont = handle;

	return g_pFontManager.AddGlyphSetToFont(handle, lowerRange, upperRange);
}

bool CFontSystem::BuildAllFonts( void )
{
	return g_pFontManager.BuildAllFonts();
}

HFont CFontSystem::LoadFontCache(const char *fileName)
{
	return g_pFontManager.LoadFontCache(fileName);
}

bool CFontSystem::DumpFontCache(HFont handle, const char* path)
{
	return g_pFontManager.DumpFontCache(handle, path);
}

void CFontSystem::DrawFilledRect(int x0, int y0, int x1, int y1)
{
	GLfloat vVerts[] =	{	(float)x0, (float)y1,	//	V1
							(float)x1, (float)y1,	//	V2
							(float)x0, (float)y0,	//	V6
							(float)x1, (float)y0,	//	V5
						};

	glBindVertexArray(m_VertexArrayId[1]);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId[1]);

	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vVerts, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// pass to the shader program the color data
	m_fontShader.Set_Float4v(m_DrawColor, m_UnifColor);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

void CFontSystem::DrawOutLinedRect(int x0, int y0, int x1, int y1)
{
	DrawLine(x0, y0, x1, y0+1);	//top
	DrawLine(x0, y1-1, x1, y1);	//bottom
	DrawLine(x0, y0, x0+1, y1);	//left
	DrawLine(x1-1, y0, x1, y1);	//right
}

//-----------------------------------------------------------------------------
// Draws the line
//-----------------------------------------------------------------------------
void CFontSystem::DrawLine(int x0, int y0, int x1, int y1)
{
	GLfloat vVerts[] = 	{	(float)x0, (float)y0,	//	V1
							(float)x1, (float)y1,	//	V2
						};

	glBindVertexArray(m_VertexArrayId[1]);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId[1]);

	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), vVerts, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	m_fontShader.Set_Float4v(m_DrawColor, m_UnifColor);

	glDrawArrays(GL_LINES, 0, 2);

	glBindVertexArray(0);
}

int CFontSystem::GetFontSize(HFont font) const
{
	return g_pFontManager.GetFontHeight(font);
}

void CFontSystem::ClearAllState(void)
{
	m_DrawTextPos[0] = m_DrawTextPos[1] = 0;

	m_DrawTextColor[0] = 0;
	m_DrawTextColor[1] = 0;
	m_DrawTextColor[2] = 0;
	m_DrawTextColor[3] = 255;

	m_StaticTextInfo.Clear();

	m_CurrStaticVertex = 0;
	m_StaticFreeVMem = STATIC_CHARS * 96;
}

void CFontSystem::ResetStaticText()
{
	if (m_StaticTotalVMem != (STATIC_CHARS * 96))
	{
		m_StaticTotalVMem = STATIC_CHARS * 96;
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId[0]);

		// reserve video memory for a static text
		// 6 vertices (to draw 1 char) * float4(x,y,s,t) * sizeof(float) = 96
		glBufferData( GL_ARRAY_BUFFER, m_StaticTotalVMem, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	m_StaticTextInfo.Clear();

	m_CurrStaticVertex = 0;
	m_StaticFreeVMem = STATIC_CHARS * 96;
}

bool CFontSystem::BuildCache(void)
{
	return g_pFontManager.BuildCacheFonts();
}

void CFontSystem::EnableStateDraw(void)
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	m_VertexPerFrame = 0;

	m_fontShader.Begin_Use();
}

void CFontSystem::DisableStateDraw(void)
{
	m_fontShader.End_Use();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

bool	SetFontTextureWidth(const int texture_width)
{
	if ( (texture_width < MIN_TEXTURE_WIDTH) ||
		(texture_width > MAX_TEXTURE_WIDTH) )
		return false;

	FONT_TEXTURE_WIDTH = texture_width;

	return true;
}

bool	SetMaxStaticChars(const int max_Static_Chars)
{
	if ( (max_Static_Chars < MIN_STATIC_CHARS) ||
		(max_Static_Chars > MAX_STATIC_CHARS) )
		return false;

	STATIC_CHARS = max_Static_Chars;

	return true;
}