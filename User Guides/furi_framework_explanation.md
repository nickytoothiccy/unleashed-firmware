# Furi Framework Explanation

## Introduction to Furi

Furi is a custom framework developed for the Flipper Zero device. It serves as an abstraction layer between the hardware and the application layer, providing a set of tools and utilities to simplify development and ensure consistent behavior across the system.

## Core Components

The Furi framework consists of several core components:

1. **furi.h / furi.c**: These files contain the main initialization functions for the Furi framework.
2. **flipper.h / flipper.c**: These files handle the initialization of the Flipper device and its services.
3. **Core utilities**: Located in the `core/` directory, these include various system utilities such as event handling, memory management, threading, and logging.

## Initialization Process

The initialization of the Furi framework and the Flipper device happens in two main steps:

1. **Furi Initialization** (`furi_init()`):
   - Initializes the logging system
   - Initializes the record system (likely for data storage and retrieval)

2. **Flipper Initialization** (`flipper_init()`):
   - Prints the firmware version
   - Starts all the defined Flipper services

After initialization, `furi_run()` is called to start the FreeRTOS scheduler.

## Key Functions and Their Purposes

### In furi.c:

- `furi_init()`: Initializes the Furi framework. It checks that it's not being called from an interrupt context and that the scheduler hasn't started yet.
- `furi_run()`: Starts the FreeRTOS scheduler. This function doesn't return under normal operation.

### In flipper.c:

- `flipper_init()`: Initializes the Flipper device. It prints the firmware version and starts all the defined Flipper services.
- `flipper_print_version()`: Utility function to print version information.
- `vApplicationGetIdleTaskMemory()` and `vApplicationGetTimerTaskMemory()`: These functions are required by FreeRTOS for static memory allocation of the idle task and timer task.

## Integration with FreeRTOS

Furi is built on top of FreeRTOS, a real-time operating system kernel for embedded devices. This integration is evident in several ways:

1. The use of FreeRTOS task management functions like `xTaskGetSchedulerState()` and `vTaskStartScheduler()`.
2. The implementation of `vApplicationGetIdleTaskMemory()` and `vApplicationGetTimerTaskMemory()` for static memory allocation.
3. The use of FreeRTOS-style naming conventions (e.g., `vApplication...` functions).

## Best Practices for Using Furi

When developing applications or services for the Flipper Zero using the Furi framework:

1. Always ensure that `furi_init()` is called before any other Furi functions.
2. Use the provided core utilities (like event handling, threading, and logging) instead of implementing your own.
3. Be aware of the FreeRTOS integration and follow real-time programming best practices.
4. When adding new services, follow the pattern established in `flipper_init()` for consistent initialization.
5. Utilize the logging system (`FURI_LOG_*` macros) for debugging and system monitoring.
6. Be mindful of memory management, using the provided memory management functions when possible.

## Conclusion

The Furi framework provides a robust foundation for developing applications and services for the Flipper Zero device. By abstracting hardware interactions and providing a set of core utilities, it allows developers to focus on creating functionality rather than dealing with low-level details. Understanding the initialization process and core components of Furi is crucial for effective development on the Flipper Zero platform.