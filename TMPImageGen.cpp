// TMPImageGen.cpp : c++ version +it works +not yet +faster +better +stronger
//
#define SHOULD_TRY_OPTIMIZE true

#include <Windows.h>
#include <tchar.h>

#include <thread>
#include <chrono>

#include <iostream>
#include <vector>
#include <string>


#pragma comment(lib, "spng.lib")
#include "spng/spng.h"



int GetOptimizationLevel(int Idx, int* ProcessedPixelsList);
void MarkProcessedPixel(int Idx, int OptimizationLevel, int* ProcessedPixelsList);
void GenerateTMP();
std::string itohexstr(unsigned int i, unsigned int bytes = 2);

struct RGBA
{
public:
    unsigned char r, g, b, a;

    RGBA() : r(0), g(0), b(0), a(0) {};
    RGBA(int _r, int _g, int _b, int _a) : r(_r), g(_g), b(_b), a(_a) {};

    bool operator==(RGBA& other)
    {
        return (r == other.r && g == other.g && b == other.b && a == other.a);
    }
    bool operator!=(RGBA& other)
    {
        return (!(r == other.r && g == other.g && b == other.b && a == other.a));
    }

    std::string HexString(bool alpha = true)
    {
        return std::string("#") + itohexstr(r) + itohexstr(g) + itohexstr(b) + (alpha == true ? itohexstr(a) : "");
    }
};
struct Image
{
public:
    int width, height;
    std::vector<RGBA> data;

    Image(int w, int h, std::vector<RGBA>pixels) : width(w), height(h), data(pixels) {};
};
struct TMPCommand
{
public:
    int x, y;
    int size;
    RGBA color;

    TMPCommand(int x, int y, int size, RGBA color) : x(x), y(y), size(size), color(color) {};
};

int ReadPng(char* filePath);

std::vector<TMPCommand> CommandList;


Image Img = { 0, 0, {} };

char imagePath[MAX_PATH];
FILE* outFile;

FILE* png;
int ret = 0;
spng_ctx* ctx = NULL;
unsigned char* image = NULL;
double OffsetX = 0, OffsetY = 0;

int main(int argc, char** argv)
{
    char execPath[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, execPath, MAX_PATH);
    printf("\ngot executable path: %s\n", execPath);

    bool pathFromArgv = false;
    if (argc > 1) {
        {
            if (argv[1][1] == ':') {
                memcpy(imagePath, argv[1], strlen(argv[1]));
                printf("image path copied from first arg: \"%s\"\n", imagePath);
                pathFromArgv = true;
            }
            
            printf("checking args\n");
            printf("argc: %d\n", argc);
            for (int i = 0; i < argc; i++) {
                printf("argv[%d]: %s\n", i, argv[i]);
                if (!strncmp(argv[i], "-x", 2)) {
                    OffsetX = static_cast<double>(atoi(argv[i] + 2));
                    continue;
                }
                if (!strncmp(argv[i], "-y", 2)) {
                    OffsetY = static_cast<double>(atoi(argv[i] + 2));
                    continue;
                }
                if (!strcmp(argv[i], "-AskOffset")) {
                    char tempBuf[256];

                    ZeroMemory(tempBuf, 256);
                    printf("x offset: ");
                    std::cin >> tempBuf;
                    printf("\n");
                    OffsetX = atoi(tempBuf);

                    ZeroMemory(tempBuf, 256);
                    printf("y offset: ");
                    std::cin >> tempBuf;
                    printf("\n");
                    OffsetY = atoi(tempBuf);
                    continue;
                }
            }
        }
    }
    if (!pathFromArgv) {
        OPENFILENAMEA ofn;
        ZeroMemory(&imagePath, sizeof(imagePath));
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
        ofn.lpstrFilter = ("PNG File\0*.png\0");
        ofn.lpstrFile = imagePath;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrTitle = ("Select Image File");
        ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
        if (GetOpenFileNameA(&ofn)) {
            printf("image file selected: \"%s\"\n", imagePath);
        }
        else {
            // All this stuff below is to tell you exactly how you messed up above. 
            // Once you've got that fixed, you can often (not always!) reduce it to a 'user cancelled' assumption.
            switch (CommDlgExtendedError()) {
            case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
            case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
            case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
            case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
            case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
            case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
            case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
            case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
            case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
            case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
            case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
            case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
            case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
            case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
            case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
            default: std::cout << "You cancelled.\n";
            }
            return 1;
        }
    }

    char* filePath = 0;
    bool pathCreated = false;
    int fileNameStart = 0, fileNameEnd = 0, idx = 0;

    int stage = 0;
    while (!pathCreated) {
        if (stage == 0) {
            if (imagePath[idx] == '\0') {
                stage += 1;
                fileNameEnd = idx - 1;
            }
            else {
                idx += 1;
                continue;
            }
        }
        if (stage == 1 || stage == 2) {
            if (imagePath[idx] == '.') {
                if (stage == 1) {
                    fileNameEnd = idx - 1;
                    stage += 1;
                    idx -= 1;
                }
            }
            else if (imagePath[idx] == '\\') {
                fileNameStart = idx + 1;
                stage = 3;
            }
            else {
                idx -= 1;
                continue;
            }
        }
        if (stage == 3) {
            idx = 0;
            stage += 1;
        }
        if (stage == 4) {
            if (execPath[idx] == '\0') {
                stage += 1;
            }
            else {
                idx += 1;
                continue;
            }
        }
        if (stage == 5) {
            if (execPath[idx] == '\\') {
                filePath = new char[idx + 1 + (fileNameEnd - fileNameStart + 1) + 1 + 4];
                ZeroMemory(filePath, idx + 1 + (fileNameEnd - fileNameStart + 1) + 1 + 4);
                memcpy(filePath, execPath, idx + 1);
                memcpy(filePath + idx + 1, imagePath + fileNameStart, fileNameEnd - fileNameStart + 1);
                memcpy(filePath + idx + 1 + (fileNameEnd - fileNameStart + 1), ".txt", 4);
                printf("assembled path: %s\n", filePath);
                stage = 6;
                pathCreated = true;
                continue;
            }
            else {
                idx -= 1;
                continue;
            }
        }
    }

    if (fopen_s(&outFile, filePath, "w")) {
        printf("***ERROR*** COULDN'T CREATE FILE: %s\n", filePath);
    }
    printf("created output file: %s\n", filePath);

    if (!ReadPng(imagePath)) {

        int* processedPixels = new int[Img.width * Img.height];
        memset(processedPixels, 0, Img.width * Img.height * sizeof(int));

        for (int idx = 0; idx < Img.width * Img.height; idx++) {
            int x = idx % Img.width;
            int y = (int)(idx / Img.width);

            if (SHOULD_TRY_OPTIMIZE) {
                int optimizationLevel = GetOptimizationLevel(idx, processedPixels);
                if (optimizationLevel != -1) {
                    //printf("added command: %d    at (%d,%d)\n", optimizationLevel, x, y);
                    CommandList.push_back(TMPCommand(x, y, optimizationLevel, Img.data[idx]));
                }

                MarkProcessedPixel(idx, optimizationLevel, processedPixels);
                /*
                if (x == 0)
                    printf("\n");
                printf(optimizationLevel == -1 ? "# " : "%d ", optimizationLevel);
                */
            }
            else {
                CommandList.push_back(TMPCommand(x, y, 1, Img.data[idx]));
            }
        }
        //printf("\n");

        /*
        printf("[");
        for (auto& cmd : CommandList) {
            printf("(%d, %d, %d, (%d,%d,%d,%d)),", cmd.x, cmd.y, cmd.size, cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);
        }
        printf("]\n");
        */

        GenerateTMP();

        delete[] processedPixels;
    }
    delete[] filePath;

    spng_ctx_free(ctx);

    std::cout << "Done!\n";
}

int ReadPngError()
{
    Img.width = 0;
    Img.height = 0;
    Img.data.clear();
    spng_ctx_free(ctx);
    free(image);
    return 1;
}

const char* color_type_str(uint8_t color_type)
{
    switch ((spng_color_type)color_type) {
    case SPNG_COLOR_TYPE_GRAYSCALE: return "grayscale";
    case SPNG_COLOR_TYPE_TRUECOLOR: return "truecolor";
    case SPNG_COLOR_TYPE_INDEXED: return "indexed color";
    case SPNG_COLOR_TYPE_GRAYSCALE_ALPHA: return "grayscale with alpha";
    case SPNG_COLOR_TYPE_TRUECOLOR_ALPHA: return "truecolor with alpha";
    default: return "(invalid)";
    }
}

int ReadPng(char* filePath)
{
    fopen_s(&png, filePath, "rb");

    if (png == NULL) {
        printf("error opening input file %s\n", filePath);
        return ReadPngError();
    }

    ctx = spng_ctx_new(0);

    if (ctx == NULL) {
        printf("spng_ctx_new() failed\n");
        return ReadPngError();
    }

    /* Ignore and don't calculate chunk CRC's */
    spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);

    /* Set memory usage limits for storing standard and unknown chunks,
       this is important when reading untrusted files! */
    size_t limit = 1024 * 1024 * 64;
    spng_set_chunk_limits(ctx, limit, limit);

    /* Set source PNG */
    spng_set_png_file(ctx, png); /* or _buffer(), _stream() */

    struct spng_ihdr ihdr;
    ret = spng_get_ihdr(ctx, &ihdr);

    if (ret) {
        printf("spng_get_ihdr() error: %s\n", spng_strerror(ret));
        return ReadPngError();
    }

    const char* color_name = color_type_str(ihdr.color_type);

    printf("width: %u\n"
        "height: %u\n"
        "bit depth: %u\n"
        "color type: %u - %s\n",
        ihdr.width, ihdr.height, ihdr.bit_depth, ihdr.color_type, color_name);

    Img.width = ihdr.width;
    Img.height = ihdr.height;

    printf("compression method: %u\n"
        "filter method: %u\n"
        "interlace method: %u\n",
        ihdr.compression_method, ihdr.filter_method, ihdr.interlace_method);

    struct spng_plte plte = { 0 };
    ret = spng_get_plte(ctx, &plte);

    if (ret && ret != SPNG_ECHUNKAVAIL) {
        printf("spng_get_plte() error: %s\n", spng_strerror(ret));
        return ReadPngError();
    }

    if (!ret) printf("palette entries: %u\n", plte.n_entries);


    size_t image_size;

    /* Output format, does not depend on source PNG format except for
       SPNG_FMT_PNG, which is the PNG's format in host-endian or
       big-endian for SPNG_FMT_RAW.
       Note that for these two formats <8-bit images are left byte-packed */
    int fmt = SPNG_FMT_PNG;

    /* With SPNG_FMT_PNG indexed color images are output as palette indices,
       pick another format to expand them. */
    if (ihdr.color_type == SPNG_COLOR_TYPE_INDEXED) fmt = SPNG_FMT_RGB8;

    ret = spng_decoded_image_size(ctx, fmt, &image_size);

    if (ret) {
        printf("spng_decoded_image_size() error\n");
        return ReadPngError();
    }

    //image = (unsigned char*)malloc(image_size);
    Img.data.resize(image_size);
    image = (unsigned char*)Img.data.data();

    if (image == NULL) return ReadPngError();

    /* Decode the image in one go */
     ret = spng_decode_image(ctx, image, image_size, SPNG_FMT_RGBA8, 0);

    if(ret)
    {
        printf("spng_decode_image() error: %s\n", spng_strerror(ret));
        return ReadPngError();
    }

    printf("Successfully read png data :)\n");
    return 0;
}

int GetOptimizationLevel(int Idx, int* ProcessedPixelsList)
{
    int level = 0;
    if (ProcessedPixelsList[Idx] != 0) {
        return -1;
    }
    int newLevel = level + 1;
    while (Idx + (newLevel - 1 /*fix bottom right corner*/)
        + Img.width * (newLevel - 1 /*fix bottom*/) < Img.width * Img.height) {

        for (int x = 0; x < newLevel; x++) {
            for (int y = 0; y < newLevel; y++) {
                if (Img.data[Idx] != Img.data[Idx + x + y * Img.width]) {
                    return level;
                }
            }
        }
        if (Idx % Img.width + newLevel > Img.width) {
            return level;
        }
        level = newLevel;
        newLevel += 1;
    }

    return level;
}

void MarkProcessedPixel(int Idx, int OptimizationLevel, int* ProcessedPixelsList)
{
    for (int x = 0; x < OptimizationLevel; x++) {
        for (int y = 0; y < OptimizationLevel; y++) {
            if (x == 0 && y == 0) {
                ProcessedPixelsList[Idx + x + y * Img.width] = OptimizationLevel;
                continue;
            }
            ProcessedPixelsList[Idx + x + y * Img.width] = -1;
        }
    }
}

void GenerateTMP()
{

    std::string SymbolChar = "@";
    double SymbolXFixBase = 0.4505;
    double SymbolXFixMargin = 0.151;
    double SymbolXFixMargin2 = 0.0005;
    double SymbolYFixBase = 0.4525;
    double SymbolYFixMargin = 0.151;
    double SymbolYFixMargin2 = 0.0005;
    double SymbolWidth = 1.78;
    double SymbolHeight = 1.117;


    std::string header = { "<line-height=0><width=0><align=center>\n<mark=#000f><color=#000f>\n" };
    fwrite(header.c_str(), header.length(), 1, outFile);

    std::string out = ""; //header;

    int size = 1;
    int y = -1;
    RGBA c(-1, -1, -1, -1);
    for (auto& cmd : CommandList) {
        if (cmd.color.a < (0xff / 3))
            continue;
        if (c != cmd.color) {
            c = cmd.color;

            /*
            static int lastAlpha = 0xFF;
            if (c.a != lastAlpha) {
                out +=
                    "<color="
                    + c.HexString() +
                    ">";
                lastAlpha = c.a;
            }
            */
            out =
                "<mark="
                + c.HexString() +
                ">";
            fwrite(out.c_str(), out.length(), 1, outFile);
        }

        if (size != cmd.size) {
            out = "\n<size=" + std::to_string(cmd.size) + "e>";
            fwrite(out.c_str(), out.length(), 1, outFile);
        }

        if (y != cmd.y || size != cmd.size) {
            y = cmd.y;

            double voffset = (y * SymbolHeight + OffsetY) / cmd.size;

            if (cmd.size > 1) {
                voffset += SymbolYFixBase;
                double fixer = SymbolYFixMargin;
                double fixer2 = SymbolYFixMargin2;

                for (int i = 0; i < cmd.size - 2; i++) {
                    voffset += fixer + fixer2;
                    fixer = fixer / 2;
                    fixer += SymbolYFixMargin2*2;
                }
            }

            out = "<voffset=" + std::to_string(-voffset) + "e>\n";
            fwrite(out.c_str(), out.length(), 1, outFile);
        }


        double pos = (cmd.x * SymbolWidth + OffsetX) / cmd.size;
        if (cmd.size > 1) {
            pos += SymbolXFixBase;
            double fixer = SymbolXFixMargin;
            double fixer2 = SymbolXFixMargin2;

            for (int i = 0; i < cmd.size - 2; i++) {
                pos += fixer + SymbolXFixMargin2;
                fixer = fixer / 2;
                fixer += SymbolXFixMargin2*2;

            }
        }
        out = "<pos=" + std::to_string(pos) + "e>";
        out += SymbolChar;
        fwrite(out.c_str(), out.length(), 1, outFile);

        size = cmd.size;

    }

    printf("\n");
}

std::string itohexstr(unsigned int i, unsigned int bytes)
{
    std::string out = "";
    while (bytes > 0) {
        std::string buf = "?";
        unsigned int temp = i - (i & 0xFFFFFFF0);
        buf = (temp > 9 ? (char)(temp - 10 + 'A') : (char)(temp + '0'));
        out = std::string(buf) + out;
        i >>= sizeof(int);
        bytes -= 1;
    }
    return out;
}
