# Pi CMD Application

This application provides a simple interface for sending commands via UART and receiving responses.

## Application Structure

```
applications/application_users/pi_cmd_app/
├── application.fam
├── pi_cmd_app.c
├── pi_cmd_app_i.h
├── scenes/
│   ├── pi_cmd_scene.c
│   └── pi_cmd_scene.h
└── README.md
```

- `application.fam`: Application manifest file
- `pi_cmd_app.c`: Main application code
- `pi_cmd_app_i.h`: Internal application structure and declarations
- `scenes/`: Directory containing scene-related files
  - `pi_cmd_scene.c`: Scene implementations
  - `pi_cmd_scene.h`: Scene declarations
- `README.md`: This file, containing application documentation

## Functionality

1. Main Menu:
   - "Pi" option (does nothing when selected)
   - "CMD" option (sends UART command and waits for response)
   - "Option 2" (behaves the same as "CMD")
   - "Option 3" (behaves the same as "CMD")
   - "Option 4" (behaves the same as "CMD")

2. UART Communication:
   - Sends code 0x01 when an option is selected
   - Waits for a reply of 0x02
   - Uses USART1 with a baud rate of 115200

3. User Interface:
   - Uses SceneManager for managing application flow
   - Uses Submenu for the main menu
   - Uses Popup for displaying the "Sending Command" screen

4. Threading:
   - Main thread for UI and application logic
   - Separate thread for handling UART reception

## Usage

Navigate the menu using the Flipper Zero's d-pad. Press the center button to select an option. The "Back" button will exit the application.

When "CMD" or any of the numbered options are selected, the application will send a UART command and wait for a response. Once a response is received, it will return to the main menu.

## Note

This application disables the Flipper Zero's console output on USART1 while running. The console is re-enabled when the application exits.