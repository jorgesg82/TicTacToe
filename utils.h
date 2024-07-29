#ifndef UTILS_H
#define UTILS_H

#include <array>

enum class box {
    X,
    O,
    VOID
};

using Board = std::array<std::array<box, 3>, 3>;

#endif