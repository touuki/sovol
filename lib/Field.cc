#include "Field.hh"
#include "Config.hh"

REGISTER_MULTITON(Field, CustomField)

Field::~Field(){};

EMField Field::operator()(const Vector3<double> &position, double t) const {
    return operator()(position.x, position.y, position.z, t);
};

double CustomField::defaultFunction(double, double, double, double) {
    return 0;
};

CustomField::CustomField()
    : CustomField(Config::getPointer(SOVOL_CONFIG_KEY(CUSTOMFIELD_EX),
                                     CustomField::defaultFunction),
                  Config::getPointer(SOVOL_CONFIG_KEY(CUSTOMFIELD_EY),
                                     CustomField::defaultFunction),
                  Config::getPointer(SOVOL_CONFIG_KEY(CUSTOMFIELD_EZ),
                                     CustomField::defaultFunction),
                  Config::getPointer(SOVOL_CONFIG_KEY(CUSTOMFIELD_BX),
                                     CustomField::defaultFunction),
                  Config::getPointer(SOVOL_CONFIG_KEY(CUSTOMFIELD_BY),
                                     CustomField::defaultFunction),
                  Config::getPointer(SOVOL_CONFIG_KEY(CUSTOMFIELD_BZ),
                                     CustomField::defaultFunction)) {}

CustomField::CustomField(C_FUNC_P(_ex), C_FUNC_P(_ey), C_FUNC_P(_ez),
                         C_FUNC_P(_bx), C_FUNC_P(_by), C_FUNC_P(_bz))
    : ex(_ex), ey(_ey), ez(_ez), bx(_bx), by(_by), bz(_bz){};

EMField CustomField::operator()(double x, double y, double z, double t) const {
    return EMField{
        Vector3<double>(ex(x, y, z, t), ey(x, y, z, t), ez(x, y, z, t)),
        Vector3<double>(bx(x, y, z, t), by(x, y, z, t), bz(x, y, z, t))};
};
