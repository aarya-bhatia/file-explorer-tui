#pragma once
#include "app_state.h"

struct ActionHandler {
  bool operator()(int ch, AppState &state);
};
