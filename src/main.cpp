#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "raylib.h"

#include <vector>
#include <cstring>

enum ActiveView {
    PRODUCT_VIEW,
    SUPPLIER_VIEW,
    DEPARTMENT_VIEW,
    INVENTORY_VIEW
};

ActiveView currentView = PRODUCT_VIEW;

bool ClayIdEquals(Clay_StringSlice a, const char* str) {
    size_t len = strlen(str);
    if (a.length != len) return false;
    return strncmp(a.chars, str, len) == 0;
};

void DrawTabLabel(Clay_ElementId* id, const char* label, Clay_RenderCommandArray& commands) {
    for (int i = 0; i < commands.length; ++i) {
        Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&commands, i);
        if (cmd->commandType == CLAY_RENDER_COMMAND_TYPE_RECTANGLE && cmd->id == id->id) {
            const auto& bb = cmd->boundingBox;

            // Center the text within the tab rectangle
            int textWidth = MeasureText(label, 14);
            int textX = (int)(bb.x + (bb.width - textWidth) / 2);
            int textY = (int)(bb.y + (bb.height - 14) / 2);  // 14 = font size

            DrawText(label, textX, textY, 14, WHITE);
            return;
        }
    }
};

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
        Clay_SetPointerState(
            { (float)GetMouseX(), (float)GetMouseY() },
            IsMouseButtonDown(MOUSE_LEFT_BUTTON)
        );
        Clay_SetLayoutDimensions(clayScreen);

        // Start Clay layout pass
        Clay_BeginLayout();

        CLAY({
            .id = CLAY_ID("RootLayout"),
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_PERCENT(1.0f),
                    .height = CLAY_SIZING_PERCENT(1.0f)
                },
                .childGap = 10,
                .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP },
                .layoutDirection = CLAY_TOP_TO_BOTTOM
            },
            .backgroundColor = { 240, 240, 240, 255 } // light gray
        }) {
            // Top navigation bar
            CLAY({
                .id = CLAY_ID("NavBar"),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_PERCENT(1.0f),
                        .height = CLAY_SIZING_FIXED(50)
                    },
                    .padding = {10, 10, 5, 5},
                    .childGap = 10,
                    .childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER },
                    .layoutDirection = CLAY_LEFT_TO_RIGHT
                },
                .backgroundColor = { 30, 30, 60, 255 }
            }) {
                // Tab: Product
                CLAY({
                    .id = CLAY_ID("ProductTab"),
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_FIXED(100),
                            .height = CLAY_SIZING_FIXED(30)
                        }
                    },
                    .backgroundColor = currentView == PRODUCT_VIEW ? 
                                    Clay_Color{ 120, 120, 255, 255 } : Clay_Color{ 100, 100, 200, 255 }
                });

                if (Clay_PointerOver(CLAY_ID("ProductTab")) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    currentView = PRODUCT_VIEW;
                }

                // Tab: Supplier
                CLAY({
                    .id = CLAY_ID("SupplierTab"),
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_FIXED(100),
                            .height = CLAY_SIZING_FIXED(30)
                        }
                    },
                    .backgroundColor = currentView == SUPPLIER_VIEW ? 
                                    Clay_Color{ 120, 120, 255, 255 } : Clay_Color{ 100, 100, 200, 255 }
                });

                if (Clay_PointerOver(CLAY_ID("SupplierTab")) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    currentView = SUPPLIER_VIEW;
                }

                // Tab: Department
                CLAY({
                    .id = CLAY_ID("DepartmentTab"),
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_FIXED(120),
                            .height = CLAY_SIZING_FIXED(30)
                        }
                    },
                    .backgroundColor = currentView == DEPARTMENT_VIEW ? 
                                    Clay_Color{ 120, 120, 255, 255 } : Clay_Color{ 100, 100, 200, 255 }
                });

                if (Clay_PointerOver(CLAY_ID("DepartmentTab")) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    currentView = DEPARTMENT_VIEW;
                }

                // Tab: Inventory
                CLAY({
                    .id = CLAY_ID("InventoryTab"),
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_FIXED(100),
                            .height = CLAY_SIZING_FIXED(30)
                        }
                    },
                    .backgroundColor = currentView == INVENTORY_VIEW ? 
                                    Clay_Color{ 120, 120, 255, 255 } : Clay_Color{ 100, 100, 200, 255 }
                });

                if (Clay_PointerOver(CLAY_ID("InventoryTab")) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    currentView = INVENTORY_VIEW;
                }
            }
        
            // Content view depending on selected tab
            CLAY({
                .id = CLAY_ID("ContentView"),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_PERCENT(1.0f),
                        .height = CLAY_SIZING_GROW(0, clayScreen.height - 60)
                    },
                    .padding = {20, 20, 20, 20}
                },
                .backgroundColor = { 255, 255, 255, 255 }
            }) {
                if (currentView == PRODUCT_VIEW) {
                    CLAY({
                        .id = CLAY_ID("ProductBox"),
                        .layout = {
                            .sizing = {
                                .width = CLAY_SIZING_PERCENT(1.0f),
                                .height = CLAY_SIZING_PERCENT(1.0f)
                            },
                            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = { 200, 255, 200, 255 }  // Light green
                    });
                } else if (currentView == SUPPLIER_VIEW) {
                    CLAY({
                        .id = CLAY_ID("SupplierBox"),
                        .layout = {
                            .sizing = {
                                .width = CLAY_SIZING_PERCENT(1.0f),
                                .height = CLAY_SIZING_PERCENT(1.0f)
                            },
                            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = { 200, 200, 255, 255 }  // Light blue
                    });
                } else if (currentView == DEPARTMENT_VIEW) {
                    CLAY({
                        .id = CLAY_ID("DepartmentBox"),
                        .layout = {
                            .sizing = {
                                .width = CLAY_SIZING_PERCENT(1.0f),
                                .height = CLAY_SIZING_PERCENT(1.0f)
                            },
                            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = { 255, 255, 200, 255 }  // Light yellow
                    });
                } else if (currentView == INVENTORY_VIEW) {
                    CLAY({
                        .id = CLAY_ID("InventoryBox"),
                        .layout = {
                            .sizing = {
                                .width = CLAY_SIZING_PERCENT(1.0f),
                                .height = CLAY_SIZING_PERCENT(1.0f)
                            },
                            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = { 255, 200, 200, 255 }  // Light red
                    });
                }
            }
            
        }

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

        // Draw label for content view
        const char* label =
            currentView == PRODUCT_VIEW    ? "Product View Box" :
            currentView == SUPPLIER_VIEW   ? "Supplier View Box" :
            currentView == DEPARTMENT_VIEW ? "Department View Box" :
                                        "Inventory View Box";

        Clay_ElementId contentId =
            currentView == PRODUCT_VIEW    ? CLAY_ID("ProductBox") :
            currentView == SUPPLIER_VIEW   ? CLAY_ID("SupplierBox") :
            currentView == DEPARTMENT_VIEW ? CLAY_ID("DepartmentBox") :
                                        CLAY_ID("InventoryBox");

        for (int i = 0; i < commands.length; ++i) {
            Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&commands, i);
            if (cmd->commandType == CLAY_RENDER_COMMAND_TYPE_RECTANGLE &&
                cmd->id == contentId.id) {
                    
                const auto& bb = cmd->boundingBox;

                int textWidth = MeasureText(label, 20);
                int textX = (int)(bb.x + (bb.width - textWidth) / 2);
                int textY = (int)(bb.y + (bb.height - 20) / 2);

                DrawText(label, textX, textY, 20, DARKGRAY);
                break;
            }
        }

        Clay_ElementId productTabId    = CLAY_ID("ProductTab");
        Clay_ElementId supplierTabId   = CLAY_ID("SupplierTab");
        Clay_ElementId departmentTabId = CLAY_ID("DepartmentTab");
        Clay_ElementId inventoryTabId  = CLAY_ID("InventoryTab");

        DrawTabLabel(&productTabId,    "Product",    commands);
        DrawTabLabel(&supplierTabId,   "Supplier",   commands);
        DrawTabLabel(&departmentTabId, "Department", commands);
        DrawTabLabel(&inventoryTabId,  "Inventory",  commands);

        const char* viewLabel =
            currentView == PRODUCT_VIEW   ? "Product View" :
            currentView == SUPPLIER_VIEW  ? "Supplier View" :
            currentView == DEPARTMENT_VIEW? "Department View" :
                                     "Inventory View";

        int fontSize = 20;
        int labelWidth = MeasureText(viewLabel, fontSize);
        int x = (screenWidth - labelWidth) / 2;
        int y = screenHeight - 10;  // Still near bottom

        DrawText(viewLabel, x, y, fontSize, GRAY);
        EndDrawing();
    }

    // Clean up
    CloseWindow();
    free(memory);

    return 0;
}
