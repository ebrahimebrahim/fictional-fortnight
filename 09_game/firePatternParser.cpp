#include "firePatternParser.h"


std::vector<std::string> split(std::string str, const char * delimiters) {

  std::vector<std::string> split_string;

  char * cstr = new char [str.length()];
  strcpy(cstr,str.c_str());

  char * pch = strtok(cstr,delimiters);
  while(pch!=nullptr){
    split_string.push_back(std::string(pch));
    pch = strtok(nullptr,delimiters);
  }

  delete [] cstr;

  return split_string; //RVO? I hope.
}


FirePattern parseFirePattern(std::string str) {
  FirePattern firePattern;
  for (std::string firePatternStepStr : split(str," ")) {
    FirePatternStep firePatternStep;
    std::vector<std::string> firePatternStepStrSplit = split(firePatternStepStr,";");
    if (firePatternStepStrSplit.size()!=2)
      return FirePattern(); // parse error; return empty
    std::string firePatternStepDirsStr = firePatternStepStrSplit[0];
    std::string firePatternStepWaitStr = firePatternStepStrSplit[1];
    for (std::string firePatterDirStr : split(firePatternStepDirsStr,",")){
      std::vector<std::string> firePatternDirStrSplit = split(firePatterDirStr,":");
      if (firePatternDirStrSplit.size()!=2)
        return FirePattern(); // parse error; return empty
      if (firePatternDirStrSplit[0].length()<1 || firePatternDirStrSplit[1].length()<1)
        return FirePattern(); // parse error; return empty
      char dir = firePatternDirStrSplit[0][0];
      int vel = std::stoi(firePatternDirStrSplit[1]);
      switch (dir) {
        case 'U': firePatternStep.u=vel; break;
        case 'D': firePatternStep.d=vel; break;
        case 'L': firePatternStep.l=vel; break;
        case 'R': firePatternStep.r=vel; break;
        default : return FirePattern(); // parse error; return empty
      }
    }
    firePatternStep.wait = std::stoi(firePatternStepWaitStr); // this throws exceptions and i should catch them... TODO
    firePattern.push_back(firePatternStep);
  }
  return firePattern; // hoping for RVO again.
}
