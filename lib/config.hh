#ifndef _SOVOL_CONFIG_HH
#define _SOVOL_CONFIG_HH 1
// #define __EMSCRIPTEN__ 1
#ifdef __EMSCRIPTEN__
#include <emscripten/val.h>

#include "WasmFactoryHelper.hh"
#else
#include "Lua.hh"
#include "LuaFactoryHelper.hh"
#endif

// data source https://physics.nist.gov/cuu/Constants
#define CONST_PI 3.141592653589793238462643383279503  // pi
#define CONST_M0 9.1093837015e-31                     // electron mass; unit: kg
#define CONST_Q0 1.602176634e-19         // elementary charge; unit: C
#define CONST_C 299792458.0              // speed of light in vacuum; unit: m/s
#define CONST_C2 8.987551787368176e+16   // c^2
#define CONST_HBAR 1.054571817e-34       // reduced Planck constant; unit: J s
#define CONST_E_S 1.323285474948166e+18  // m_e**2 * c**3 / (hbar * q_0)
#define CONST_ALPHA \
  7.2973525693e-3  // fine-structure constant; q_0**2 / (2 * epsilon_0 * h * c)
#define CONST_TAU_C 1.2880886674083155e-21          // h_bar / (m_e * c**2)
#define CONST_SQRT3 1.73205080756887729352744634151 /* sqrt(3) */

#endif