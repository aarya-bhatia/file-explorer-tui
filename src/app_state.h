#pragma once
#include <map>
#include <string>
#include <vector>
#include <time.h>

#define DATA_DIRECTORY "data/"

struct AppState {
  AppState();
  ~AppState();
  bool running = true;

};
