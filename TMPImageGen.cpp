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
int GetOptimizationLevel(int Idx, int* ProcessedPixelsList);
void MarkProcessedPixel(int Idx, int OptimizationLevel, int* ProcessedPixelsList);
void GenerateTMP();
std::string itohexstr(unsigned int i, unsigned int bytes = 2);

struct RGBA
{
public:
    int r, g, b, a;

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

std::vector<TMPCommand> CommandList;


Image a = { 0, 0, {} };
Image& Img = a;

char imagePath[MAX_PATH];
FILE* outFile;

int main(int argc, char** argv)
{
    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    if (argc <= 1) {
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
    else {
        memcpy(imagePath, argv[1], strlen(argv[1]));
        printf("image file selected: \"%s\"\n", imagePath);
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
            if (argv[0][idx] == '\0') {
                stage += 1;
            }
            else {
                idx += 1;
                continue;
            }
        }
        if (stage == 5) {
            if (argv[0][idx] == '\\') {
                filePath = new char[idx + 1 + (fileNameEnd - fileNameStart + 1) + 1 + 4];
                ZeroMemory(filePath, idx + 1 + (fileNameEnd - fileNameStart + 1) + 1 + 4);
                memcpy(filePath, argv[0], idx + 1);
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
    printf("created file: %s\n", filePath);



    for (int i = 0; i < Img.width * Img.height; i++) {
        printf("%u\t%u\n",Img.data[i].r, i);
    }

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
            if (x == 0)
                printf("\n");
            printf(optimizationLevel == -1 ? "# " : "%d ", optimizationLevel);
        }
        else {
            CommandList.push_back(TMPCommand(x, y, 1, Img.data[idx]));
        }
    }printf("\n");

    printf("[");
    for (auto& cmd : CommandList) {
        printf("(%d, %d, %d, (%d,%d,%d,%d)),", cmd.x, cmd.y, cmd.size, cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);
    }
    printf("]\n");

    GenerateTMP();

    delete[] filePath;
    delete[] processedPixels;
    std::cout << "Done!\n";
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

            double voffset = (y * SymbolHeight) / cmd.size;

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

            out = "<voffset=-" + std::to_string(voffset) + "e>\n";
            fwrite(out.c_str(), out.length(), 1, outFile);
        }


        double pos = (cmd.x * SymbolWidth) / cmd.size;
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
    std::cout << out << std::endl;
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
