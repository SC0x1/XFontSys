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

// 16 bytes(pos) + 16 bytes(texCoords) + 4 bytes (color)
const int VERTEX_SIZE = 36; 

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

class CFontSystem : public IFontSystem
{
public:
	CFontSystem();
	~CFontSystem();

	bool Initialize( void );

	bool SetScreenSize(int sWidth, int sHeight);

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

	void SetTextColor(uint8 r, uint8 g, uint8 b, uint8 a)
	{
		m_DrawTextColor[0] = r;
		m_DrawTextColor[1] = g;
		m_DrawTextColor[2] = b;
		m_DrawTextColor[3] = a;
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

	bool HasKerning(HFont handle)
	{ 
		return g_pFontManager.HasKerning(handle);
	}

	void UseKerning(bool flag)
	{
		m_bIsKerning = flag;
	}

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

	//-----------------------------------------------------------------------------
	// Structure that store informations about the static text
	//-----------------------------------------------------------------------------
	struct TextInfo
	{
		// first vertex in the VBO
		unsigned short	firstVertex;	
		// total number of vertices for a string
		unsigned short	countVertex;
	};

	CShaderOGL m_fontShader;

	float m_scaleX;
	float m_scaleY;

	unsigned int m_VertexArrayId[2];  // Vertex Array Object ID
	unsigned int m_VertexBufferId[2]; // Vertex Buffer Object ID

	unsigned int m_TexID;

	// current number vertex for a static text
	unsigned short m_CurrStaticVertex;

	CUtlVector<TextInfo> m_StaticTextInfo;

	// amount of free space in the video memory for the vertices of the static text in bytes
	int m_StaticFreeVMem;
	int m_StaticTotalVMem;

	// color for text
	uint8 m_DrawTextColor[4];

	// current position in screen space
	int m_DrawTextPos[2];

	// descriptor (id) of the current font
	HFont m_hFont;

	// current length of the text
	int m_TextLen;

	float m_BufferVertices[1024];

	float *pBaseVertex;

	int m_ElementCount;

	// debug info
	int m_VertexPerFrame;

	bool m_bIsInit;

	bool m_bIsKerning;
};

static CFontSystem s_FontSystem;

extern IFontSystem& FontSystem()
{
	return s_FontSystem;
}

CFontSystem::CFontSystem()
{
	pBaseVertex = nullptr;

	m_ElementCount = 0;
	m_TextLen = 0;

	m_StaticTotalVMem = 0;
	m_StaticFreeVMem = 0;

	m_VertexPerFrame = 0;

	m_bIsKerning = m_bIsInit = false;

	ClearAllState();
}

CFontSystem::~CFontSystem()
{ }

inline void AdjustVertexPointers( void )
{
	// pointers
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 4, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (GLvoid*)0);

	glEnableVertexAttribArray(ATTRIB_TEXCOORD0);
	glVertexAttribPointer(ATTRIB_TEXCOORD0, 4, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (GLvoid*)16/*4*sizeof(float)*/);

	glEnableVertexAttribArray(ATTRIB_COLOR);
	glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_FALSE, VERTEX_SIZE, (GLvoid*)32/*8*sizeof(float)*/);
}

void CFontSystem::InitBuffers(void)
{
	// Vertex Buffer Object 
	glGenBuffers(2, m_VertexBufferId);

	// Vertex Array Object
	glGenVertexArrays(2, m_VertexArrayId);

	// For the static draws
	glBindVertexArray(m_VertexArrayId[0]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId[0]);
		glBufferData( GL_ARRAY_BUFFER, m_StaticTotalVMem, nullptr, GL_DYNAMIC_DRAW);

		AdjustVertexPointers();

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glBindVertexArray(m_VertexArrayId[1]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId[1]);

		AdjustVertexPointers();

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
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

			glDrawArrays( GL_POINTS, dti.firstVertex, dti.countVertex );

			m_VertexPerFrame += dti.countVertex;
		}
	} 
	else {
		const int index = idText - 1;

		if((numString - 1) < index)
			return;

		TextInfo &dti = m_StaticTextInfo[index];

		glDrawArrays( GL_POINTS, dti.firstVertex, dti.countVertex );

		m_VertexPerFrame += dti.countVertex;
	}

	glDisable( GL_BLEND );

	glBindVertexArray(0);
}

inline void PosQuad4f(float* &pData, float const &x0, float const &y0, float const &x1, float const &y1)
{
	float *p = pData;
	*p++ = x0;
	*p++ = y0;
	*p++ = x1;
	*p   = y1;
	pData += 4;
}

inline void TexCoord4f(float* &pData, float const *pTexCoords)
{
	float *p = pData;
	*p++ = pTexCoords[0];
	*p++ = pTexCoords[1];
	*p++ = pTexCoords[2];
	*p   = pTexCoords[3];

	pData += 4;
}

inline void Color4b(float* &pData, uint8 const *pColor)
{
	uint8 *p = reinterpret_cast<uint8*>(pData);
	*p++ = *pColor;
	*p++ = *(pColor + 1);
	*p++ = *(pColor + 2);
	*p   = *(pColor + 3);

	pData += 1;
}

template<typename T>
void CFontSystem::BuildTextVertices(const T* text)
{
	CFont &font = *g_pFontManager.GetFontByID(m_hFont);

	const int height = font.Height();

	int x = 0, y = 0;
	int posX = m_DrawTextPos[0];
	int posY = m_DrawTextPos[1];

	m_VertexPerFrame += m_ElementCount;
	m_ElementCount = 0;

	for (int i = 0; i < m_TextLen; ++i)
	{
		const T &Ch = text[i];

		if ( !font.AssignCacheForChar(Ch) )
			continue;

		const GlyphDesc_t &g = *font.GetGlyphDesc();

		x = posX + g.bitmapLeft;
		y = (posY + height) - g.bitmapTop;

		m_bIsKerning ?
			posX += g.advanceX + font.GetKerningX(text[i], text[i+1])
			:
			posX += g.advanceX;

		//posY += g.advanceY;

		if (iswspace(Ch))
		{
			if (Ch == '\n') {
				posX = m_DrawTextPos[0];
				posY += height;
			}
			continue;
		}

		PosQuad4f(pBaseVertex, x, y, x + g.bitmapWidth, y + g.bitmapHeight);
		TexCoord4f(pBaseVertex, g.texCoord);
		Color4b(pBaseVertex, m_DrawTextColor);
		++m_ElementCount;
	}
}

template<typename T>
int CFontSystem::BuildStaticText(const T* text)
{
	if ( !text || (m_StaticFreeVMem < (m_TextLen * VERTEX_SIZE)))
		return -1;

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId[0]);

	void* pVM = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	if ( !pVM )
	{
		fprintf(stderr, "OpenGL Error glMapBuffer");
		return -1;
	}

	// float4(pos) + float4(texCoords) + (4 bytes per color) = 9
	pBaseVertex = (float*)pVM + (m_CurrStaticVertex * 9);

	BuildTextVertices<T>(text);

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	TextInfo ti;

	ti.countVertex = m_ElementCount;
	ti.firstVertex = m_CurrStaticVertex;

	m_CurrStaticVertex += m_ElementCount;

	// float4(x1,y1,x2,y2) + float4(s1,t1,s2,t2) + (4 bytes per color) * sizeof(float)
	m_StaticFreeVMem -= m_ElementCount * (sizeof(float) * 9); 

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

	const int lineHeight = font.Height();

	for (int i = 0; i < m_TextLen; ++i)
	{
		const T &Ch = text[i];

		if (iswspace(Ch))
		{
			if (Ch == '\n')
			{
				posY += lineHeight;
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

		m_bIsKerning ? 
			posX += pDesc->advanceX + font.GetKerningX(text[i], text[i+1])
			:
			posX += pDesc->advanceX;

		//posY += pDesc->advanceY;

		if (posX > maxW) {
			maxW = posX;
		}
	}

	maxH = posY + lineHeight + (lineHeight - font.GetAbsoluteValue());

	bbox.xMin = m_DrawTextPos[0];
	bbox.yMin = m_DrawTextPos[1];
	bbox.xMax = maxW;
	bbox.yMax = maxH;

	return bbox;
}

void CFontSystem::Draw2DText( void )
{
	glBindVertexArray(m_VertexArrayId[1]);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId[1]);

	glBufferData(GL_ARRAY_BUFFER, m_ElementCount * VERTEX_SIZE, m_BufferVertices, GL_DYNAMIC_DRAW);

	g_pFontManager.GetTextureID(m_TexID);

	glBindTexture(GL_TEXTURE_2D, m_TexID);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_POINTS, 0, m_ElementCount);

	glBindVertexArray(0);

	glDisable(GL_BLEND);
}

#include "shaders/shaders.inl"

bool CFontSystem::Initialize( void )
{
	if (m_bIsInit)
		return false;

	if (!ftLib::InitFT2Lib())
		return false;

	if (!m_fontShader.BuildShaderProgramMem(VertexShader, GeometryShader, FragmentShader))
		return false;

	m_fontShader.Begin_Use();

	m_StaticTotalVMem = STATIC_CHARS * VERTEX_SIZE;

	InitBuffers();

	return m_bIsInit = true;
}

bool CFontSystem::SetScreenSize(int sWidth, int sHeight)
{
	m_fontShader.Begin_Use();

	m_scaleX = 2.0 / (float)sWidth;
	m_scaleY = 2.0 / (float)sHeight;

	bool isSetScale = false;

	isSetScale = m_fontShader.Set_Float2(m_scaleX, m_scaleY, "u_Scale");

	return isSetScale;
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
	m_StaticFreeVMem = STATIC_CHARS * VERTEX_SIZE;
}

void CFontSystem::ResetStaticText()
{
	if (m_StaticTotalVMem != (STATIC_CHARS * VERTEX_SIZE))
	{
		m_StaticTotalVMem = STATIC_CHARS * VERTEX_SIZE;
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId[0]);

		glBufferData(GL_ARRAY_BUFFER, m_StaticTotalVMem, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	m_StaticTextInfo.Clear();

	m_CurrStaticVertex = 0;
	m_StaticFreeVMem = STATIC_CHARS * VERTEX_SIZE;
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