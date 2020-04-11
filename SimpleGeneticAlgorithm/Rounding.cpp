#include "Rounding.h"

double Rounding::nearest(double value, size_t precision)
{
   uint64_t appropriatePowerOfTen = (uint64_t)pow(10, precision);
   return ::round(value * appropriatePowerOfTen) / appropriatePowerOfTen;
}

double Rounding::up(double value, size_t precision)
{
   uint64_t appropriatePowerOfTen = (uint64_t)pow(10, precision);
   return ::ceil(value * appropriatePowerOfTen) / appropriatePowerOfTen;
}

double Rounding::down(double value, size_t precision)
{
   uint64_t appropriatePowerOfTen = (uint64_t)pow(10, precision);
   return ::floor(value * appropriatePowerOfTen) / appropriatePowerOfTen;
}