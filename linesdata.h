#ifndef LINESDATA_H
#define LINESDATA_H

#include <array>

enum FieldColumns {
    none,
    blocked,
    robot,
    col_one,
    col_two,
    col_three
};

//typedef std::array<FieldColumns, 8> FullLinesData;
typedef std::array<FieldColumns, 8> FieldLinesData;

#endif // LINESDATA_H
