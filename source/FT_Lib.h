/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: Wrap a function the FreeType 2
/*
/****************************************************************************/

#pragma once

// Forward declaration for a freetype type
typedef struct FT_FaceRec_ *FT_Face;

namespace ftLib
{
	class FTFace
	{
	public:

		FTFace( void );
		~FTFace( void );

		bool CreateFace(const char* fontFile, int index = 0);
		void SetPixelSize(int fontSize, int horz_res = 96, int vert_res = 96);

		// render the glyph
		bool Glyph(const int wch);

		// glyph metrics
		int GlyphAdvanceX( void ) const;
		int GlyphAdvanceX(const int wch_prev, const int wch_next);
		int GlyphAdvanceY( void ) const;
		int GlyphAdvanceY(const int wch_prev, const int wch_next);

		int GlyphHeight( void ) const;
		int GlyphWidth( void ) const;

		int GlyphBearingX( void ) const;
		int GlyphBearingY( void ) const;

		int GlyphAscender( void ) const;
		int GlyphDescender( void ) const;

		
		int GlyphBitmapWidth( void ) const;
		int GlyphBitmapHeight( void ) const;
		int GlyphBitmapLeft( void ) const;
		int GlyphBitmapTop( void ) const;
		int GlyphBitmapPith( void ) const;

		// gets the glyph bitmap
		unsigned char const* GlyphBitmap( void ) const;

	private:

		FT_Face m_Face;
	};

	// Initialize/Shutdown the FreeType 2
	bool	InitFT2Lib( void );
	void	DoneFT2Lib( void );
};
