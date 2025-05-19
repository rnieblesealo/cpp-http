#include "helper.h"
#include <fmt/color.h>
#include <fmt/core.h>

void helper::exitWithError(
    const std::string &message) { // NOTE: const in param: this value

  fmt::print(stderr, fmt::emphasis::bold | fg(fmt::color::red), "ERROR: {}\n",
             message);
  exit(EXIT_FAILURE);
}
