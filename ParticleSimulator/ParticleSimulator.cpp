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
    sand,
    water
}CellType;

class MainClass {
private:
    CellType grid[GRIDW][GRIDH];
    CellType selectedBrush;
    int brushSize;
public:
    MainClass() {
        InitWindow(SCREENW, SCREENH, "ParticleSim");
        SetTargetFPS(80);
        for (int i = 0; i < GRIDW; i++) {
            for (int j = 0; j < GRIDH; j++) {
                grid[i][j] = air;
            }
        }
        brushSize = 1;        
        _mainFunc();
    }
    void _mainFunc() {
        while (!WindowShouldClose())
        {
            _drawingFunction();
            _mouseInteraction();
            _simulate();
        }
    }
    void _drawingFunction() {
        BeginDrawing();
        ClearBackground(BLACK);
        for (int i = 0; i < GRIDW; i++) {
            for (int j = 0; j < GRIDH; j++) {
                switch (grid[i][j]) {
                case air:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, SKYBLUE);
                    break;
                case sand:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, SANDCOL);
                    break;
                case water:
                    DrawRectangle(i * CELLSIDE, j * CELLSIDE, CELLSIDE, CELLSIDE, DARKBLUE);
                    break;
                }
            }
        }
        DrawRectangle(10, 713, TYPESIDE, TYPESIDE, SKYBLUE);
        DrawRectangle(70, 713, TYPESIDE, TYPESIDE, SANDCOL);
        DrawRectangle(130, 713, TYPESIDE, TYPESIDE, DARKBLUE);
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
                    selectedBrush = sand;
                }
                else if (x > 130 && x < 130 + TYPESIDE) {
                    selectedBrush = water;
                }
                else if (brushSize < 20 && x > 930 && x < 960 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    brushSize++;
                }
                else if (brushSize > 1 && x > 970 && x < 1000 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    brushSize--;
                }
            }
            else {
                x /= CELLSIDE;
                y /= CELLSIDE;
                for (int i = 0; i < brushSize; i++) {
                    for (int j = 0; j < brushSize; j++) {
                        if (x - brushSize / 2 + i >= 0 && x - brushSize / 2 + i < GRIDW && y - brushSize / 2 + j >= 0 && y - brushSize / 2 + j < GRIDH) {
                            grid[x - brushSize / 2 + i][y - brushSize / 2 + j] = selectedBrush; 
                        }
                    }
                }
            }
        }
    }
    void _simulate() {
        int max, min, checked = 0;
        for (int i = 0; i < GRIDW; i++) {
            for (int j = GRIDH - 1; j >= 0; j--) {
                switch (grid[i][j]) {
                case air:
                    break;
                case sand:
                    if (j + 1 < GRIDH && grid[i][j + 1] != sand) {
                        grid[i][j] = grid[i][j + 1];
                        grid[i][j + 1] = sand;
                    }
                    break;
                case water:
                    if (j + 1 < GRIDH) {
                        if (grid[i][j + 1] != sand && grid[i][j + 1] != water) {
                            grid[i][j] = grid[i][j + 1];
                            grid[i][j + 1] = water;
                        }
                        else {
                            max = GRIDW;
                            min = -1;
                            for (int x = i + 1; x < GRIDW; x++) {
                                if (grid[x][j] == sand || grid[x][j] == water) {
                                    max = x + 1;
                                    break;
                                }
                            }
                            for (int x = i - 1; x >= 0; x--) {
                                if (grid[x][j] == sand || grid[x][j] == water) {
                                    min = x - 1;
                                    break;
                                }
                            }
                            if (!checked) {
                                for (int x = i; x < max ; x++) {
                                    if (grid[x][j] == air) {
                                        grid[i][j] = air;
                                        grid[x][j] = water;
                                        checked = 1;
                                        break;
                                    }
                                }
                            }
                            else {
                                checked = 0;
                            }
                            if (!checked) {
                                for (int x = i; x > min; x--) {
                                    if (grid[x][j] == air) {
                                        grid[i][j] = air;
                                        grid[x][j] = water;
                                        break;
                                    }
                                }
                            }
                            else {
                                checked = 0;
                            }
                        }
                    }                   
                    break;
                }
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

