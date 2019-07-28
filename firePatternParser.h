#pragma once
#include <string>
#include <cstring>
#include <vector>
#include "utilities.h"

struct FirePatternStep {
  int u=-1; // velocity of upward shot, if -1 then there's no up shot;
  int d=-1; // etc.
  int l=-1;
  int r=-1;
  int wait=50;
};

typedef std::vector<FirePatternStep> FirePattern;

FirePattern parseFirePattern(std::string);
// If parse error or empty string given, then return an empty vector
