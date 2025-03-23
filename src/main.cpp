#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "raylib.h"

#include <vector>
#include <cstring>

// ----[ Views (aka States) ]----
enum ActiveView {
    PRODUCT_VIEW,
    SUPPLIER_VIEW,
    DEPARTMENT_VIEW,
    INVENTORY_VIEW
};

struct ViewState {
    const char* label;
    Clay_ElementId tabId;
    Clay_ElementId contentId;
    Clay_Color tabColorActive;
    Clay_Color tabColorInactive;
    Clay_Color contentColor;
};

static Clay_ElementId PRODUCT_TAB_ID    = CLAY_ID("ProductTab");
static Clay_ElementId SUPPLIER_TAB_ID   = CLAY_ID("SupplierTab");
static Clay_ElementId DEPARTMENT_TAB_ID = CLAY_ID("DepartmentTab");
static Clay_ElementId INVENTORY_TAB_ID  = CLAY_ID("InventoryTab");

static Clay_ElementId PRODUCT_BOX_ID    = CLAY_ID("ProductBox");
static Clay_ElementId SUPPLIER_BOX_ID   = CLAY_ID("SupplierBox");
static Clay_ElementId DEPARTMENT_BOX_ID = CLAY_ID("DepartmentBox");
static Clay_ElementId INVENTORY_BOX_ID  = CLAY_ID("InventoryBox");

ViewState viewStates[] = {
    { "Product View",    PRODUCT_TAB_ID,    PRODUCT_BOX_ID,    {120,120,255,255}, {100,100,200,255}, {200,255,200,255} },
    { "Supplier View",   SUPPLIER_TAB_ID,   SUPPLIER_BOX_ID,   {120,120,255,255}, {100,100,200,255}, {200,200,255,255} },
    { "Department View", DEPARTMENT_TAB_ID, DEPARTMENT_BOX_ID, {120,120,255,255}, {100,100,200,255}, {255,255,200,255} },
    { "Inventory View",  INVENTORY_TAB_ID,  INVENTORY_BOX_ID,  {120,120,255,255}, {100,100,200,255}, {255,200,200,255} }
};

ActiveView currentView = PRODUCT_VIEW;

// ----[ Utility ]----
void DrawCenteredText(const char* text, Rectangle rect, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    int x = (int)(rect.x + (rect.width - textWidth) / 2);
    int y = (int)(rect.y + (rect.height - fontSize) / 2);
    DrawText(text, x, y, fontSize, color);
}

int main() {
    const int initialWidth = 800;
    const int initialHeight = 600;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(initialWidth, initialHeight, "Clay + Raylib: State Machine UI");
    SetTargetFPS(60);

    uint32_t clayMemSize = Clay_MinMemorySize();
    void* memory = malloc(clayMemSize);
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(clayMemSize, memory);

    Clay_ErrorHandler errorHandler = {};
    Clay_Context* ctx = Clay_Initialize(arena, { (float)initialWidth, (float)initialHeight }, errorHandler);

    Clay_SetMeasureTextFunction([](Clay_StringSlice, Clay_TextElementConfig*, void*) {
        return Clay_Dimensions{ 0, 0 };
    }, nullptr);

    while (!WindowShouldClose()) {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        Clay_Dimensions clayScreen = { (float)screenWidth, (float)screenHeight };

        Clay_SetPointerState({ (float)GetMouseX(), (float)GetMouseY() }, IsMouseButtonDown(MOUSE_LEFT_BUTTON));
        Clay_SetLayoutDimensions(clayScreen);

        Clay_BeginLayout();

        CLAY({
            .id = CLAY_ID("RootLayout"),
            .layout = {
                .sizing = { CLAY_SIZING_PERCENT(1.0f), CLAY_SIZING_PERCENT(1.0f) },
                .childGap = 10,
                .layoutDirection = CLAY_TOP_TO_BOTTOM
            },
            .backgroundColor = {240, 240, 240, 255}
        }) {
            // Navigation Bar
            CLAY({
                .id = CLAY_ID("NavBar"),
                .layout = {
                    .sizing = { CLAY_SIZING_PERCENT(1.0f), CLAY_SIZING_FIXED(50) },
                    .padding = {10,10,5,5},
                    .childGap = 10,
                    .layoutDirection = CLAY_LEFT_TO_RIGHT
                },
                .backgroundColor = {30,30,60,255}
            }) {
                const int viewCount = sizeof(viewStates) / sizeof(viewStates[0]);
                for (int i = 0; i < viewCount; ++i) {
                    CLAY({
                        .id = viewStates[i].tabId,
                        .layout = {
                            .sizing = { CLAY_SIZING_FIXED(120), CLAY_SIZING_FIXED(30) }
                        },
                        .backgroundColor = (currentView == i)
                            ? viewStates[i].tabColorActive
                            : viewStates[i].tabColorInactive
                    });

                    if (Clay_PointerOver(viewStates[i].tabId) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                        currentView = static_cast<ActiveView>(i);
                    }
                }
            }

            // Content Area
            // Header box below NavBar
            CLAY({
                .id = CLAY_ID("HeaderBox"),
                .layout = {
                    .sizing = { CLAY_SIZING_PERCENT(1.0f), CLAY_SIZING_FIXED(40) },
                    .padding = { 5, 10, 5, 10 }
                },
                .backgroundColor = { 220, 220, 240, 255 }  // Light header background
            });
            CLAY({
                .id = CLAY_ID("ContentView"),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_PERCENT(1.0f),
                        .height = CLAY_SIZING_GROW(0, clayScreen.height - 60)
                    },
                    .padding = {20, 20, 20, 20}
                },
                .backgroundColor = {255,255,255,255}
            }) {
                CLAY({
                    .id = viewStates[currentView].contentId,
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_PERCENT(1.0f),
                            .height = CLAY_SIZING_PERCENT(1.0f)
                        },
                        .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}
                    },
                    .backgroundColor = viewStates[currentView].contentColor
                });
            }
        }

        Clay_RenderCommandArray commands = Clay_EndLayout();

        // --- Rendering begins ---
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw rectangles
        for (int i = 0; i < commands.length; ++i) {
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

        // Draw tab labels
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < commands.length; ++j) {
                Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&commands, j);
                if (cmd->commandType == CLAY_RENDER_COMMAND_TYPE_RECTANGLE &&
                    cmd->id == viewStates[i].tabId.id) {
                    DrawCenteredText(viewStates[i].label, {
                        cmd->boundingBox.x, cmd->boundingBox.y,
                        cmd->boundingBox.width, cmd->boundingBox.height
                    }, 14, WHITE);
                    break;
                }
            }
        }

        // Draw Header Text inside HeaderBox
        for (int i = 0; i < commands.length; ++i) {
            Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&commands, i);
            if (cmd->commandType == CLAY_RENDER_COMMAND_TYPE_RECTANGLE &&
                cmd->id == CLAY_ID("HeaderBox").id) {
                DrawCenteredText(viewStates[currentView].label, {
                    cmd->boundingBox.x, cmd->boundingBox.y,
                    cmd->boundingBox.width, cmd->boundingBox.height
                }, 20, DARKBLUE);
                break;
            }
        }

        // Draw centered content label
        for (int i = 0; i < commands.length; ++i) {
            Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&commands, i);
            if (cmd->commandType == CLAY_RENDER_COMMAND_TYPE_RECTANGLE &&
                cmd->id == viewStates[currentView].contentId.id) {
                DrawCenteredText(viewStates[currentView].label, {
                    cmd->boundingBox.x, cmd->boundingBox.y,
                    cmd->boundingBox.width, cmd->boundingBox.height
                }, 24, DARKGRAY);
                break;
            }
        }

        EndDrawing();
    }

    // Clean up
    CloseWindow();
    free(memory);
    return 0;
}
