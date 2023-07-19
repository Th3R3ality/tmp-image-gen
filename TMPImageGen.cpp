// TMPImageGen.cpp : c++ version +it works +not yet +faster +better +stronger
//
#define SHOULD_TRY_OPTIMIZE true

#include <iostream>
#include <vector>
#include <string>

struct RGBA {
public:
    int r, g, b, a;

    RGBA(int _r, int _g, int _b, int _a) : r(_r), g(_g), b(_b), a(_a) {};

    bool operator==(RGBA& other) {
        return (r == other.r && g == other.g && b == other.b && a == other.a);
    }
    bool operator!=(RGBA& other) {
        return (!(r == other.r && g == other.g && b == other.b && a == other.a));
    }
};
struct Image {
public:
    int width, height;
    std::vector<RGBA> data;
    
    Image(int w, int h, std::vector<RGBA>pixels) : width(w), height(h), data(pixels) {};
};
struct TMPCommand {
public:
    int x, y;
    int size;
    RGBA color;

    TMPCommand(int x, int y, int size, RGBA color) : x(x), y(y), size(size), color(color) {};
};

int GetOptimizationLevel(int Idx, int* ProcessedPixelsList);
void MarkProcessedPixel(int Idx, int OptimizationLevel, int* ProcessedPixelsList);
void GenerateTMP();

Image Img5x5 = { 5, 5, { RGBA(255, 0, 0, 255), RGBA(0, 255, 0, 255), RGBA(0, 0, 255, 255), RGBA(0, 0, 0, 255), RGBA(255, 255, 255, 255), RGBA(255, 106, 0, 255), RGBA(0, 255, 144, 255), RGBA(178, 0, 255, 255), RGBA(64, 64, 64, 255), RGBA(255, 255, 255, 200), RGBA(255, 216, 0, 255), RGBA(0, 255, 255, 255), RGBA(255, 0, 220, 255), RGBA(128, 128, 128, 255), RGBA(255, 255, 255, 150), RGBA(182, 255, 0, 255), RGBA(0, 148, 255, 255), RGBA(255, 0, 110, 255), RGBA(160, 160, 160, 255), RGBA(255, 255, 255, 100), RGBA(76, 255, 0, 255), RGBA(0, 38, 255, 255), RGBA(255, 0, 0, 255), RGBA(192, 192, 192, 255), RGBA(255, 255, 255, 50) } };
Image Img10x10 = { 10, 10, { RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 0, 0, 255), RGBA(255, 0, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 0, 0, 255), RGBA(255, 0, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 0, 0, 255), RGBA(255, 0, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 0, 0, 255), RGBA(255, 0, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(76, 255, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 0, 0, 255), RGBA(255, 0, 0, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(0, 38, 255, 255), RGBA(0, 38, 255, 255), RGBA(0, 38, 255, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 0, 0, 255), RGBA(255, 0, 0, 255), RGBA(178, 0, 255, 255), RGBA(178, 0, 255, 255), RGBA(0, 38, 255, 255), RGBA(0, 38, 255, 255), RGBA(0, 38, 255, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(0, 0, 0, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 0, 220, 255), RGBA(255, 0, 220, 255), RGBA(255, 0, 220, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 0, 220, 255), RGBA(255, 0, 220, 255), RGBA(255, 0, 220, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(255, 106, 0, 255), RGBA(255, 106, 0, 255), RGBA(255, 0, 220, 255), RGBA(255, 0, 220, 255), RGBA(255, 0, 220, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255) } };
Image Img2 = { 10, 10, {RGBA(255, 150, 200, 255), RGBA(255, 163, 137, 255), RGBA(255, 163, 137, 255), RGBA(224, 255, 140, 255), RGBA(224, 255, 140, 255), RGBA(224, 255, 140, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(255, 150, 200, 255), RGBA(255, 163, 137, 255), RGBA(255, 163, 137, 255), RGBA(224, 255, 140, 255), RGBA(224, 255, 140, 255), RGBA(224, 255, 140, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(255, 255, 255, 255), RGBA(255, 163, 137, 255), RGBA(255, 163, 137, 255), RGBA(224, 255, 140, 255), RGBA(224, 255, 140, 255), RGBA(224, 255, 140, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(224, 255, 140, 255), RGBA(224, 255, 140, 255), RGBA(224, 255, 140, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(255, 255, 255, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(165, 255, 185, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(255, 127, 182, 255), RGBA(255, 127, 182, 255), RGBA(128, 128, 128, 255), RGBA(64, 64, 64, 255), RGBA(0, 0, 0, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(255, 127, 182, 255), RGBA(255, 127, 182, 255), RGBA(128, 128, 128, 255), RGBA(64, 64, 64, 255), RGBA(0, 0, 0, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(160, 160, 160, 255), RGBA(160, 160, 160, 255), RGBA(128, 128, 128, 255), RGBA(64, 64, 64, 255), RGBA(0, 0, 0, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(192, 192, 192, 255), RGBA(192, 192, 192, 255), RGBA(192, 192, 192, 255), RGBA(64, 64, 64, 255), RGBA(0, 0, 0, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(163, 250, 255, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(0, 0, 0, 255)} };
std::vector<TMPCommand> CommandList;

//Image& Img = Img5x5;
Image& Img = Img2;
int main(int argc, char** argv)
{

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

    delete[] processedPixels;
    std::cout << "Done!\n";
}

int GetOptimizationLevel(int Idx, int* ProcessedPixelsList) {
    int level = 0;
    if (ProcessedPixelsList[Idx] != 0) {
        return -1;
    }
    int newLevel = level + 1;
    while (Idx + (newLevel - 1 /*fix bottom right corner*/ )
        + Img.width * (newLevel - 1 /*fix bottom*/ ) < Img.width * Img.height) {
        
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

void MarkProcessedPixel(int Idx, int OptimizationLevel, int* ProcessedPixelsList) {
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

void GenerateTMP() {

    std::string SymbolChar = "@";
    double SymbolXFix = 0.45;
    double SymbolYFix = 0.45275;
    double SymbolWidth = 1.78;
    double SymbolHeight = 1.117;

    std::string header = { "<line-height=0><width=0><align=center>\n<mark=#ff005055><color=#0000>\n" };
    std::string out = header;
    
    int size = 1;
    int y = -1;
    for (auto& cmd : CommandList) {
        if (size != cmd.size) {
            out += "\n<size=" + std::to_string(pow(2, cmd.size-1)) + "e>";
        }

        if (y != cmd.y || size != cmd.size) {
            y = cmd.y;

            double voffset = (double)y / (double)cmd.size * SymbolHeight;

            double yFix = 0;
            if (cmd.size > 0) {
                double fixer = 1;
                for (int i = 0; i < cmd.size; i++) {
                    voffset += SymbolYFix * fixer;
                    fixer *= 0.5;
                }
            }
            
            out += "<voffset=-" + std::to_string(voffset) + "e>\n";
        }
        

        double pos = cmd.x / cmd.size * SymbolWidth;
        if (cmd.size > 0) {
            double fixer = 1;
            for (int i = 0; i < cmd.size; i++) {
                pos += SymbolXFix * fixer;
                fixer *= 0.5;
            }
        }
        out += "<pos=" + std::to_string(pos) + "e>";
        out += SymbolChar;

        size = cmd.size;

    }

    printf("\n");
    std::cout << out << std::endl;
}
