SET(SOURCES screens/ui_Screen1.c
    ui.c
    ui_helpers.c
    fonts/ui_font_chinese.c)

add_library(ui ${SOURCES})
