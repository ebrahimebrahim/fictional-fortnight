#include <string>
#include <cstring>
#include <vector>

struct FirePatternStep {
  int u=-1; // velocity of upward shot, if -1 then there's no up shot;
  int d=-1; // etc.
  int l=-1;
  int r=-1;
  int wait=50;
};

typedef std::vector<FirePatternStep> FirePattern;

std::vector<std::string> split(std::string str, const char * delimiters);

FirePattern parseFirePattern(std::string);
// If parse error or empty string given, then return an empty vector
