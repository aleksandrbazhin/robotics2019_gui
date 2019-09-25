#ifndef LINESDATA_H
#define LINESDATA_H

#include <array>

enum FieldColumns {
    robot = 0,
    col_one = 1,
    col_two = 2,
    col_three = 3,
    none,
    blocked
};

typedef std::array<FieldColumns, 8> FieldLinesData;

static const FieldLinesData defaultLinesData = {
    FieldColumns::none, FieldColumns::none, FieldColumns::none, FieldColumns::none,
    FieldColumns::none, FieldColumns::none, FieldColumns::none, FieldColumns::none
};

#endif // LINESDATA_H
