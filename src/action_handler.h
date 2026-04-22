#pragma once
#include "app_state.h"
#include "action_callbacks.h"

struct ActionHandler {
  bool operator()(int ch, AppState &state);
  std::unique_ptr<ActionCallback> next_callback;  

private:
  bool on_enter(AppState &state);
  bool create_file_prompt(AppState &state);
};
