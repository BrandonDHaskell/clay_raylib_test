#define CLAY_IMPLEMENTATION
#include "clay.h"

#include "raylib.h"
#include <vector>

int main() {
    const int initialWidth = 800;
    const int initialHeight = 600;

    InitWindow(initialWidth, initialHeight, "Clay + Raylib: Auto Resize");
    SetTargetFPS(60);

    // Allocate memory for Clay
    uint32_t clayMemSize = Clay_MinMemorySize();
    void* memory = malloc(clayMemSize);
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(clayMemSize, memory);

    // Set up error handler
    Clay_ErrorHandler errorHandler = {};
    errorHandler.errorHandlerFunction = [](Clay_ErrorData err) {
        TraceLog(LOG_ERROR, "Clay Error: %.*s", err.errorText.length, err.errorText.chars);
    };

    // Initialize Clay with initial dimensions
    Clay_Context* context = Clay_Initialize(arena, { (float)initialWidth, (float)initialHeight }, errorHandler);

    // Provide dummy text measure function
    Clay_SetMeasureTextFunction([](Clay_StringSlice, Clay_TextElementConfig*, void*) {
        return Clay_Dimensions{ 0, 0 };
    }, nullptr);

    while (!WindowShouldClose()) {
        // Detect current screen dimensions
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        Clay_Dimensions clayScreen = { (float)screenWidth, (float)screenHeight };

        BeginDrawing();
        ClearBackground(BLACK);

        // Update Clay pointer state and dimensions
        Clay_SetPointerState({ 0, 0 }, false);
        Clay_SetLayoutDimensions(clayScreen);

        // Start Clay layout pass
        Clay_BeginLayout();

        // Create a full-screen colored background that resizes with the window
        CLAY({
            .id = CLAY_ID("Background"),
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(clayScreen.width),
                    .height = CLAY_SIZING_FIXED(clayScreen.height)
                }
            },
            .backgroundColor = { 135, 206, 235, 255 } // Sky blue
        });

        // Finalize layout and get render commands
        Clay_RenderCommandArray commands = Clay_EndLayout();

        // Render Clay commands
        for (int i = 0; i < commands.length; i++) {
            Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&commands, i);

            if (cmd->commandType == CLAY_RENDER_COMMAND_TYPE_RECTANGLE) {
                auto& bb = cmd->boundingBox;
                auto& color = cmd->renderData.rectangle.backgroundColor;

                DrawRectangleRec(
                    { bb.x, bb.y, bb.width, bb.height },
                    { (unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b, (unsigned char)color.a }
                );
            }
        }

        EndDrawing();
    }

    // Clean up
    CloseWindow();
    free(memory);

    return 0;
}
