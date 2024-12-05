#include "../pi_cmd_app_i.h"

void pi_cmd_scene_start_on_enter(void* context) {
    PiCmdApp* app = context;
    submenu_reset(app->submenu);
    submenu_add_item(app->submenu, "Pi", 0, NULL, NULL);
    submenu_add_item(app->submenu, "CMD", 1, pi_cmd_submenu_callback, app);
    submenu_add_item(app->submenu, "Option 2", 2, pi_cmd_submenu_callback, app);
    submenu_add_item(app->submenu, "Option 3", 3, pi_cmd_submenu_callback, app);
    submenu_add_item(app->submenu, "Option 4", 4, pi_cmd_submenu_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, PiCmdViewSubmenu);
}

bool pi_cmd_scene_start_on_event(void* context, SceneManagerEvent event) {
    PiCmdApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == 1) {
            scene_manager_next_scene(app->scene_manager, PiCmdSceneCmd);
            consumed = true;
        }
    }

    return consumed;
}

void pi_cmd_scene_start_on_exit(void* context) {
    PiCmdApp* app = context;
    submenu_reset(app->submenu);
}

void pi_cmd_scene_cmd_on_enter(void* context) {
    PiCmdApp* app = context;
    popup_reset(app->popup);
    popup_set_header(app->popup, "Sending Command", 64, 32, AlignCenter, AlignTop);
    popup_set_text(app->popup, "Waiting for response...", 64, 32, AlignCenter, AlignCenter);
    view_dispatcher_switch_to_view(app->view_dispatcher, PiCmdViewPopup);

    uint8_t uart_code = 0x01;
    furi_hal_uart_tx(UART_CH, &uart_code, 1);
    FURI_LOG_I(TAG, "Sent UART code: 0x01");
}

bool pi_cmd_scene_cmd_on_event(void* context, SceneManagerEvent event) {
    PiCmdApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PiCmdCustomEventUartReceived) {
            scene_manager_previous_scene(app->scene_manager);
            consumed = true;
        }
    }

    return consumed;
}

void pi_cmd_scene_cmd_on_exit(void* context) {
    PiCmdApp* app = context;
    popup_reset(app->popup);
}

const SceneManagerHandlers pi_cmd_scene_handlers = {
    .on_enter_handlers =
        {
            [PiCmdSceneStart] = pi_cmd_scene_start_on_enter,
            [PiCmdSceneCmd] = pi_cmd_scene_cmd_on_enter,
        },
    .on_event_handlers =
        {
            [PiCmdSceneStart] = pi_cmd_scene_start_on_event,
            [PiCmdSceneCmd] = pi_cmd_scene_cmd_on_event,
        },
    .on_exit_handlers =
        {
            [PiCmdSceneStart] = pi_cmd_scene_start_on_exit,
            [PiCmdSceneCmd] = pi_cmd_scene_cmd_on_exit,
        },
    .scene_num = PiCmdSceneCmd + 1,
};
