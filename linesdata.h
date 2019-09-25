#ifndef LINESDATA_H
#define LINESDATA_H

#include <array>

enum FieldColumns {
    robot = 0,
    one = 1,
    two = 2,
    three = 3
};

typedef std::array<FieldColumns, 8> linesData;

#endif // LINESDATA_H
