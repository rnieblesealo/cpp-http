#include "helper.h"

void helper::exitWithError(
    const std::string &message) { // NOTE: const in param: this value
                                  // should NOT be moded inside func
  std::cerr << message << std::endl;
  exit(1);
}
