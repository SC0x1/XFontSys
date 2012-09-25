/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: The unicode text render interface
/*
/****************************************************************************/

#pragma once

#define FSGL3_VERSION 0.3

// A font handle
typedef int HFont;

#define INVALID_FONT -1

typedef struct BBox_s 
{
	BBox_s(void) 
		: xMin(0), yMin(0), xMax(0), yMax(0)
	{ }
	BBox_s& operator=(const BBox_s& r)
	{
		if (this == &r) {
			return *this;
		}
		this->xMin = r.xMin;
		this->yMin = r.yMin;
		this->xMax = r.xMax;
		this->yMax = r.yMax;

		return *this;
	}
	int xMin;
	int yMin;
	int xMax;
	int yMax;
}BBox_t;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

enum CharacterSets
{
	BASIC_LATIN = 0,             // U+0000 - U+007F (0–127)
	LATIN_1_SUPPLEMENT,          // U+0080 - U+00FF (128–255)
	LATIN_EXTENDED_A,            // U+0100 - U+017F (256–383)
	LATIN_EXTENDED_B,            // U+0180 - U+024F (384–591)
	IPA_EXTENSIONS,              // U+0250 - U+02AF (592-687)
	SPACING_MODIFIER_LETTERS,    // U+02B0 - U+02FF (688-767)
	COMBINING_DIACRITICAL_MARKS, // U+0300 - U+036F (768-879)
	GREEK_AND_COPTIC,            // U+0370 - U+03FF (880–1023)
	CYRILLIC,                    // U+0400 - U+04FF (1024–1279)
	CYRILLIC_SUPPLEMENTARY,      // U+0500 - U+052F (1280-1327)

	/* ... add more */

	NUM_CHARACTER_SETS,
};

class IFontSystem
{
public:
	virtual ~IFontSystem() { }

	//---------------------------------------------------------------------------
	// Purpose: Initialize a library
	//---------------------------------------------------------------------------
	virtual bool Initialize( void ) = 0;

	//---------------------------------------------------------------------------
	// Purpose: This method should be called every time when you change the screen size
	// Input  : int sWidth - Window width
	//          int sHeight- Window height
	//---------------------------------------------------------------------------
	virtual bool SetScreenSize(int sWidth, int sHeight) = 0;

	//---------------------------------------------------------------------------
	// Purpose: Shutdown a library
	//---------------------------------------------------------------------------
	virtual void Shutdown( void ) = 0;

	//---------------------------------------------------------------------------
	// Purpose: Creates the font (using the FreeType 2) from a file (*.ttf, *.otf ...)
	// Input  : const char *fontPathName - A path to the font file
	//          int size - The font size to be used (8 .. 128)
	// Output : Returns the font handle
	//---------------------------------------------------------------------------
	virtual HFont Create_Font(const char *fontPathName, int size) = 0;
	
	//---------------------------------------------------------------------------
	// Purpose: Adds the glyphs to a font created by 'Create_Font( ... )'
	// Input  : HFont handle - A font handle
	//          CharacterSets flags - Flag set of characters
	// Output : Returns true on success, false on failure
	//---------------------------------------------------------------------------
	virtual bool AddGlyphSetToFont(HFont handle, CharacterSets flags) = 0;

	//---------------------------------------------------------------------------
	// Purpose: Adds the glyphs to a font created by 'Create_Font( ... )'
	// Input  : HFont handle - A font handle
	//          int lowerRange - The lower unicode range
	//          int upperRange - The upper unicode range
	// Output : Returns true on success, false on failure
	//---------------------------------------------------------------------------
	virtual bool AddGlyphSetToFont(HFont handle, int lowerRange, int upperRange) = 0;

	//---------------------------------------------------------------------------
	// Purpose: Build all fonts
	//          This method must be called after all calls 'AddGlyphSetToFont'
	// Output : Returns true on success, false on failure
	//---------------------------------------------------------------------------
	virtual bool BuildAllFonts( void ) = 0;

	//---------------------------------------------------------------------------*/
	// Purpose: Dump the font caches (metrics of all characters and texture)
	// Input  : HFont handle - A font handle
	//          const char* path- Your directory path to save the font cache
	// Output : Returns true on success, false on failure
	//---------------------------------------------------------------------------*/
	virtual bool DumpFontCache(HFont handle, const char* path) = 0;

	//---------------------------------------------------------------------------
	// Purpose: Loads cach for the range of characters
	//          (only need to specify the filename with the extension '*.cfnt')
	// Input  : const char *filename - Filename with the extension '*.cfnt'
	// Output : Returns the font handle
	//---------------------------------------------------------------------------
	virtual HFont LoadFontCache(const char *filename) = 0;

	//---------------------------------------------------------------------------
	// Purpose: If the fonts are loaded from the cache, then need call this method
	//          instead 'BuildAllFonts()'
	// Output : Returns true on success, false on failure
	//---------------------------------------------------------------------------
	virtual	bool BuildCache( void ) = 0;
	
	//---------------------------------------------------------------------------
	// Purpose: We must bind the font descriptor (id) before we can display any text with it
	//          For the static text this method must be call before the method 'SetStaticText' only!
	// Input  : const HFont handle - A font handle
	//---------------------------------------------------------------------------
	virtual void BindFont(HFont handle) = 0;

	//---------------------------------------------------------------------------
	// Purpose: Calculate a bounding box for the given text
	// Input  : const char *text / const wchar_t *text - String of a text
	//         (newline characters processed)
	// Output : BBox_t &bbox - Returns the bounding box for a text area
	//---------------------------------------------------------------------------
	virtual void GetTextBBox(const char *text, const int textLen, BBox_t &bbox) = 0;
	virtual void GetWTextBBox(const wchar_t *text, const int textLen, BBox_t &bbox) = 0;

	//---------------------------------------------------------------------------
	// Purpose: Sets color for the any text
	//          (call before methods 'SetStaticText' and 'PrintText')
	// Input  : uint8 r, uint8 g, uint8 b, uint8 a - The RGBA color values
	//---------------------------------------------------------------------------
	virtual void SetTextColor(uint8 r, uint8 g, uint8 b, uint8 a = 255) = 0;

	//---------------------------------------------------------------------------
	// Purpose: Sets position in screen space for the display text
	//          (call before methods 'SetStaticText' and 'PrintText')
	//---------------------------------------------------------------------------
	virtual void SetTextPos(const int x, const int y) = 0;
	virtual void SetTextPosX(const int x) = 0;
	virtual void SetTextPosY(const int y) = 0;

	//---------------------------------------------------------------------------
	// Purpose: Returns the current position for text in the screen space coordinate
	//---------------------------------------------------------------------------
	virtual void GetTextPos(int &x, int &y) const = 0;
	virtual void GetTextPosX(int &x) const = 0;
	virtual void GetTextPosY(int &y) const = 0;
	
	//---------------------------------------------------------------------------
	// Purpose: Sets a static text and returns for it id
	// Input  : const char *text / const wchar_t *text - String of a text
	//          (newline characters processed)
	//          const int textLen - length of a text string
	// Output : Returns ID for the given text, using which we can display it,
	//          in the method 'PrintStaticText'
	//---------------------------------------------------------------------------
	virtual int SetStaticText(const char *text, const int textLen) = 0;
	virtual int SetStaticWText(const wchar_t *text, const int textLen) = 0;
	
	//---------------------------------------------------------------------------
	// Purpose: Resets a video memory for the static text
	//          (changes the amount of available video memory, for the static text, 
	//          if the previous value do not equally new value,
	//          specified in a function 'SetMaxStaticChars')
	//---------------------------------------------------------------------------
	virtual void ResetStaticText( void ) = 0;

	//---------------------------------------------------------------------------
	// Purpose: Draws a static text by its descriptor
	// Input  : int idText - ID for the static text
	//---------------------------------------------------------------------------
	virtual void PrintStaticText(int idText = 0) = 0;

	//---------------------------------------------------------------------------
	// Purpose: Draws a dynamic text
	// Input  : const char *text / const wchar_t *text - String of a text
	//          const int textLen - length of a text string 
	//---------------------------------------------------------------------------
	virtual void PrintText(const char *text, const int textLen) = 0;
	virtual void PrintWText(const wchar_t *text, const int textLen) = 0;

	//---------------------------------------------------------------------------
	// Purpose: Kerning 
	//---------------------------------------------------------------------------
	virtual bool HasKerning(HFont handle) = 0;
	virtual void UseKerning(bool flag) = 0;

	//---------------------------------------------------------------------------
	// Purpose: Begins, ends 2D painting (to display any text, all calls draws text
	//          must be between 'EnableStateDraw' / 'DisableStateDraw')
	//---------------------------------------------------------------------------
	virtual void BeginDraw( void ) = 0;
	virtual void EndDraw( void ) = 0;

	/*
		Debug info ...
	*/

	//---------------------------------------------------------------------------
	// Purpose: Returns a total number of vertices per frame
	//          TrianglesCount = VertexPerFrame() / 3
	//---------------------------------------------------------------------------
	virtual int VertexPerFrame( void ) const = 0;
};

extern IFontSystem& FontSystem();


/*  FSGL3 Additional settings:  */

//---------------------------------------------------------------------------
// Purpose: Sets the maximum width of the font texture (as default 1024),
//          height of the texture will be calculated
//          Minimum value: 128
//          Maximum value: 4096
//---------------------------------------------------------------------------
bool SetFontTextureWidth(const int texture_width);

//---------------------------------------------------------------------------
// Purpose: Set the maximum number of characters for the static text (as default 4096)
//          Call before methods 'Initialize()' and 'ResetStaticText()'
//          Minimum value: 256
//          Maximum value: 50000
// 6 vertices(1 char) * float4(x,y,s,t) * sizeof(float) = 96 bytes - to display
// a single character
//---------------------------------------------------------------------------
bool SetMaxStaticChars(const int max_Static_Chars);