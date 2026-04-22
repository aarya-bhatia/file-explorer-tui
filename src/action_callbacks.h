#pragma once
#include "app_state.h"

struct ActionCallback {
  virtual bool run(AppState &state) = 0;
  virtual ~ActionCallback() {}
};

struct CreateFileCallback: ActionCallback {
  bool run(AppState &state);
};

