#pragma once

#ifndef __RANDOM_GENERATOR_H__
#define __RANDOM_GENERATOR_H__

#include <random>

class RandomGenerator
{
private:
   RandomGenerator();
   RandomGenerator(const RandomGenerator& randomGenerator) = delete;

public:

   ~RandomGenerator();
   static RandomGenerator& getInstance(void);
   std::mt19937& getGenerator(void);
   double getDouble(double min, double max);
   int getInteger(int min, int max);

private:
   std::random_device randomDevice;
   std::mt19937 generator;
};

#endif // __RANDOM_GENERATOR_H__