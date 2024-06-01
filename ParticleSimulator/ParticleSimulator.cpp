#include "raylib.h"
#include <iostream>

#define SCREENW 1024
#define SCREENH 768
#define CELLSIDE 4
#define TYPESIDE 50
#define GRIDW SCREENW/CELLSIDE
#define GRIDH SCREENH/CELLSIDE - 15
#define SANDCOL {211, 169, 108, 255}

typedef enum CellType {
    air = 0,
    gravel,
    water,
    sand,
    smoke
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
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, SKYBLUE);
                    break;
                case gravel:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, GRAY);
                    break;
                case water:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, DARKBLUE);
                    break;
                case sand:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, SANDCOL);
                    break;
                case smoke:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, LIGHTGRAY);
                    break;
                }
            }
        }
        DrawRectangle(10, 713, TYPESIDE, TYPESIDE, SKYBLUE);
        DrawRectangle(70, 713, TYPESIDE, TYPESIDE, GRAY);
        DrawRectangle(130, 713, TYPESIDE, TYPESIDE, DARKBLUE);
        DrawRectangle(190, 713, TYPESIDE, TYPESIDE, SANDCOL);
        DrawRectangle(250, 713, TYPESIDE, TYPESIDE, LIGHTGRAY);
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
        int checked;
        bool** moved = new bool*[GRIDW];
        for (int i = 0; i < GRIDW; i++) {
            moved[i] = new bool[GRIDH];
        }
        for (int i = GRIDW - 1; i >= 0; i--) {
            for (int j = GRIDH - 1; j >= 0; j--) {
                moved[i][j] = false;
            }
        }
        int left = 0, right = 0;
        for (int i = GRIDW - 1; i >= 0; i--) {
            for (int j = GRIDH - 1; j >= 0; j--) {
                if (moved[i][j] == true) {
                    continue;
                }
                switch (grid[i][j]) {
                case air:
                    break;
                case gravel:
                    checked = 0;
                    if (j + 1 < GRIDH && grid[i][j + 1] != gravel && grid[i][j + 1] != sand) {
                        if (grid[i][j + 1] == water) {
                            for (int x = 0; i + x < GRIDW; x++) {
                                if (grid[i + x][j] == air) {
                                    grid[i + x][j] = water;
                                    checked = 1;
                                    break;
                                }
                            }
                            if (!checked) {
                                for (int x = 0; i + x >= 0; x--) {
                                    if (grid[i + x][j] == air) {
                                        grid[i + x][j] = water;
                                        break;
                                    }
                                }
                            }
                            if (!checked) {
                                grid[i][j] = water;
                                grid[i][j + 1] = gravel;
                                break;
                            }
                        }
                        grid[i][j] = air;
                        grid[i][j + 1] = gravel;
                    }
                    break;
                case water:
                    if (j + 1 < GRIDH) {
                        if (grid[i][j + 1] == air) {
                            grid[i][j] = air;
                            grid[i][j + 1] = water;
                            break;
                        }
                        else {
                            for (int x = 1; left != 1 || right != 1; x++) {
                                if (i - x < 0 || (grid[i - x][j] != air && grid[i - x][j] != water)) {
                                    left = 1;
                                }
                                if (i + x > GRIDW - 1 || (grid[i + x][j] != air && grid[i + x][j] != water)) {
                                    right = 1;
                                }
                                if (left != 1 && grid[i - x][j] == air) {
                                    grid[i][j] = air;
                                    grid[i - x][j] = water;
                                    break;
                                }
                                else if (right != 1 && grid[i + x][j] == air) {
                                    grid[i][j] = air;
                                    grid[i + x][j] = water;
                                    break;
                                }
                            }
                        }
                    }
                    left = 0;
                    right = 0;
                    break;
                case sand:
                    checked = 0;
                    if (j + 1 < GRIDH && grid[i][j + 1] != gravel && grid[i][j + 1] != sand) {
                        if (grid[i][j + 1] == water) {
                            for (int x = 0; i + x < GRIDW; x++) {
                                if (grid[i + x][j] == air) {
                                    grid[i + x][j] = water;
                                    moved[i + x][j] = true;
                                    checked = 1;
                                    break;
                                }
                            }
                            if (!checked) {
                                for (int x = 0; i + x >= 0; x--) {
                                    if (grid[i + x][j] == air) {
                                        grid[i + x][j] = water;
                                        moved[i + x][j] = true;
                                        break;
                                    }
                                }
                            }
                            if (!checked) {
                                grid[i][j] = water;
                                grid[i][j + 1] = sand;
                                break;
                            }
                        }
                        grid[i][j] = air;
                        grid[i][j + 1] = sand;
                        break;
                    }
                    else if (i - 1 >= 0 && j + 1 < GRIDH && (grid[i - 1][j + 1] == air || grid[i - 1][j + 1] == water)) {
                        grid[i][j] = grid[i - 1][j + 1];
                        grid[i - 1][j + 1] = sand;
                    }
                    else if (i + 1 < GRIDW && j + 1 < GRIDH && (grid[i + 1][j + 1] == air || grid[i + 1][j + 1] == water)) {
                        grid[i][j] = grid[i + 1][j + 1];
                        grid[i + 1][j + 1] = sand;
                    }
                    break;
                case smoke:
                    if (rand() % 480 == 0) {
                        grid[i][j] = air;
                        break;
                    }
                    if (j - 1 >= 0) {
                        if (grid[i][j - 1] == air || grid[i][j - 1] == water) {
                            grid[i][j] = grid[i][j - 1];
                            moved[i][j - 1] = true;
                            grid[i][j - 1] = smoke;
                            break;
                        }
                        else {
                            switch (rand() % 3) {
                            case 0:
                                break;
                            case 1:
                                if (i + 1 < GRIDW && grid[i + 1][j] == air) {
                                    grid[i][j] = air;
                                    moved[i + 1][j] = true;
                                    grid[i + 1][j] = smoke;
                                }
                                break;
                            case 2:
                                if (i - 1 >= 0 && grid[i - 1][j] == air) {
                                    grid[i][j] = air;
                                    moved[i - 1][j] = true;
                                    grid[i - 1][j] = smoke;
                                }
                                break;
                            }
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
        SetTargetFPS(80);
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

