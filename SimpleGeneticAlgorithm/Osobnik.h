#pragma once

#ifndef __OSOBNIK_H__
#define __OSOBNIK_H__

#include "RandomGenerator.h"

#include <math.h>
#include <memory>

template <size_t iloscGenow>
struct Osobnik
{
   Osobnik()
   {
      memset(this->genotyp, '0', iloscGenow);
      this->ocena = 0;
      this->wskaznikPrzystosowania = 0.;
      RandomGenerator& randomGenerator = RandomGenerator::getInstance();
      for (size_t i = 0; i < iloscGenow; i++)
      {
         this->genotyp[i] = randomGenerator.getInteger(0, 1) == 1 ? '1' : '0';
      }
      this->obliczFenotyp();
   }

   Osobnik(const Osobnik<iloscGenow>*& osobnik)
   {
      if (this != osobnik)
      {
         this->fenotyp = osobnik->fenotyp;
         memcpy(this->genotyp, osobnik->genotyp, iloscGenow);
         this->ocena = osobnik->ocena;
         this->wskaznikPrzystosowania = osobnik->wskaznikPrzystosowania;
      }
   }

   Osobnik(const std::unique_ptr<Osobnik<iloscGenow>>& osobnik)
   {
      if (this != osobnik.get())
      {
         this->fenotyp = osobnik->fenotyp;
         memcpy(this->genotyp, osobnik->genotyp, iloscGenow);
         this->ocena = osobnik->ocena;
         this->wskaznikPrzystosowania = osobnik->wskaznikPrzystosowania;
      }
   }

   Osobnik& operator= (const Osobnik<iloscGenow>*& osobnik)
   {
      if (this != osobnik)
      {
         this->fenotyp = osobnik->fenotyp;
         memcpy(this->genotyp, osobnik->genotyp, iloscGenow);
         this->ocena = osobnik->ocena;
         this->wskaznikPrzystosowania = osobnik->wskaznikPrzystosowania;
      }
      return *this;
   }

   Osobnik& operator= (const std::unique_ptr<Osobnik<iloscGenow>>*& osobnik)
   {
      if (this != osobnik)
      {
         this->fenotyp = osobnik->fenotyp;
         memcpy(this->genotyp, osobnik->genotyp, iloscGenow);
         this->ocena = osobnik->ocena;
         this->wskaznikPrzystosowania = osobnik->wskaznikPrzystosowania;
      }
      return *this;
   }

   void obliczFenotyp(void)
   {
      this->fenotyp = 0;
      for (size_t i = 0; i < iloscGenow; i++)
      {
         if ('1' == this->genotyp[i])
         {
            this->fenotyp += static_cast<size_t>(pow(2, iloscGenow - 1 - i));
         }
      }
   }

   void ustawGen(size_t index, char allel)
   {
      if (index >= iloscGenow && allel != '0' && allel != '1')
      {
         return;
      }
      this->genotyp[index] = allel;
   }

   char pobierzGen(size_t index)
   {
      return this->genotyp[index];
   }

   char genotyp[iloscGenow];
   size_t fenotyp;
   size_t ocena;
   double wskaznikPrzystosowania;
};

#endif // __OSOBNIK_H__