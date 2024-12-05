# SubGhz and Flipagotchi Applications for Flipper Zero

## Overview

This document covers two applications for the Flipper Zero device: the SubGhz application and the Flipagotchi application. The SubGhz application interacts with various sub-gigahertz radio protocols, while the Flipagotchi application interfaces with a Pwnagotchi device using UART communication.

## SubGhz Application

### Main Components

1. **SubGhz Struct**: The core structure that holds all the necessary components and state for the application.
2. **SubGhzTxRx**: Handles the transmit and receive functionality of the application.
3. **View System**: Utilizes Flipper Zero's GUI framework with components like ViewDispatcher, SceneManager, and various view modules.
4. **Frequency Analyzer**: Allows for analyzing sub-gigahertz frequencies.
5. **Protocol Handling**: Supports various sub-gigahertz protocols for decoding and encoding.
6. **File Management**: Handles saving and loading of protocol data and application settings.
7. **RPC System**: Enables remote procedure calls for external control of the application.
8. **Scenes**: Manage the application's state and control flow.
9. **Views**: Handle the visual representation and user interaction for specific functionalities.

### Key Functionalities

1. **Initialization and Setup**: The `subghz_alloc` function initializes the SubGhz application.
2. **Transmit and Receive**: The SubGhzTxRx module provides core functionality for radio operations.
3. **Protocol Handling**: Supports various sub-gigahertz protocols.
4. **User Interface**: Uses a scene-based UI system.
5. **File Operations**: Manages protocol data and settings.
6. **Frequency Management**: Handles radio presets and frequency validation.
7. **External Device Support**: Works with external radio devices.
8. **RPC Functionality**: Implements a system for external control.

### Scenes and Views

The SubGhz application uses a scene-based architecture for managing its user interface and application flow.

#### Scenes
Scenes manage the application's state and control flow, defined in the `scenes` directory.

#### Views
Views handle the visual representation and user interaction for specific functionalities, defined in the `views` directory.

### Program Flow

1. Application initialization
2. Main event loop (ViewDispatcher)
3. User interactions or RPC commands trigger scene changes and actions
4. Perform various operations based on the current scene and user input
5. Transition between scenes and update views
6. Continue running until user exit or external RPC command stops it

## Flipagotchi Application

The Flipagotchi application extends the Flipper Zero firmware to interface with a Pwnagotchi device using UART communication.

### UART Procedures

#### UART Setup and Initialization

1. Predefined UART channel and baud rate:
   ```c
   #define PWNAGOTCHI_UART_CHANNEL FuriHalSerialIdUsart
   #define PWNAGOTCHI_UART_BAUD 115200
   ```

2. UART initialization in `flipagotchi_app_alloc`:
   ```c
   app->serial_handle = furi_hal_serial_control_acquire(PWNAGOTCHI_UART_CHANNEL);
   furi_check(app->serial_handle);
   furi_hal_serial_init(app->serial_handle, PWNAGOTCHI_UART_BAUD);
   furi_hal_serial_async_rx_start(app->serial_handle, flipagotchi_on_irq_cb, app, true);
   ```

#### UART Interrupt Handling

Uses an interrupt-driven approach with a callback function:

```c
static void flipagotchi_on_irq_cb(FuriHalSerialHandle* serial_handle, FuriHalSerialRxEvent ev, void* context) {
    FlipagotchiApp* app = context;
    uint8_t data = furi_hal_serial_async_rx(serial_handle);

    if(ev & FuriHalSerialRxEventData) {
        furi_stream_buffer_send(app->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventRx);
    }
}
```

#### Data Processing

A worker thread processes received data:

```c
static int32_t flipagotchi_worker(void* context) {
    // ... [worker thread implementation]
}
```

#### Command Processing

The `flipagotchi_exec_cmd` function interprets commands from the Pwnagotchi:

```c
static bool flipagotchi_exec_cmd(PwnDumpModel* model) {
    if(message_queue_has_message(model->queue)) {
        PwnCommand cmd;
        message_queue_pop_message(model->queue, &cmd);
        
        switch(cmd.parameterCode) {
            case 0x04: // Process Face
                // ...
            case 0x05: // Process Name
                // ...
            case 0x06: // Process channel
                // ...
            // ... [other cases]
        }
    }
    return false;
}
```

#### UART Cleanup

Proper cleanup of UART resources:

```c
static void flipagotchi_app_free(FlipagotchiApp* app) {
    // ...
    furi_hal_serial_deinit(app->serial_handle);
    furi_hal_serial_control_release(app->serial_handle);
    // ...
}
```

## Conclusion

The SubGhz and Flipagotchi applications demonstrate the versatility of the Flipper Zero device. The SubGhz application provides a sophisticated system for interacting with sub-gigahertz radio protocols, utilizing a modular design with separate components for UI, radio operations, and protocol handling. Its scene-based architecture and view system offer a structured approach to managing complex state and user interfaces.

The Flipagotchi application extends this functionality by enabling communication with Pwnagotchi devices through UART. It efficiently receives and processes data, updating its display and status in real-time based on received commands.

Both applications showcase the Flipper Zero's capability to handle different types of communication protocols and interact with various external devices, making it a powerful tool for radio exploration, experimentation, and device interaction.