#pragma once

#include <gui/scene_manager.h>

// List of scenes
typedef enum {
    PiCmdSceneStart,
    PiCmdSceneCmd,
} PiCmdScene;

extern const SceneManagerHandlers pi_cmd_scene_handlers;

// Scene on_enter handlers
void pi_cmd_scene_start_on_enter(void* context);
void pi_cmd_scene_cmd_on_enter(void* context);

// Scene on_event handlers
bool pi_cmd_scene_start_on_event(void* context, SceneManagerEvent event);
bool pi_cmd_scene_cmd_on_event(void* context, SceneManagerEvent event);

// Scene on_exit handlers
void pi_cmd_scene_start_on_exit(void* context);
void pi_cmd_scene_cmd_on_exit(void* context);
