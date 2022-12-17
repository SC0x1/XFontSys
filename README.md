XFS is a library of font management (FreeType2 based),
which allows you to flexibly combine different fonts to display text.

XFS does not use the C runtime library, so it does not matter which version
(single-threaded, multi-threaded or multi-threaded DLL) it is linked with.

Depends
------

FreeType 2
http://freetype.org/

##Usage Example

####1. Initialize and configure

```c++
// Initialize
xfs::Initialize();

// Pass the window size
xfs::SetScreenSize(WIDTH, HEIGHT);

// Create a font from a TrueType file, set it size and obtain its handle
HFont hFreeSans_14 = xfs::Create_Font("YourPath/FreeSans.ttf", 14);

// If something went wrong
assert(hFreeSans_14 != INVALID_FONT);

// Adds the character ranges for fonts, using the obtained handle
xfs::AddGlyphSetToFont(hFreeSans_14, BASIC_LATIN);
xfs::AddGlyphSetToFont(hFreeSans_14, CYRILLIC);

// or like this
xfs::AddGlyphSetToFont(hFreeSans_14, 31, 127);
xfs::AddGlyphSetToFont(hFreeSans_14, 1024, 1104);

// Now simply call
xfs::BuildAllFonts();
```

The resulting texture for these ranges:

![alt text](https://3.bp.blogspot.com/-9D3EECHkULM/WcZ5FsJrBpI/AAAAAAAAJps/MUm-aGrw1jc4f8gD_xucgoLAkkoz71-uwCKgBGAs/s1600/FreeSans14.png "The font texture atlas")

####2. Text Rendering

```c++
// Unicode text string
const wchar_t wText[] = L"Hello World\nили Здравствуй мир! :)";

// Bind the font handle
xfs::BindFont(hFreeSans_14);

// Sets position for the text in screen space
xfs::SetTextPos(100, 25);

// Sets text color
xfs::SetTextColor(255, 0, 200);

// Pass a string of text and obtain its id
int staticTextID = xfs::SetStaticWText(wText, wcslen(wText));

// then at runtime
while(true)
{
	// Bind the shader program, clear all state etc.
	xfs::Draw2D_Begin();

	// Draw a static text
	xfs::PrintStaticText(staticTextID);

	// Bind a next font handle
	xfs::BindFont(hVerdanaB_11);

	xfs::SetTextColor(0, 0, 255);
	xfs::SetTextPos(100, 10);

	std::string time;
	NowTime(time);

	// Draw a dynamic text string
	xfs::PrintText(time.c_str(), time.length());

	xfs::Draw2D_End();
}
```

As a result:

![alt text](https://3.bp.blogspot.com/-UgBZ9bNj_DI/WcZ54HEn7kI/AAAAAAAAJqE/OTkYTaO5OZ0-F7xch9obiLZkP_IMHnqWQCKgBGAs/s1600/scrHelloWorld.png "Rendering text result")

For any text you can get its bounding box (BBox).

```c++
// for example, load any text from a file
std::wstring wstr = ReadWholeFileIntoString("SomeFile.txt");

// pass it and obtain its index (ID)
int textID = FontSystem.SetStaticWText(wstr.c_str(), wstr.lenght());

// obtain its bounding box
BBox_t bbox;
xfs::GetWTextBBox(wstr.c_str(), wstr.lenght(), bbox);

// draws the entire contents of the file
xfs::PrintStaticText(textID);
```

Result:

![alt text](https://4.bp.blogspot.com/-BVU7wCn_8kc/WcZ5_taQBxI/AAAAAAAAJqI/XmtWB5T1EEYEQQ802iBO0aXt7SgvW4WrwCKgBGAs/s1600/scrMSL.png "Rendering text result")

##License

[GNU Library or Lesser General Public License (LGPL)](http://www.gnu.org/licenses/lgpl.html "License")
