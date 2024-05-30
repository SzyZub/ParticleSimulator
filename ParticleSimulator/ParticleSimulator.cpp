#include "raylib.h"
#define SCREENW 1024
#define SCREENH 768
#define CELLSIDE 4
#define TYPESIDE 50
#define GRIDW SCREENW/CELLSIDE
#define GRIDH SCREENH/CELLSIDE - 15
#define SANDCOL {211, 169, 108, 255}

typedef enum CellType {
    air = 0,
    sand
}CellType;

class MainClass {
private:
    CellType grid[GRIDW][GRIDH];
    CellType selectedBrush;
public:
    MainClass() {
        InitWindow(SCREENW, SCREENH, "ParticleSim");
        SetTargetFPS(60);
        for (int i = 0; i < GRIDW; i++) {
            for (int j = 0; j < GRIDH; j++) {
                grid[i][j] = air;
            }
        }
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
                }
            }
        }
        DrawRectangle(5, 713, TYPESIDE, TYPESIDE, SKYBLUE);
        DrawRectangle(65, 713, TYPESIDE, TYPESIDE, SANDCOL);
        EndDrawing();
    }
    void _mouseInteraction() {
        int x = GetMouseX();
        int y = GetMouseY();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (y >= 708) {
                if (x > 5 && x < 5 + TYPESIDE) {
                    selectedBrush = air;
                }
                else if (x > 65 && x < 65 + TYPESIDE) {
                    selectedBrush = sand;
                }
            }
            else {
                grid[x / CELLSIDE][y / CELLSIDE] = selectedBrush;
            }
        }
    }
    void _simulate() {
        for (int i = 0; i < GRIDW; i++) {
            for (int j = GRIDH; j >= 0; j--) {
                switch (grid[i][j]) {
                case air:
                    break;
                case sand:
                    if (j < GRIDH - 1 && grid[i][j + 1] == air) {
                        grid[i][j] = air;
                        grid[i][j + 1] = sand;
                    }
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

