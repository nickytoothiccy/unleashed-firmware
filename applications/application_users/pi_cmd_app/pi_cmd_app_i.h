#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include "scenes/pi_cmd_scene.h"

#define TAG "PiCmdApp"

#define UART_CH       (&furi_hal_uart_usart)
#define UART_BAUDRATE 115200

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    Submenu* submenu;
    Popup* popup;
    FuriThread* uart_thread;
    FuriStreamBuffer* rx_stream;
} PiCmdApp;

typedef enum {
    PiCmdViewSubmenu,
    PiCmdViewPopup,
} PiCmdView;

typedef enum {
    PiCmdCustomEventUartReceived,
} PiCmdCustomEvent;

void pi_cmd_submenu_callback(void* context, uint32_t index);
