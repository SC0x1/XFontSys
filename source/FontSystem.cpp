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

#include "shaders/ShaderSystem.h"
#include "shaders/VertexBuffer.h"

#include "public/utlvector.h"
#include "public/common.h"

#include "FontGlobal.h"

const int cfg::MAX_LENGTH_STRING = 1024;

int cfg::STATIC_CHARS = 4096;
const int cfg::MIN_STATIC_CHARS = 0;
const int cfg::MAX_STATIC_CHARS = 50000;

int cfg::FONT_TEXTURE_WIDTH = 1024;
const int cfg::MIN_TEXTURE_WIDTH = 128;
const int cfg::MAX_TEXTURE_WIDTH = 4096;

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

	/* ... add more */

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
		if ((m_TextLen = textLen) > cfg::STATIC_CHARS)
			return;

		GetTextArea<char>(text, bbox);
	}

	void GetWTextBBox(const wchar_t *text, const int textLen, BBox_t &bbox)
	{
		if ((m_TextLen = textLen) > cfg::STATIC_CHARS)
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
		if ((m_TextLen = textLen) > cfg::STATIC_CHARS)
			return -1;

		return BuildStaticText<char>(text);
	}

	int SetStaticWText(const wchar_t *text, const int textLen)
	{
		if ((m_TextLen = textLen) > cfg::STATIC_CHARS)
			return -1;

		return BuildStaticText<wchar_t>(text);
	}

	void ResetStaticText( void );

	void PrintStaticText(int idText);

	void PrintText(const char *text, const int textLen)
	{
		if (!text || ((m_TextLen = textLen) > cfg::MAX_LENGTH_STRING))
			return;

		if ((m_hFont <= 0) || (m_DrawTextColor[3] <= 0.1))
			return;

		pBaseVertex = m_BufferVertices;

		BuildTextVertices<char>(text);

		CommitDynamicVertex();
	}

	void PrintWText(const wchar_t *text, const int textLen)
	{
		if (!text || ((m_TextLen = textLen) > cfg::MAX_LENGTH_STRING))
			return;

		if ((m_hFont <= 0) || (m_DrawTextColor[3] <= 0.1))
			return;

		pBaseVertex = m_BufferVertices;

		BuildTextVertices<wchar_t>(text);

		CommitDynamicVertex();
	}

	void CommitDynamicVertex(void)
	{
		m_pVBODynamic->PushVertexData(m_CountVertex, m_BufferVertices);

		TextInfo_t txtInf;
		txtInf.vaoID = m_pVBODynamic->VertexArrayObjectID();
		txtInf.textureID = g_pFontManager.GetTextureID();
		txtInf.countVerts = m_CountVertex;
		txtInf.offsetVerts = 0;

		DrawSimple2DText(txtInf);
	}

	bool HasKerning(HFont handle)
	{ 
		return g_pFontManager.HasKerning(handle);
	}

	void UseKerning(bool flag)
	{
		m_bIsKerning = flag;
	}

	void BeginDraw( void );
	void EndDraw( void );

	int VertexPerFrame( void ) const
	{
		return m_VertexPerFrame;
	}

private:

	template<typename T>
	void BuildTextVertices(const T* text);

	template<typename T>
	int BuildStaticText(const T* text);

	template<typename T>
	BBox_t GetTextArea(const T* text, BBox_t &bbox) const;

	void ClearAllState( void );

	CShaderOGL m_fontShader;

	CVertexBuffer *m_pVBOStatic;
	CVertexBuffer *m_pVBODynamic;

	float m_scaleX;
	float m_scaleY;

	CUtlVector<TextInfo_t> m_StaticTextInfo;

	int MAX_STATIC_CHARS;

	// color for text
	uint8 m_DrawTextColor[4];

	// current position in screen space
	int m_DrawTextPos[2];

	// descriptor (id) of the current font
	HFont m_hFont;

	// current length of the text
	int m_TextLen;

	float m_BufferVertices[cfg::MAX_LENGTH_STRING];

	float *pBaseVertex;

	int m_CountVertex;

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

	m_CountVertex = 0;

	m_TextLen = 0;

	MAX_STATIC_CHARS = 0;

	m_VertexPerFrame = 0;

	m_bIsKerning = m_bIsInit = false;

	ClearAllState();
}

CFontSystem::~CFontSystem()
{ }

void CFontSystem::PrintStaticText(int idText)
{
	const int numString = m_StaticTextInfo.Num();

	if (numString <= 0)
		return;

	const int index = idText - 1;

	if((numString - 1) < index)
		return;

	DrawSimple2DText(m_StaticTextInfo[index]);
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

	m_VertexPerFrame += m_CountVertex;
	m_CountVertex = 0;

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

		// writes quad positions
		PosQuad4f(pBaseVertex, x, y, x + g.bitmapWidth, y + g.bitmapHeight);
		TexCoord4f(pBaseVertex, g.texCoord);
		Color4b(pBaseVertex, m_DrawTextColor);
		++m_CountVertex;
	}
}

template<typename T>
int CFontSystem::BuildStaticText(const T* text)
{
	// since we use a type GL_POINTS so textLenght == vertex count
	if ( !text || !m_pVBOStatic->HasEnoughSpace(m_TextLen))
		return -1;

	int baseVertex;

	void *pVM = m_pVBOStatic->Lock(m_TextLen, baseVertex);
	
	if ( !pVM )
	{
		fprintf(stderr, "\nError Lock Vertex Buffer\n");
		return -1;
	}

	// float4(pos) + float4(texCoords) + (4 bytes per color) = 9
	pBaseVertex = (float*)pVM + (baseVertex * 9);

	BuildTextVertices<T>(text);

	m_pVBOStatic->Unlock(m_CountVertex);

	TextInfo_t txtInfo;

	txtInfo.vaoID = m_pVBOStatic->VertexArrayObjectID();
	txtInfo.textureID = g_pFontManager.GetTextureID();
	txtInfo.offsetVerts = baseVertex;
	txtInfo.countVerts = m_CountVertex;

	m_StaticTextInfo.Append( txtInfo );

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

#include "shaders/shaders.inl"

bool CFontSystem::Initialize( void )
{
	if (m_bIsInit)
		return false;

	if ( !ftLib::InitFT2Lib() )
		return false;

	if (!m_fontShader.BuildShaderProgram(VertexShader, GeometryShader, FragmentShader))
	{
		ftLib::DoneFT2Lib();
		return false;
	}

	m_pVBODynamic = new CVertexBuffer(FVF_Simple2DColoredText, VERTEX_SIZE, 0, true);

	m_pVBOStatic = new CVertexBuffer(FVF_Simple2DColoredText, VERTEX_SIZE, cfg::STATIC_CHARS, false);

	MAX_STATIC_CHARS = cfg::STATIC_CHARS;

	return m_bIsInit = true;
}

bool CFontSystem::SetScreenSize(int sWidth, int sHeight)
{
	m_fontShader.Begin_Use();

	m_scaleX = 2.0 / (float)sWidth;
	m_scaleY = 2.0 / (float)sHeight;

	bool isSetScale = false;

	isSetScale = m_fontShader.Set_Float2("u_Scale", m_scaleX, m_scaleY);

	return isSetScale;
}

void CFontSystem::Shutdown( void )
{
	g_pFontManager.ClearAllFonts();

	m_fontShader.DestroyShaderProgram();

	if (m_pVBOStatic)
		delete m_pVBOStatic;

	if (m_pVBODynamic)
		delete m_pVBODynamic;

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
}

void CFontSystem::ResetStaticText(void)
{
	if (MAX_STATIC_CHARS != cfg::STATIC_CHARS)
	{
		m_pVBOStatic->ReInit(FVF_Simple2DColoredText, VERTEX_SIZE, cfg::STATIC_CHARS, true);
		MAX_STATIC_CHARS = cfg::STATIC_CHARS;
	}

	m_pVBOStatic->Clear();

	m_StaticTextInfo.Clear();
}

bool CFontSystem::BuildCache(void)
{
	return g_pFontManager.BuildCacheFonts();
}

void CFontSystem::BeginDraw(void)
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	m_VertexPerFrame = 0;

	m_fontShader.Begin_Use();
}

void CFontSystem::EndDraw(void)
{
	m_fontShader.End_Use();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

bool SetFontTextureWidth(const int texture_width)
{
	if ( (texture_width < cfg::MIN_TEXTURE_WIDTH) ||
		(texture_width > cfg::MAX_TEXTURE_WIDTH) )
		return false;

	cfg::FONT_TEXTURE_WIDTH = texture_width;

	return true;
}

bool SetMaxStaticChars(const int max_Static_Chars)
{
	if ( (max_Static_Chars < cfg::MIN_STATIC_CHARS) ||
		(max_Static_Chars > cfg::MAX_STATIC_CHARS) )
		return false;

	cfg::STATIC_CHARS = max_Static_Chars;

	return true;
}