// Copyright (c) 2013 Vitaly Lyaschenko < scxv86@gmail.com >
// Purpose: Demo for a X Font System
//
#include "common.h"

#include "xfontSys.h"

#include <locale>
#include <codecvt>

// the font descriptors
xfs::HFont g_hDejaVuSans_10 = INVALID_FONT;
xfs::HFont g_hFreeSans_14 = INVALID_FONT;
xfs::HFont g_hVerdana_11 = INVALID_FONT;
xfs::HFont g_hVerdanaB_11 = INVALID_FONT;
xfs::HFont g_hVerdanaI_11 = INVALID_FONT;
xfs::HFont g_hFontCurrent = INVALID_FONT;

#define DIR_FONT  "../base/fonts/"

enum { g_Width = 800, g_Height = 600, };

const int g_halfWidth = g_Width / 2;
const int g_halfHeight = g_Height / 2;

bool g_running = true;
bool g_bChangePage = false;

int g_fileID = 0;

void events(int, int);

bool Init(void)
{
    LOG_FILE_INIT("logApp.txt");

    if (!SetWorkingDirectory())
        return false;

    if (!glfwInit())
    {
        LOG_ERROR << "Init a library GLFW";
        return false;
    }

    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    int is_ok = glfwOpenWindow(g_Width, g_Height, 8, 8, 8, 8, 24, 0, GLFW_WINDOW);

    if (!is_ok)
    {
        glfwTerminate();
        LOG_ERROR << "glfwOpenWindow ";
        return false;
    }

    glfwEnable(GLFW_KEY_REPEAT);
    glfwSetWindowTitle("X Font System Demo (OpenGL 3.2)");
    glfwSetWindowPos(200, 100);

    if (!InitGLEW())
        return false;

    PrintVideoInfo();

    // specifies the maximum number of characters for a static text
    xfs::SetMaxStaticChars(3072);

    // initialize XFS
    if (!xfs::Initialize())
    {
        LOG_ERROR << "XFS Initialize";
        return false;
    }
    
    xfs::SetScreenSize(g_Width, g_Height);
    
    glViewport(0, 0, g_Width, g_Height);

    bool isBuild = false;

    // Load and create fonts
    
    g_hFreeSans_14 = xfs::Create_Font(DIR_FONT"FreeSans.ttf", 14);
    assert(g_hFreeSans_14 !=  INVALID_FONT);

    g_hDejaVuSans_10 = xfs::Create_Font(DIR_FONT"DejaVuSans.ttf", 10);
    assert(g_hDejaVuSans_10 !=  INVALID_FONT);

    g_hVerdana_11 = xfs::Create_Font(DIR_FONT"verdana.ttf", 11);
    assert(g_hVerdana_11 !=  INVALID_FONT);

    g_hVerdanaB_11 = xfs::Create_Font(DIR_FONT"verdanab.ttf", 11);
    assert(g_hVerdanaB_11 !=  INVALID_FONT);

    g_hVerdanaI_11 = xfs::Create_Font(DIR_FONT"verdanai.ttf", 11);
    assert(g_hVerdanaI_11 !=  INVALID_FONT);

    // Add character ranges
    
    // 0-127 + 1024-1279
    xfs::AddGlyphSetToFont(g_hFreeSans_14, 31, 127);
    xfs::AddGlyphSetToFont(g_hFreeSans_14, 1024, 1104);

    // only ASCI 0-127
    xfs::AddGlyphSetToFont(g_hDejaVuSans_10, xfs::US_BASIC_LATIN);

    // 0-127 + 1024-1279
    xfs::AddGlyphSetToFont(g_hVerdana_11, xfs::US_BASIC_LATIN);
    xfs::AddGlyphSetToFont(g_hVerdana_11, xfs::US_CYRILLIC);
    

    // 0-127 + 1024-1279
    xfs::AddGlyphSetToFont(g_hVerdanaB_11, xfs::US_BASIC_LATIN);
    xfs::AddGlyphSetToFont(g_hVerdanaB_11, xfs::US_CYRILLIC);

    // 1024-1279 + 0-127
    xfs::AddGlyphSetToFont(g_hVerdanaI_11, xfs::US_CYRILLIC);
    xfs::AddGlyphSetToFont(g_hVerdanaI_11, xfs::US_BASIC_LATIN);

    xfs::SetFontTextureWidth(1024);

    isBuild = xfs::BuildAllFonts();

    if (!isBuild)
        return false;

    g_hFontCurrent = g_hVerdana_11;
    return true;
}

int InitStaticTextFromFile(const char* nameFile, xfs::BBox& bbox)
{
    const char* pText = ReadWholeFileIntoString(nameFile);
    if (!pText)
        return -1;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    auto wstr = conv.from_bytes(pText);
    delete[] pText;

    xfs::BindFont(g_hFontCurrent);

    xfs::SetTextPos(2, 75);

    // gets the bounding box for the text from a file
    xfs::GetWTextBBox(wstr.c_str(), wstr.length(), bbox);

    xfs::SetTextColor(255, 0, 0);

    // transmit the static text
    int idText = xfs::SetStaticWText(wstr.c_str(), wstr.length());

    xfs::BindFont(0);
    wstr.clear();
    return idText;
}

int main(int argc, char* argv[])
{
    if (!Init())
        exit(1);

    char buffer[1024];
    int vertexPerFrame(0);
    const char NameFile [] = "../base/text/GF_Info_";
    sprintf(buffer, "%s%d.txt", NameFile, g_fileID);
    xfs::BBox bboxTextFromFile;

    int idTextFromFile(0);
    idTextFromFile = InitStaticTextFromFile(buffer, bboxTextFromFile);
    if (idTextFromFile == -1)
        return -1;

    std::string time;
    int mouseX(0);
    int mouseY(0);
    int currLen(0);

    xfs::BindFont(g_hFreeSans_14);

    xfs::BBox bboxTextLink;
    const char strLink[] = " shaman-code.blogspot.com ";

    xfs::SetTextPos(1, 1);
    xfs::GetTextBBox(strLink, strlen(strLink), bboxTextLink);
    xfs::SetTextColor(10, 158, 10);
    const int len = strlen(strLink);
    int idTextLink = xfs::SetStaticText(strLink, len);

    NowTime(time);

    xfs::BBox bboxTime;

    xfs::SetTextPos(bboxTextLink.maxX + 2, 1);
    xfs::GetTextBBox(time.c_str(), time.length(), bboxTime);

    const char strHelp[] = "F1 - F3 - change font for the text\n" \
                         "\nKey Right/Left Space - show the next/prev block of text\n" \
                         "\nCtrl - use kerning";

    xfs::BindFont(g_hDejaVuSans_10);

    xfs::SetTextPos(bboxTime.maxX + 25, 1);
    xfs::SetTextColor(0, 0, 255);
    int idTextHelp = xfs::SetStaticText(strHelp, strlen(strHelp));

    xfs::BindFont(0);

    while (g_running)
    {
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // Begins painting (bind shader program, clear all state etc.)
        xfs::Draw2D_Begin();

        // (Dynamic text) Debug info
        xfs::BindFont(g_hDejaVuSans_10);

        currLen = sprintf(buffer, "Debug Info:\nFPS : %.3f\nVertexPerFrame : %i", GetFPS(1.0), vertexPerFrame);

        xfs::SetTextPos(1, bboxTextLink.maxY + 1);
        xfs::SetTextColor(255, 0, 255);
        xfs::PrintText(buffer, currLen);

        xfs::BindFont(0);
        xfs::PrintStaticText(idTextFromFile); // (Static  text) Text from File
        xfs::PrintStaticText(idTextLink);     // (Static  text) link
        xfs::PrintStaticText(idTextHelp);     // (Static  text) Help 

        xfs::BindFont(g_hFreeSans_14);        // font to display the time
        static float time0 = 0;
        const float currTime = glfwGetTime();

        if ((currTime - time0) > 1.0)         // time is updated once per second
        {
            NowTime(time);
            time0 = glfwGetTime();
        }

        // color of the rectangle
        xfs::SetTextPos(bboxTime.minX, bboxTime.minY);
        xfs::SetTextColor(0, 0, 0);
        xfs::PrintText(time.c_str(), time.length());
        xfs::BindFont(0);

        // (Dynamic text) Cursor position
         
        xfs::BindFont(g_hFreeSans_14);

        glfwGetMousePos(&mouseX, &mouseY);

        if((mouseX < g_halfWidth) && (mouseY < g_halfHeight))
        {
            xfs::SetTextPos(mouseX+15, mouseY+10);
        } 
        else if((mouseX > g_halfWidth) && (mouseY < g_halfHeight))
        {
            xfs::SetTextPos(mouseX-100, mouseY+10);
        } 
        else if((mouseX < g_halfWidth) && (mouseY > g_halfHeight))
        {
            xfs::SetTextPos(mouseX+15, mouseY-20);
        }
        else
        {
            xfs::SetTextPos(mouseX-100, mouseY-20);
        }

        currLen = sprintf(buffer, "x:%i y:%i ", mouseX, mouseY);

        xfs::SetTextColor(255, 0, 255);
        xfs::PrintText(buffer, currLen);

        if (g_bChangePage)
        {
            // Reinitializing static text

            xfs::ResetStaticText(); 

            if ((g_fileID >= 3) || (g_fileID < 0))
                g_fileID = 0;
            
            sprintf(buffer, "%s%d.txt", NameFile, g_fileID);

            idTextFromFile = InitStaticTextFromFile(buffer, bboxTextFromFile);
            if (idTextFromFile == -1)
                return -1;

            xfs::BindFont(g_hFreeSans_14);
            
            xfs::SetTextPos(1, 1);
            xfs::SetTextColor(10, 158, 10);
            const int len = strlen(strLink);
            idTextLink = xfs::SetStaticText(strLink, len);

            xfs::BindFont(g_hDejaVuSans_10);

            xfs::SetTextPos( bboxTime.maxX + 25, 1 );
            xfs::SetTextColor(0, 0, 255);
            idTextHelp = xfs::SetStaticText(strHelp, strlen(strHelp));
            g_bChangePage = false;
        }

        xfs::Draw2D_End();
        vertexPerFrame = xfs::VertexPerFrame();

        glfwSetKeyCallback(events);
        glfwSwapBuffers();

        g_running = !glfwGetKey(GLFW_KEY_ESC) &&
                    glfwGetWindowParam(GLFW_OPENED);
    }

    glfwCloseWindow();
    xfs::Release();
    glfwTerminate();
}

void events(int key, int action)
{
    switch (key)
    {
    case GLFW_KEY_SPACE:
        if (glfwGetKey(GLFW_KEY_SPACE) == GLFW_RELEASE)
        {
            g_bChangePage = true;
            ++g_fileID;
        }
        break;
    case GLFW_KEY_RIGHT:
        if (glfwGetKey(GLFW_KEY_RIGHT) == GLFW_RELEASE)
        {
            g_bChangePage = true;
            ++g_fileID;
        }
        break;
    case GLFW_KEY_LEFT:
        if (glfwGetKey(GLFW_KEY_LEFT) == GLFW_RELEASE)
        {
            g_bChangePage = true;
            --g_fileID;
        }
        break;
    case GLFW_KEY_F1:
        if (glfwGetKey(GLFW_KEY_F1) == GLFW_RELEASE)
        {
            g_hFontCurrent = g_hVerdana_11;
            g_bChangePage = true;
        }
        break;
    case GLFW_KEY_F2:
        if (glfwGetKey(GLFW_KEY_F2) == GLFW_RELEASE)
        {
            g_hFontCurrent = g_hVerdanaB_11;
            g_bChangePage = true;
        }
        break;
    case GLFW_KEY_F3:
        if (glfwGetKey(GLFW_KEY_F3) == GLFW_RELEASE)
        {
            g_hFontCurrent = g_hVerdanaI_11;
            g_bChangePage = true;
        }
        break;
    case GLFW_KEY_LCTRL:
        if (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_RELEASE)
        {
            static bool useKerning = false;
            xfs::UseKerning(useKerning ? useKerning = false : useKerning = true);
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
