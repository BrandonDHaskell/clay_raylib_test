#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "raylib.h"
#include "themes.h"

#include <vector>
#include <cstring>
#include <unordered_map>

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
    Clay_Color tabActiveColor;
    Clay_Color tabInactiveColor;
    Clay_Color tabBackgroundColor;
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
    { "Products",        PRODUCT_TAB_ID,    PRODUCT_BOX_ID,
        CurrentTheme->tabActiveColor,           // Active Tab
        CurrentTheme->tabInactiveColor,         // Inactive Tab
        CurrentTheme->contentBackgroundColor    // Content BG
    },
    { "Suppliers",       SUPPLIER_TAB_ID,   SUPPLIER_BOX_ID,
        CurrentTheme->tabActiveColor,           // Active Tab
        CurrentTheme->tabInactiveColor,         // Inactive Tab
        CurrentTheme->contentBackgroundColor    // Content BG
    },
    { "Departments",     DEPARTMENT_TAB_ID, DEPARTMENT_BOX_ID,
        CurrentTheme->tabActiveColor,           // Active Tab
        CurrentTheme->tabInactiveColor,         // Inactive Tab
        CurrentTheme->contentBackgroundColor    // Content BG
    },
    { "Inventory",       INVENTORY_TAB_ID,  INVENTORY_BOX_ID,
        CurrentTheme->tabActiveColor,           // Active Tab
        CurrentTheme->tabInactiveColor,         // Inactive Tab
        CurrentTheme->contentBackgroundColor    // Content BG
    }
};

ActiveView currentView = PRODUCT_VIEW;

// ----[ Drawing Utilities ]----

void DrawCenteredText(const char* text, Rectangle rect, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    int x = (int)(rect.x + (rect.width - textWidth) / 2);
    int y = (int)(rect.y + (rect.height - fontSize) / 2);
    DrawText(text, x, y, fontSize, color);
}

void DrawRectangleById(Clay_RenderCommandArray& commands, Clay_ElementId id) {
    for (int i = 0; i < commands.length; ++i) {
        Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&commands, i);
        if (cmd->commandType == CLAY_RENDER_COMMAND_TYPE_RECTANGLE && cmd->id == id.id) {
            const auto& bb = cmd->boundingBox;
            const auto& color = cmd->renderData.rectangle.backgroundColor;
            DrawRectangleRec(
                { bb.x, bb.y, bb.width, bb.height },
                { (unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b, (unsigned char)color.a }
            );
            break;
        }
    }
}

void DrawCenteredTextInElement(Clay_RenderCommandArray& commands, Clay_ElementId id, const char* text, int fontSize, Color color) {
    for (int i = 0; i < commands.length; ++i) {
        Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&commands, i);
        if (cmd->commandType == CLAY_RENDER_COMMAND_TYPE_RECTANGLE && cmd->id == id.id) {
            DrawCenteredText(text, {
                cmd->boundingBox.x, cmd->boundingBox.y,
                cmd->boundingBox.width, cmd->boundingBox.height
            }, fontSize, color);
            break;
        }
    }
}

void DrawLeftAlignedTextInElement(Clay_RenderCommandArray& commands, Clay_ElementId id, const char* text, int fontSize, Color color, float leftPadding = 10) {
    for (int i = 0; i < commands.length; ++i) {
        Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&commands, i);
        if (cmd->commandType == CLAY_RENDER_COMMAND_TYPE_RECTANGLE && cmd->id == id.id) {
            float x = cmd->boundingBox.x + leftPadding;
            float y = cmd->boundingBox.y + (cmd->boundingBox.height - fontSize) / 2;
            DrawText(text, (int)x, (int)y, fontSize, color);
            break;
        }
    }
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

    CurrentTheme = &LightTheme;
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
                // .childGap = 10,
                .layoutDirection = CLAY_TOP_TO_BOTTOM
            },
            .backgroundColor = CurrentTheme->contentBackgroundColor
        }) {
            // Begin App content

            // Navigation Bar
            CLAY({
                .id = CLAY_ID("NavBar"),
                .layout = {
                    .sizing = { CLAY_SIZING_PERCENT(1.0f), CLAY_SIZING_FIXED(50) },
                    .padding = {10,10,10,10},
                    .childGap = 10,
                    .childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER },
                    .layoutDirection = CLAY_LEFT_TO_RIGHT
                },
                .backgroundColor = CurrentTheme->navBarColor
            }) {
                const int viewCount = sizeof(viewStates) / sizeof(viewStates[0]);
                for (int i = 0; i < viewCount; ++i) {
                    CLAY({
                        .id = viewStates[i].tabId,
                        .layout = {
                            .sizing = { CLAY_SIZING_FIXED(120), CLAY_SIZING_FIXED(30) }
                        },
                        .backgroundColor = (currentView == i)
                            ? viewStates[i].tabActiveColor
                            : viewStates[i].tabInactiveColor
                    });

                    if (Clay_PointerOver(viewStates[i].tabId) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                        currentView = static_cast<ActiveView>(i);
                    }
                }
                CLAY({
                    .id = CLAY_ID("ToggleThemeButton"),
                    .layout = {
                        .sizing = { CLAY_SIZING_FIXED(120), CLAY_SIZING_FIXED(30) }
                    },
                    .backgroundColor = { 200, 200, 200, 255 }  // distinguishable color
                });

                if (Clay_PointerOver(CLAY_ID("ToggleThemeButton")) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    CurrentTheme = (CurrentTheme == &LightTheme) ? &DarkTheme : &LightTheme;
                }
            }

            // Header box below NavBar
            CLAY({
                .id = CLAY_ID("HeaderBox"),
                .layout = {
                    .sizing = { CLAY_SIZING_PERCENT(1.0f), CLAY_SIZING_FIXED(40) },
                    .padding = { 5, 10, 5, 10 }
                },
                .backgroundColor = CurrentTheme->headerBackgroundColor
            });
            
            // Content Area
            CLAY({
                .id = CLAY_ID("ContentView"),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_PERCENT(1.0f),
                        .height = CLAY_SIZING_GROW(0, clayScreen.height - 60),
                    },
                    // .padding = {20, 20, 20, 20}
                },
                .backgroundColor = CurrentTheme->contentBackgroundColor
            }) {
                CLAY({
                    .id = viewStates[currentView].contentId,
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_PERCENT(1.0f),
                            .height = CLAY_SIZING_PERCENT(1.0f)
                        },
                        .padding = {5, 5, 5, 5},
                        .childGap = 5,
                        .layoutDirection = CLAY_TOP_TO_BOTTOM
                    },
                    .backgroundColor = CurrentTheme->contentBackgroundColor
                }) {
                    // Top 1/3: two side-by-side boxes
                    CLAY({
                        .id = CLAY_ID("TopRow"),
                        .layout = {
                            .sizing = {
                                .width = CLAY_SIZING_PERCENT(1.0f),
                                .height = CLAY_SIZING_PERCENT(1.0f / 3.0f)  // 1/3 height
                            },
                            .childGap = 5,
                            .layoutDirection = CLAY_LEFT_TO_RIGHT
                        }
                    }) {
                        CLAY({
                            .id = CLAY_ID("TopLeftBox"),
                            .layout = {
                                .sizing = {
                                    .width = CLAY_SIZING_PERCENT(0.5f),
                                    .height = CLAY_SIZING_PERCENT(1.0f)
                                }
                            },
                            .backgroundColor = CurrentTheme->recordDetailsBoxColor
                        });
                
                        CLAY({
                            .id = CLAY_ID("TopRightBox"),
                            .layout = {
                                .sizing = {
                                    .width = CLAY_SIZING_PERCENT(0.5f),
                                    .height = CLAY_SIZING_PERCENT(1.0f)
                                }
                            },
                            .backgroundColor = CurrentTheme->recordCrudBoxColor
                        });
                    }
                
                    // Bottom 2/3: future table area
                    CLAY({
                        .id = CLAY_ID("BottomTablePlaceholder"),
                        .layout = {
                            .sizing = {
                                .width = CLAY_SIZING_PERCENT(1.0f),
                                .height = CLAY_SIZING_PERCENT(2.0f / 3.0f)
                            }
                        },
                        .backgroundColor = CurrentTheme->recordsDisplayBoxColor
                    });
                }
                
            }
        }

        Clay_RenderCommandArray commands = Clay_EndLayout();

        // --- Rendering begins ---
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw all rectangles
        for (int i = 0; i < commands.length; ++i) {
            Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&commands, i);
            if (cmd->commandType == CLAY_RENDER_COMMAND_TYPE_RECTANGLE) {
                const auto& bb = cmd->boundingBox;
                const auto& color = cmd->renderData.rectangle.backgroundColor;
                DrawRectangleRec(
                    { bb.x, bb.y, bb.width, bb.height },
                    { (unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b, (unsigned char)color.a }
                );
            }
        }

        // Draw tab labels
        int viewCount = sizeof(viewStates) / sizeof(viewStates[0]);
        for (int i = 0; i < viewCount; ++i) {
            DrawCenteredTextInElement(commands, viewStates[i].tabId, viewStates[i].label, 14, WHITE);
        }

        // Draw header text
        DrawLeftAlignedTextInElement(commands, CLAY_ID("HeaderBox"), viewStates[currentView].label, 20, DARKBLUE);

        // Draw centered label in content area
        DrawCenteredTextInElement(commands, viewStates[currentView].contentId, viewStates[currentView].label, 24, DARKGRAY);

        EndDrawing();
    }

    // Clean up
    CloseWindow();
    free(memory);
    return 0;
}
