#ifndef _SOVOL_FIELDWITHSHIFTERS_HH
#define _SOVOL_FIELDWITHSHIFTERS_HH 1

#include "Field.hh"
#include "FieldShifter.hh"
#include <vector>

class FieldWithShifters : public Field
{
private:
    std::shared_ptr<Field> field;
    std::vector<std::shared_ptr<FieldShifter>> shifters;
public:
    FieldWithShifters(/* args */);
};


#endif