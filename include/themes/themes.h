// theme.h
#pragma once
#include "clay.h"
#include "raylib.h"

struct Theme {
    // Navbar Colors
    Clay_Color navBarColor;
    Clay_Color tabActiveColor;
    Clay_Color tabInactiveColor;

    // Content Colors
    Clay_Color headerBackgroundColor;
    Clay_Color contentBackgroundColor;

    // Section Colors
    Clay_Color recordDetailsBoxColor;
    Clay_Color recordCrudBoxColor;
    Clay_Color recordsDisplayBoxColor;

    // Text
    Color tabLabelTextColor;
    Color headerTextColor;
    Color contentTextColor;

    // Font Sizes
    int tabFontSize;
    int headerFontSize;
    int contentFontSize;
};

extern const Theme LightTheme;
extern const Theme DarkTheme;
extern const Theme* CurrentTheme;
