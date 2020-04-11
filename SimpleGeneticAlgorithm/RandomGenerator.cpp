#include "RandomGenerator.h"

RandomGenerator::RandomGenerator()
{
  this->generator.seed(this->randomDevice());
}

RandomGenerator::~RandomGenerator()
{

}

RandomGenerator& RandomGenerator::getInstance(void)
{
   static RandomGenerator randomGeneratorInstance;
   return randomGeneratorInstance;
}

double RandomGenerator::getDouble(double min, double max)
{
   return std::uniform_real_distribution<>(min, max)(this->generator);
}

int RandomGenerator::getInteger(int min, int max)
{
   return std::uniform_int_distribution<>(min, max)(this->generator);
}