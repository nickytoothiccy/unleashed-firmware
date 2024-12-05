#include "pi_cmd_app_i.h"

static bool pi_cmd_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    PiCmdApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool pi_cmd_back_event_callback(void* context) {
    furi_assert(context);
    PiCmdApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void pi_cmd_tick_event_callback(void* context) {
    furi_assert(context);
    PiCmdApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

void pi_cmd_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    PiCmdApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static int32_t pi_cmd_uart_worker(void* context) {
    furi_assert(context);
    PiCmdApp* app = context;
    uint8_t data;

    while(1) {
        if(furi_hal_uart_is_rx_available(UART_CH)) {
            data = furi_hal_uart_getc(UART_CH);
            if(data == 0x02) {
                FURI_LOG_I(TAG, "Received UART code: 0x02");
                view_dispatcher_send_custom_event(
                    app->view_dispatcher, PiCmdCustomEventUartReceived);
            }
        }
        furi_delay_ms(10);
    }

    return 0;
}

PiCmdApp* pi_cmd_app_alloc() {
    PiCmdApp* app = malloc(sizeof(PiCmdApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&pi_cmd_scene_handlers, app);
    app->submenu = submenu_alloc();
    app->popup = popup_alloc();

    view_dispatcher_add_view(
        app->view_dispatcher, PiCmdViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(app->view_dispatcher, PiCmdViewPopup, popup_get_view(app->popup));

    furi_hal_console_disable();
    furi_hal_uart_set_br(UART_CH, UART_BAUDRATE);
    furi_hal_uart_set_irq_cb(UART_CH, NULL, NULL);

    app->uart_thread = furi_thread_alloc_ex("PiCmdUartWorker", 1024, pi_cmd_uart_worker, app);

    return app;
}

void pi_cmd_app_free(PiCmdApp* app) {
    furi_assert(app);

    furi_thread_free(app->uart_thread);
    furi_hal_uart_set_irq_cb(UART_CH, NULL, NULL);
    furi_hal_console_enable();

    view_dispatcher_remove_view(app->view_dispatcher, PiCmdViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, PiCmdViewPopup);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    popup_free(app->popup);
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t pi_cmd_app(void* p) {
    UNUSED(p);
    PiCmdApp* app = pi_cmd_app_alloc();

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, pi_cmd_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, pi_cmd_back_event_callback);
    view_dispatcher_set_tick_event_callback(app->view_dispatcher, pi_cmd_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, PiCmdSceneStart);

    furi_thread_start(app->uart_thread);

    view_dispatcher_run(app->view_dispatcher);

    pi_cmd_app_free(app);
    return 0;
}
