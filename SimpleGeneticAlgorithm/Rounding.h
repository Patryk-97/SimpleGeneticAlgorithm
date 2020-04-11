#pragma once

#ifndef __ROUNDING_H__
#define __ROUNDING_H__

#include <stdint.h>
#include <math.h>

class Rounding
{
public:
   static double nearest(double value, size_t precision);
   static double up(double value, size_t precision);
   static double down(double value, size_t precision);
};

#endif // __ROUNDING_H__