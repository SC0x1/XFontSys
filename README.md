FSGL3 is a cross-platform C++ text rendering library for OpenGL 3.2 and above with Unicode support.

Note: The library does not use the C runtime library, it does not matter which version (single-threaded, multi-threaded or multi-threaded DLL) it is linked.

Depends
------

FreeType 2
http://freetype.org/

##Usage Example

####1. Initialize and configure

C++:

```c++
// Initialize
FontSystem().Initialize();

// Pass the window size
FontSystem().SetScreenSize( SCREEN_WIDTH, SCREEN_HEIGHT );

// Create a font from a TrueType file, set it size and obtain its handle
HFont hFreeSans_14 = FontSystem().Create_Font( "YourPath/FreeSans.ttf", 14 );

// If something went wrong
assert( hFreeSans_14 != INVALID_FONT );

// Adds the character ranges for fonts, using the obtained handle
FontSystem().AddGlyphSetToFont( hFreeSans_14, BASIC_LATIN );
FontSystem().AddGlyphSetToFont( hFreeSans_14, CYRILLIC );

// or like this
FontSystem().AddGlyphSetToFont( hFreeSans_14, 31, 127 );
FontSystem().AddGlyphSetToFont( hFreeSans_14, 1024, 1104 );

// Now simply call
FontSystem().BuildAllFonts();
```

The resulting texture for these ranges:

![alt text](https://dl.dropbox.com/u/45284518/ShamanCode/TextRederSystem/FreeSans14.png "The font texture atlas")

You can also save a cache font (metrics glyphs + texture) on a disk (using font handle),
as shown below:

```c++
// Just dump font cache to the disk
FontSystem().DumpFontCache( hFreeSans_14, "./YourPath/" );

// For load it
hFreeSans_14 = FontSystem().LoadFontCache( "./YourPath/FreeSans_14.cfnt" );

// After the load, you must build a font cache
FontSystem().BuildCache();
```

####2. Rendering text

C++:

```c++
// Unicode text string
const wchar_t WText[] = L"Hello World\nили Здравствуй мир! :)";

// Bind the font handle
FontSystem().BindFont( hFreeSans_14 );

// Sets position for the text in screen space
FontSystem().SetTextPos(100, 25);

// Sets text color
FontSystem().SetTextColor(255, 0, 200);

// Pass a string of text and obtain its id
int id_SText = FontSystem().SetStaticWText(WText, wcslen(WText));

// then at runtime
while( run )
{
	// Bind the shader program, clear all state etc.
	FontSystem().EnableStateDraw();

	// Draw a static text
	FontSystem().PrintStaticText( id_SText );

	// Bind a next font handle
	FontSystem().BindFont( hVerdanaB_11 );

	FontSystem().SetTextColor(0, 0, 255);
	FontSystem().SetTextPos(100, 10);

	std::string time;
	NowTime(time);

	// Draw a dynamic text
	FontSystem().PrintText( time.c_str(), time.length() );

	FontSystem().DisableStateDraw();
}
```

As a result:

![alt text](https://dl.dropbox.com/u/45284518/ShamanCode/TextRederSystem/scrHelloWorld.png "Rendering text result")

For any text you can get it bounding box (BBox).

```c++
// for example, load any text from a file
std::wstring wstr = ReadWholeFileIntoString( "SomeFile.txt" );

// pass it and obtain its index (id)
int idText = FontSystem.SetStaticWText( wstr.c_str(), wstr.lenght() );

// obtain its bounding box
BBox_t bbox;
FontSystem().GetWTextBBox( wstr.c_str(), wstr.lenght(), bbox );

// simply draws the entire contents of the file and its bbox
FontSystem().DrawOutLinedRect( bbox );
FontSystem().PrintStaticText( idText );
```

Result:

![alt text](https://dl.dropbox.com/u/45284518/ShamanCode/TextRederSystem/scrMSL.png "Rendering text result")

##License

[GNU Library or Lesser General Public License (LGPL)](http://www.gnu.org/licenses/lgpl.html "License")

