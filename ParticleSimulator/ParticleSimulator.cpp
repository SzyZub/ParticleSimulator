#include "raylib.h"
#include <iostream>

#define SCREENW 1024
#define SCREENH 768
#define CELLSIDE 4
#define TYPESIDE 50
#define GRIDW SCREENW/CELLSIDE
#define GRIDH SCREENH/CELLSIDE - 15
#define SKYCOL {102, 191, 255, 210}
#define GRAVCOL {130, 130, 130, 150}
#define WATERCOL {20, 82, 200, 150}
#define SANDCOL {211, 169, 108, 215}
#define SMOKCOL {200, 200, 200, 245}
#define ACIDCOL {0, 208, 65, 200}

typedef enum CellType {
    air = 0,
    gravel,
    water,
    sand,
    smoke,
    acid
}CellType;

class ParticleHandler {
private:
    CellType grid[GRIDW][GRIDH];
public:
    ParticleHandler() {
        for (int i = 0; i < GRIDW; i++) {
            for (int j = 0; j < GRIDH; j++) {
                grid[i][j] = air;
            }
        }
    }
    void _DrawParticles() {
        for (int i = 0; i < GRIDW; i++) {
            for (int j = 0; j < GRIDH; j++) {
                switch (grid[i][j]) {
                case air:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, SKYCOL);
                    break;
                case gravel:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, GRAVCOL);
                    break;
                case water:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, WATERCOL);
                    break;
                case sand:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, SANDCOL);
                    break;
                case smoke:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, SMOKCOL);
                    break;
                case acid:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, ACIDCOL);
                    break;
                }
            }
        }
        DrawRectangle(10, 713, TYPESIDE, TYPESIDE, SKYCOL);
        DrawRectangle(70, 713, TYPESIDE, TYPESIDE, GRAVCOL);
        DrawRectangle(130, 713, TYPESIDE, TYPESIDE, WATERCOL);
        DrawRectangle(190, 713, TYPESIDE, TYPESIDE, SANDCOL);
        DrawRectangle(250, 713, TYPESIDE, TYPESIDE, SMOKCOL);
        DrawRectangle(310, 713, TYPESIDE, TYPESIDE, ACIDCOL);
    }
    void _PutParticles(int x, int y, int brushSize, CellType selectedBrush) {
        int xn = x / CELLSIDE;
        int yn = y / CELLSIDE;
        for (int i = 0; i < brushSize; i++) {
            for (int j = 0; j < brushSize; j++) {
                if (xn - brushSize / 2 + i >= 0 && xn - brushSize / 2 + i < GRIDW && yn - brushSize / 2 + j >= 0 && yn - brushSize / 2 + j < GRIDH) {
                    grid[xn - brushSize / 2 + i][yn - brushSize / 2 + j] = selectedBrush;
                }
            }
        }
    }
    void _simulate() {
        _simulateHorizontal();
        _simulateVertical();
    }
    void _simulateVertical() {
        int smokeCount = 0, sumSmoke = 0;
        bool** moved = new bool* [GRIDW];
        for (int i = 0; i < GRIDW; i++) {
            moved[i] = new bool[GRIDH];
        }
        for (int i = GRIDW - 1; i >= 0; i--) {
            for (int j = GRIDH - 1; j >= 0; j--) {
                moved[i][j] = false;
            }
        }
        
        for (int j = GRIDH - 1; j >= 0; j--) {
            sumSmoke += smokeCount;
            smokeCount = 0;
            for (int i = 0; i < GRIDW; i++) {
                if (moved[i][j] == true) {
                    continue;
                }
                switch (grid[i][j]) {
                case air:
                    break;
                case gravel:
                    if (j + 1 < GRIDH && grid[i][j + 1] == acid) {
                        grid[i][j] = smoke;
                        grid[i][j + 1] = smoke;
                    } else if (j + 1 < GRIDH && (grid[i][j + 1] == air || grid[i][j + 1] == water)) {
                        grid[i][j] = grid[i][j + 1];
                        grid[i][j + 1] = gravel;
                    }
                    break;
                case water:
                    if (j + 1 < GRIDH && grid[i][j + 1] == acid) {
                        grid[i][j] = smoke;
                        grid[i][j + 1] = smoke;
                    } else if (j + 1 < GRIDH && grid[i][j + 1] == air) {
                        grid[i][j] = air;
                        grid[i][j + 1] = water;
                    }
                    break;
                case sand:
                    if (j + 1 < GRIDH && grid[i][j + 1] == acid) {
                        grid[i][j] = smoke;
                        grid[i][j + 1] = smoke;
                    } else if (j + 1 < GRIDH && (grid[i][j + 1] == air || grid[i][j + 1] == water)) {
                        grid[i][j] = grid[i][j + 1];
                        grid[i][j + 1] = sand;
                    }
                    break;
                case smoke:
                    if (sumSmoke < 1400) {
                        smokeCount++;
                    };
                    if (rand() % (95 - sumSmoke / 20) == 0) {
                        grid[i][j] = air;
                    } else {
                        int x;
                        for (x = j; x > 0; x--) {
                            if (grid[i][x] == smoke) {
                                moved[i][x] = true;
                            }
                            else if(grid[i][x] == sand || grid[i][x] == gravel){
                                x++;
                                break;
                            }
                            else {
                                break;
                            }
                        }
                        grid[i][j] = grid[i][x];
                        grid[i][x] = smoke;
                        moved[i][x] = true;
                    }
                    break;
                case acid:
                    if (j + 1 < GRIDH && grid[i][j + 1] == acid) {
                        break;
                    } else if (j + 1 < GRIDH && grid[i][j + 1] != air && grid[i][j + 1] != smoke) {
                        grid[i][j] = smoke;
                        grid[i][j + 1] = smoke;
                    }
                    else if (j + 1 < GRIDH) {
                        grid[i][j] = air;
                        grid[i][j + 1] = acid;
                    }
                    break;
                }
            }
        }
        for (int i = 0; i < GRIDW; ++i) {
            delete[] moved[i];
        }
        delete[] moved;
    }
    void _simulateHorizontal() {
        bool** moved = new bool* [GRIDW];
        for (int i = 0; i < GRIDW; i++) {
            moved[i] = new bool[GRIDH];
        }
        for (int i = GRIDW - 1; i >= 0; i--) {
            for (int j = GRIDH - 1; j >= 0; j--) {
                moved[i][j] = false;
            }
        }
        for (int i = 0; i < GRIDW; i++) {
            for (int j = GRIDH - 1; j >= 0; j--) {
                if (moved[i][j] == true) {
                    continue;
                }
                switch (grid[i][j]) {
                case air:
                    break;
                case gravel:
                    break;
                case water:
                    if (rand() % 2) {
                        if (i - 1 >= 0 && grid[i - 1][j] == air) {
                            grid[i - 1][j] = water;
                            grid[i][j] = air;
                        }
                    }
                    else {
                        if (i + 1 < GRIDW && grid[i + 1][j] == air) {
                            grid[i + 1][j] = water;
                            grid[i][j] = air;
                        }
                    }
                    break;
                case sand:
                    if (rand() % 2) {
                        if (i - 1 >= 0 && j + 1 < GRIDH && (grid[i - 1][j + 1] == air || grid[i - 1][j + 1] == water) && grid[i - 1][j] != sand && grid[i - 1][j] != gravel && grid[i][j + 1] != air) {
                            grid[i][j] = grid[i - 1][j];
                            moved[i][j] = true;
                            grid[i - 1][j] = sand;
                            moved[i - 1][j] = true;
                        }
                    }
                    else if (i + 1 < GRIDW && j + 1 < GRIDH && (grid[i + 1][j + 1] == air || grid[i + 1][j + 1] == water) && grid[i + 1][j] != sand && grid[i + 1][j] != gravel && grid[i][j + 1] != air) {
                        grid[i][j] = grid[i + 1][j];
                        moved[i][j] = true;
                        grid[i + 1][j] = sand;
                        moved[i + 1][j] = true;
                    }
                    break;
                case smoke:
                    if (rand() % 2) {
                        if (i - 1 >= 0 && grid[i - 1][j] == air) {
                            grid[i - 1][j] = smoke;
                            grid[i][j] = air;
                        }
                    }
                    else {
                        if (i + 1 < GRIDW && grid[i + 1][j] == air) {
                            grid[i + 1][j] = smoke;
                            grid[i][j] = air;
                        }
                    }
                    break;
                case acid:
                    if (rand() % 2) {
                        if (j + 1 < GRIDH && i - 1 >= 0 && grid[i][j+1] == acid && grid[i-1][j] != air && grid[i - 1][j] != acid) {
                            grid[i - 1][j] = smoke;
                            grid[i][j] = smoke;
                        }
                        else if (i - 1 >= 0 && grid[i - 1][j] == air){
                            grid[i][j] = grid[i - 1][j];
                            grid[i - 1][j] = acid;
                        }
                    }
                    else {
                        if (j + 1 < GRIDH && i + 1 < GRIDW && grid[i][j + 1] == acid && grid[i + 1][j] != air && grid[i + 1][j] != acid) {
                            grid[i + 1][j] = smoke;
                            grid[i][j] = smoke;
                        }
                        else if (i + 1 < GRIDW && grid[i + 1][j] == air){
                            grid[i][j] = grid[i + 1][j];
                            grid[i + 1][j] = acid;
                        }
                    }
                    break;
                }
            }
        }
        for (int i = 0; i < GRIDW; ++i) {
            delete[] moved[i];
        }
        delete[] moved;
    }
};

class MainClass {
private:
    ParticleHandler handler;
    CellType selectedBrush;
    int brushSize;
public:
    MainClass() {
        InitWindow(SCREENW, SCREENH, "ParticleSim");
        SetTargetFPS(60);
        handler = ParticleHandler();
        brushSize = 1;      
        srand(time(0));
        _mainFunc();
    }
    void _mainFunc() {
        while (!WindowShouldClose())
        {
            _drawingFunction();
            handler._simulate();
            _mouseInteraction();
        }
    }
    void _drawingFunction() {
        BeginDrawing();
        ClearBackground(BLACK);
        handler._DrawParticles();
        DrawText(TextFormat("%d", brushSize), 890, 725, 30, WHITE);
        DrawRectangle(930, 733, 30, 10, WHITE);
        DrawRectangle(940, 723, 10, 30, WHITE);
        DrawRectangle(970, 733, 30, 10, WHITE);
        EndDrawing();
    }
    void _mouseInteraction() {
        int x = GetMouseX();
        int y = GetMouseY();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (y >= 708) {
                if (x > 10 && x < 10 + TYPESIDE) {
                    selectedBrush = air;
                }
                else if (x > 70 && x < 70 + TYPESIDE) {
                    selectedBrush = gravel;
                }
                else if (x > 130 && x < 130 + TYPESIDE) {
                    selectedBrush = water;
                }
                else if (x > 190 && x < 190 + TYPESIDE) {
                    selectedBrush = sand;
                }
                else if (x > 250 && x < 250 + TYPESIDE) {
                    selectedBrush = smoke;
                }
                else if (x > 310 && x < 310 + TYPESIDE) {
                    selectedBrush = acid;
                }
                else if (brushSize < 20 && x > 930 && x < 960 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    brushSize++;
                }
                else if (brushSize > 1 && x > 970 && x < 1000 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    brushSize--;
                }
            }
            else {
                handler._PutParticles(x, y, brushSize, selectedBrush);
            }
        }
    }
    ~MainClass(){
        CloseWindow();
    }
};

int main(void)
{
    MainClass* Prog = new MainClass();
    return 0;
}

