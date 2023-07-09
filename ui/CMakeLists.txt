SET(SOURCES screens/ui_start.c
    screens/ui_userScreen.c
    screens/ui_passwordScreen.c
    screens/ui_adminScreen.c
    screens/ui_whiteScreen.c
    ui.c
    ui_helpers.c
    ui_events.c
    fonts/ui_font_chinese.c
    fonts/ui_font_chinese24.c)

add_library(ui ${SOURCES})
