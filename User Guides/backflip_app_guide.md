# BackFlip App Development Guide

This guide will walk you through creating the BackFlip app for the Flipper Zero device. The app will feature a menu with 5 options, each leading to a separate screen where the user can send a unique UART code.

## Step 1: Project Setup

1. Create a new directory for your app in the Flipper Zero firmware:
   ```
   mkdir applications/backflip
   cd applications/backflip
   ```

2. Create the necessary files:
   ```
   touch application.fam
   touch backflip.c
   mkdir scenes
   touch scenes/backflip_scene.c
   touch scenes/backflip_scene.h
   mkdir views
   touch views/backflip_view.c
   touch views/backflip_view.h
   ```

## Step 2: Define the Application Structure

In `backflip.c`, define the main structure for the BackFlip app:

```c
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include "scenes/backflip_scene.h"
#include "views/backflip_view.h"

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    Submenu* submenu;
    BackflipCustomView* custom_view;
    Popup* popup;
    FuriThread* uart_thread;
    FuriStreamBuffer* rx_stream;
    FuriHalSerialHandle* serial_handle;
} BackflipApp;

typedef enum {
    BackflipViewSubmenu,
    BackflipViewCustom,
    BackflipViewPopup,
} BackflipView;
```

## Step 3: Implement the Scene Manager

In `scenes/backflip_scene.h`:

```c
#pragma once
#include <gui/scene_manager.h>

// Scene enumeration
typedef enum {
    BackflipSceneMenu,
    BackflipSceneOption1,
    BackflipSceneOption2,
    BackflipSceneOption3,
    BackflipSceneOption4,
    BackflipSceneOption5,
} BackflipScene;

extern const SceneManagerHandlers backflip_scene_handlers;

// Declare scene on_enter handlers
void backflip_scene_menu_on_enter(void* context);
void backflip_scene_option_on_enter(void* context);

// Declare scene on_event handlers
bool backflip_scene_menu_on_event(void* context, SceneManagerEvent event);
bool backflip_scene_option_on_event(void* context, SceneManagerEvent event);

// Declare scene on_exit handlers
void backflip_scene_menu_on_exit(void* context);
void backflip_scene_option_on_exit(void* context);
```

In `scenes/backflip_scene.c`:

```c
#include "backflip_scene.h"
#include "../backflip.h"

void backflip_scene_menu_on_enter(void* context) {
    BackflipApp* app = context;
    submenu_reset(app->submenu);
    submenu_add_item(app->submenu, "Option 1", 0, backflip_scene_menu_option_selected, app);
    submenu_add_item(app->submenu, "Option 2", 1, backflip_scene_menu_option_selected, app);
    submenu_add_item(app->submenu, "Option 3", 2, backflip_scene_menu_option_selected, app);
    submenu_add_item(app->submenu, "Option 4", 3, backflip_scene_menu_option_selected, app);
    submenu_add_item(app->submenu, "Option 5", 4, backflip_scene_menu_option_selected, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, BackflipViewSubmenu);
}

bool backflip_scene_menu_on_event(void* context, SceneManagerEvent event) {
    BackflipApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_next_scene(app->scene_manager, BackflipSceneOption1 + event.event);
        consumed = true;
    }

    return consumed;
}

void backflip_scene_menu_on_exit(void* context) {
    BackflipApp* app = context;
    submenu_reset(app->submenu);
}

void backflip_scene_option_on_enter(void* context) {
    BackflipApp* app = context;
    backflip_custom_view_set_callback(app->custom_view, backflip_option_view_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, BackflipViewCustom);
}

bool backflip_scene_option_on_event(void* context, SceneManagerEvent event) {
    BackflipApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == BackflipCustomEventSend) {
            // Send UART code based on current scene
            uint8_t uart_code = scene_manager_get_scene_state(app->scene_manager, BackflipSceneOption1);
            backflip_send_uart_code(app, uart_code);
            scene_manager_previous_scene(app->scene_manager);
            consumed = true;
        }
    }

    return consumed;
}

void backflip_scene_option_on_exit(void* context) {
    BackflipApp* app = context;
    backflip_custom_view_set_callback(app->custom_view, NULL, NULL);
}

const SceneManagerHandlers backflip_scene_handlers = {
    .on_enter_handlers = {
        [BackflipSceneMenu] = backflip_scene_menu_on_enter,
        [BackflipSceneOption1] = backflip_scene_option_on_enter,
        [BackflipSceneOption2] = backflip_scene_option_on_enter,
        [BackflipSceneOption3] = backflip_scene_option_on_enter,
        [BackflipSceneOption4] = backflip_scene_option_on_enter,
        [BackflipSceneOption5] = backflip_scene_option_on_enter,
    },
    .on_event_handlers = {
        [BackflipSceneMenu] = backflip_scene_menu_on_event,
        [BackflipSceneOption1] = backflip_scene_option_on_event,
        [BackflipSceneOption2] = backflip_scene_option_on_event,
        [BackflipSceneOption3] = backflip_scene_option_on_event,
        [BackflipSceneOption4] = backflip_scene_option_on_event,
        [BackflipSceneOption5] = backflip_scene_option_on_event,
    },
    .on_exit_handlers = {
        [BackflipSceneMenu] = backflip_scene_menu_on_exit,
        [BackflipSceneOption1] = backflip_scene_option_on_exit,
        [BackflipSceneOption2] = backflip_scene_option_on_exit,
        [BackflipSceneOption3] = backflip_scene_option_on_exit,
        [BackflipSceneOption4] = backflip_scene_option_on_exit,
        [BackflipSceneOption5] = backflip_scene_option_on_exit,
    },
    .scene_num = BackflipSceneOption5 + 1,
};
```

## Step 4: Implement the Custom View

In `views/backflip_view.h`:

```c
#pragma once
#include <gui/view.h>

typedef struct BackflipCustomView BackflipCustomView;

typedef void (*BackflipCustomViewCallback)(void* context);

BackflipCustomView* backflip_custom_view_alloc();
void backflip_custom_view_free(BackflipCustomView* custom_view);
View* backflip_custom_view_get_view(BackflipCustomView* custom_view);
void backflip_custom_view_set_callback(
    BackflipCustomView* custom_view,
    BackflipCustomViewCallback callback,
    void* context);
```

In `views/backflip_view.c`:

```c
#include "backflip_view.h"
#include <gui/elements.h>

struct BackflipCustomView {
    View* view;
    BackflipCustomViewCallback callback;
    void* context;
};

typedef struct {
    char* message;
} BackflipCustomViewModel;

static void backflip_custom_view_draw_callback(Canvas* canvas, void* _model) {
    BackflipCustomViewModel* model = _model;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, model->message);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 30, AlignCenter, AlignTop, "Press OK to send");
}

static bool backflip_custom_view_input_callback(InputEvent* event, void* context) {
    BackflipCustomView* custom_view = context;
    bool consumed = false;

    if(event->type == InputTypeShort && event->key == InputKeyOk) {
        if(custom_view->callback) {
            custom_view->callback(custom_view->context);
        }
        consumed = true;
    }

    return consumed;
}

BackflipCustomView* backflip_custom_view_alloc() {
    BackflipCustomView* custom_view = malloc(sizeof(BackflipCustomView));
    custom_view->view = view_alloc();
    view_set_context(custom_view->view, custom_view);
    view_allocate_model(custom_view->view, ViewModelTypeLocking, sizeof(BackflipCustomViewModel));
    view_set_draw_callback(custom_view->view, backflip_custom_view_draw_callback);
    view_set_input_callback(custom_view->view, backflip_custom_view_input_callback);

    with_view_model(
        custom_view->view,
        BackflipCustomViewModel * model,
        { model->message = "Ready to send"; },
        true);

    return custom_view;
}

void backflip_custom_view_free(BackflipCustomView* custom_view) {
    view_free(custom_view->view);
    free(custom_view);
}

View* backflip_custom_view_get_view(BackflipCustomView* custom_view) {
    return custom_view->view;
}

void backflip_custom_view_set_callback(
    BackflipCustomView* custom_view,
    BackflipCustomViewCallback callback,
    void* context) {
    custom_view->callback = callback;
    custom_view->context = context;
}
```

## Step 5: Implement UART Functionality

Add these functions to `backflip.c`:

```c
#define BACKFLIP_UART_CHANNEL FuriHalSerialIdUsart
#define BACKFLIP_UART_BAUD 115200

static void backflip_uart_init(BackflipApp* app) {
    app->serial_handle = furi_hal_serial_control_acquire(BACKFLIP_UART_CHANNEL);
    furi_check(app->serial_handle);
    furi_hal_serial_init(app->serial_handle, BACKFLIP_UART_BAUD);
}

static void backflip_uart_deinit(BackflipApp* app) {
    furi_hal_serial_deinit(app->serial_handle);
    furi_hal_serial_control_release(app->serial_handle);
}

static void backflip_send_uart_code(BackflipApp* app, uint8_t code) {
    furi_hal_serial_tx(app->serial_handle, &code, 1);
}
```

## Step 6: Implement the Main Application Logic

In `backflip.c`, add the following functions:

```c
static void backflip_alloc(BackflipApp* app) {
    app->gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&backflip_scene_handlers, app);
    app->submenu = submenu_alloc();
    app->custom_view = backflip_custom_view_alloc();
    app->popup = popup_alloc();

    view_dispatcher_add_view(app->view_dispatcher, BackflipViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(app->view_dispatcher, BackflipViewCustom, backflip_custom_view_get_view(app->custom_view));
    view_dispatcher_add_view(app->view_dispatcher, BackflipViewPopup, popup_get_view(app->popup));

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    backflip_uart_init(app);
}

static void backflip_free(BackflipApp* app) {
    view_dispatcher_remove_view(app->view_dispatcher, BackflipViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, BackflipViewCustom);
    view_dispatcher_remove_view(app->view_dispatcher, BackflipViewPopup);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    backflip_custom_view_free(app->custom_view);
    popup_free(app->popup);

    backflip_uart_deinit(app);

    furi_record_close(RECORD_GUI);
}

int32_t backflip_app(void* p) {
    UNUSED(p);
    BackflipApp* backflip = malloc(sizeof(BackflipApp));

    backflip_alloc(backflip);

    scene_manager_next_scene(backflip->scene_manager, BackflipSceneMenu);
    view_dispatcher_run(backflip->view_dispatcher);

    backflip_free(backflip);
    free(backflip);
    return 0;
}
```

## Step 7: Create the Application Manifest

In `application.fam`:

```python
App(
    appid="backflip",
    name="BackFlip",
    apptype=FlipperAppType.EXTERNAL,
    entry_point="backflip_app",
    cdefines=["APP_BACKFLIP"],
    requires=[
        "gui",
    ],
    stack_size=2 * 1024,
    order=20,
    fap_icon="backflip_10px.png",
    fap_category="Tools",
)
```

## Step 8: Build and Run

1. Add your app to the firmware build system (you may need to modify the appropriate build files).
2. Build the firmware with your new app included.
3. Flash the firmware to your Flipper Zero device.
4. Run the BackFlip app from the Apps menu on your Flipper Zero.

This guide provides a basic structure for the BackFlip app. You may need to adjust some parts to fit the exact Flipper Zero SDK you're using and add error handling and additional features as needed.

Remember to test thoroughly, especially the UART communication, to ensure it works as expected with your target device.