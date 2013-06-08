// Copyright (c) 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose:
//
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "public/utils.h"

// Targa header
#pragma pack(1)
typedef struct
{
    unsigned char identsize;       // size of ID field that follows 18 byte header (0 usually)
    unsigned char colourmaptype;   // type of colour map 0=none, 1=has palette
    unsigned char imagetype;       // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

    short         colourmapstart;  // first colour map entry in palette
    short         colourmaplength; // number of colours in palette
    unsigned char colourmapbits;   // number of bits per palette entry 15,16,24,32

    short         xstart;          // image x origin
    short         ystart;          // image y origin
    short         width;           // image width in pixels
    short         height;          // image height in pixels
    unsigned char bits;            // image bits per pixel 8,16,24,32
    unsigned char descriptor;      // image descriptor bits (vh flip bits)

} TGAHeader;
#pragma pack()

namespace utils
{
    bool TGA_Save(const char* fileName, const int width, const int height, unsigned char const* pData)
    {
        if (!fileName || ((width * height) == 0) || !pData)
            return false;

        TGAHeader header;
        int bytewrite = 0;

        FILE* pFile = fopen(fileName, "wb");
        if (!pFile)
            return false;

        const size_t sizeHeader = sizeof(TGAHeader);
        memset(&header, 0, sizeHeader);
        header.imagetype = 3;
        header.width =  (short)width;
        header.height = (short)height;
        header.bits = 8;
        header.descriptor = 0x20;

        header.xstart = 0;
        header.ystart = 0;

        bytewrite = fwrite(&header, 1, sizeHeader, pFile);

        if (bytewrite != sizeHeader)
        {
            fclose(pFile);
            return false;
        }
        const int texDim = width*height;
        bytewrite = fwrite(pData, 1, texDim, pFile);
        if (bytewrite != texDim)
        {
            fclose(pFile);
            return false;
        }
        fclose(pFile);
        return true;
    }

    // TODO: implement using a file system
    bool TGA_Load(const char* fileName, unsigned char **buffer, int& w, int& h)
    {
        if (!fileName)
            return false;

        TGAHeader header;
        int byteread = 0;
        w = h = 0;

        FILE* pFile = fopen(fileName, "rb");
        if (!pFile)
            return false;

        const size_t sizeHeader = sizeof(TGAHeader);
        byteread = fread(&header, 1, sizeHeader, pFile);
        if (byteread != sizeHeader)
        {
            fclose(pFile);
            return false;
        }
        if (header.imagetype != 3)
        {
            fclose(pFile);
            return false;
        }
        w = header.width;
        h = header.height;
        byteread = fread(*buffer, 1, w * h, pFile);
        if (byteread != (w * h))
        {
            fclose(pFile);
            return false;
        }
        fclose(pFile);
        return true;
    }

    int ExtractFileName(const char* path, char* buffer)
    {
        assert(path && buffer);
        int len = strlen(path);
        while (len > 0)
        {
            if (path[len] == '\\' || path[len] == '/')
                break;

            --len;
        }
        const char* beg = &path[len] + 1;
        int chInName(0);
        // count the number of characters in a string name
        while (*beg)
        {
            if (*beg == '.')
                break;

            ++chInName;
            ++beg;
        }
        strncpy(buffer, &path[len] + 1, chInName);
        buffer[chInName] = '\0';
        return chInName;
    }

    int ExtractFilePath(const char* path, char* buffer)
    {
        assert(path && buffer);
        const int len = LenghtPath(path);
        strncpy(buffer, path, len);
        buffer[len] = '\0';
        return len;
    }

    int LenghtPath(const char* path)
    {
        assert(path);
        int len = strlen(path) - 1;
        while (len > 0)
        {
            if (path[len] == '\\' || path[len] == '/')
                break;

            --len;
        }
        return len;
    }

    void FixSlashes(char* str)
    {
        while (*str)
        {
            if(*str == '\\')
            {
                *str = '/';
            }
            str++;
        }
    }

} // End namespace utils