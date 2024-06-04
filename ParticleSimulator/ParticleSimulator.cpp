#include "raylib.h"
#include <iostream>

#define SCREENW 1024
#define SCREENH 768
#define CELLSIDE 4
#define TYPESHORT 50
#define TYPELONG 100
#define GRIDW SCREENW/CELLSIDE
#define GRIDH SCREENH/CELLSIDE - 15
#define SKYCOL {102, 191, 255, 210}
#define GRAVCOL {130, 130, 130, 150}
#define WATERCOL {20, 82, 200, 150}
#define SANDCOL {211, 169, 108, 215}
#define SMOKCOL {200, 200, 200, 245}
#define ACIDCOL {0, 208, 65, 200}
#define ROCKCOL {40, 40, 40, 230}

typedef enum CellType {
    air = 0,
    gravel,
    water,
    sand,
    smoke,
    acid,
    rock
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
                case rock:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, ROCKCOL);
                    break;
                }
            }
        }
        DrawRectangle(30, 713, TYPELONG, TYPESHORT, SKYCOL);
        DrawRectangle(140, 713, TYPELONG, TYPESHORT, GRAVCOL);
        DrawRectangle(250, 713, TYPELONG, TYPESHORT, WATERCOL);
        DrawRectangle(360, 713, TYPELONG, TYPESHORT, SANDCOL);
        DrawRectangle(470, 713, TYPELONG, TYPESHORT, SMOKCOL);
        DrawRectangle(580, 713, TYPELONG, TYPESHORT, ACIDCOL);
        DrawRectangle(690, 713, TYPELONG, TYPESHORT, ROCKCOL);
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
                            else if(grid[i][x] == rock){
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
                    if (j + 1 < GRIDH && (grid[i][j + 1] == acid || grid[i][j + 1] == rock)) {
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
                        if (i - 1 >= 0 && j + 1 < GRIDH && (grid[i - 1][j + 1] == air || grid[i - 1][j + 1] == water) && grid[i - 1][j] != sand && grid[i - 1][j] != gravel && grid[i - 1][j] != rock && grid[i][j + 1] != air) {
                            grid[i][j] = grid[i - 1][j];
                            moved[i][j] = true;
                            grid[i - 1][j] = sand;
                            moved[i - 1][j] = true;
                        }
                    }
                    else if (i + 1 < GRIDW && j + 1 < GRIDH && (grid[i + 1][j + 1] == air || grid[i + 1][j + 1] == water) && grid[i + 1][j] != sand && grid[i + 1][j] != gravel && grid[i + 1][j] != rock && grid[i][j + 1] != air) {
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
                        if (j + 1 < GRIDH && i - 1 >= 0 && grid[i][j+1] == acid && grid[i-1][j] != air && grid[i - 1][j] != acid && grid[i - 1][j] != rock) {
                            grid[i - 1][j] = smoke;
                            grid[i][j] = smoke;
                        }
                        else if (i - 1 >= 0 && grid[i - 1][j] == air){
                            grid[i][j] = grid[i - 1][j];
                            grid[i - 1][j] = acid;
                        }
                    }
                    else {
                        if (j + 1 < GRIDH && i + 1 < GRIDW && grid[i][j + 1] == acid && grid[i + 1][j] != air && grid[i + 1][j] != acid && grid[i + 1][j] != rock) {
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
        DrawText(TextFormat("%d", brushSize), 835, 720, 40, WHITE);
        DrawRectangle(900, 733, 30, 10, WHITE);
        DrawRectangle(910, 723, 10, 30, WHITE);
        DrawRectangle(960, 733, 30, 10, WHITE);
        EndDrawing();
    }
    void _mouseInteraction() {
        int x = GetMouseX();
        int y = GetMouseY();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (y >= 708) {
                if (x > 30 && x < 30 + TYPELONG) {
                    selectedBrush = air;
                }
                else if (x > 140 && x < 140 + TYPELONG) {
                    selectedBrush = gravel;
                }
                else if (x > 250 && x < 250 + TYPELONG) {
                    selectedBrush = water;
                }
                else if (x > 360 && x < 360 + TYPELONG) {
                    selectedBrush = sand;
                }
                else if (x > 470 && x < 470 + TYPELONG) {
                    selectedBrush = smoke;
                }
                else if (x > 580 && x < 580 + TYPELONG) {
                    selectedBrush = acid;
                }
                else if (x > 690 && x < 690 + TYPELONG) {
                    selectedBrush = rock;
                }
                else if (brushSize < 20 && x > 900 && x < 930 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    brushSize++;
                }
                else if (brushSize > 1 && x > 960 && x < 990 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
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

