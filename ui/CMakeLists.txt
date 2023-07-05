SET(SOURCES screens/ui_Screen1.c
    ui.c
    ui_helpers.c
    ui_events.c
    fonts/ui_font_chinese.c)

add_library(ui ${SOURCES})
