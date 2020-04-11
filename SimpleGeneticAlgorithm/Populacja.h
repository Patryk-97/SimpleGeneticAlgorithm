#pragma once

#ifndef __POPULACJA_H__
#define __POPULACJA_H__

#include "Osobnik.h"
#include "Rounding.h"

#include <vector>
#include <stdint.h>
#include <memory>

template <size_t iloscGenow, typename TypFunkcjiPrzystosowania>
class Populacja
{
public:
   Populacja(uint8_t rozmiarPopulacji, uint8_t iloscPokolen, double prawdopodobienstwoKrzyzowania,
      double prawdopodobienstwoMutacji, TypFunkcjiPrzystosowania funkcjaPrzystosowania):
      rozmiarPopulacji(rozmiarPopulacji), iloscPokolen(iloscPokolen),
      PRAWDOPODOBIENSTWO_KRZYZOWANIA(prawdopodobienstwoKrzyzowania),
      PRAWDOPODOBIENSTWO_MUTACJI(prawdopodobienstwoMutacji),
      funkcjaPrzystosowania(funkcjaPrzystosowania)
   {
      this->koloRuletki.resize(this->rozmiarPopulacji);
      this->wektorSelekcji.resize(this->rozmiarPopulacji);
      this->najlepszyOsobnik = std::make_shared<Osobnik<iloscGenow>>();
   }

   void inicjacja(void)
   {
      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         populacja.push_back(std::make_unique<Osobnik<iloscGenow>>());
      }
   }

   void iteracjaPokolen(void)
   {
      // locals
      size_t sumaWartosciFunkcjiPrzystosowania = 0;

      this->ocenaPrzystosowania();
      for (size_t i = 0; i < this->iloscPokolen; i++)
      {
         sumaWartosciFunkcjiPrzystosowania = this->obliczSumeWartosciFunkcjiPrzystosowania();
         this->obliczWskaznikiPrzystosowan(sumaWartosciFunkcjiPrzystosowania);
         this->wypelnijKoloRuletki();
         this->wyznaczenieWektoraSelekcji();
         this->utworzenieNowejPopulacji();
         for (size_t i = 0; i < this->rozmiarPopulacji; i += 2)
         {
            this->krzyzowanie(this->populacja[i], this->populacja[i + 1]);
            this->mutacja(this->populacja[i]);
            this->mutacja(this->populacja[i + 1]);
         }
         this->ocenaPrzystosowania();
      }
      this->wyznaczenieNajlepszegoOsobnika();
   }

   std::shared_ptr<Osobnik<iloscGenow>> zwrocenieNajlepszegoOsobnika(void)
   {
      return this->najlepszyOsobnik;
   }

private:

   void ocenaPrzystosowania(void)
   {
      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         this->populacja[i]->ocena = this->funkcjaPrzystosowania(this->populacja[i]->fenotyp);
      }
   }

   bool krzyzowanie(std::unique_ptr<Osobnik<iloscGenow>>& rodzic1,
      std::unique_ptr<Osobnik<iloscGenow>>& rodzic2)
   {
      // locals
      bool rV = false;
      double p;
      uint8_t locus;
      std::unique_ptr<Osobnik<iloscGenow>> tymczasowyOsobnik =
         std::make_unique<Osobnik<iloscGenow>>(rodzic1);

      RandomGenerator& randomGenerator = RandomGenerator::getInstance();
      p = randomGenerator.getDouble(0., 1.);
      if (p <= this->PRAWDOPODOBIENSTWO_KRZYZOWANIA)
      {
         locus = randomGenerator.getInteger(0, iloscGenow - 1);

         for (size_t i = locus; i < iloscGenow; i++)
         {
            rodzic1->ustawGen(i, rodzic2->pobierzGen(i));
            rodzic2->ustawGen(i, tymczasowyOsobnik->pobierzGen(i));
         }

         rodzic1->obliczFenotyp();
         rodzic2->obliczFenotyp();
         rodzic1->ocena = 0;
         rodzic2->ocena = 0;

         rV = true;
      }
      return rV;
   }

   size_t mutacja(const std::unique_ptr<Osobnik<iloscGenow>>& osobnik)
   {
      // locals
      size_t counter = 0;
      double p;
      RandomGenerator& randomGenerator = RandomGenerator::getInstance();

      for (size_t i = 0; i < iloscGenow; i++)
      {
         p = randomGenerator.getDouble(0., 1.);
         if (p <= this->PRAWDOPODOBIENSTWO_MUTACJI)
         {
            osobnik->ustawGen(i, osobnik->pobierzGen(i) == '1' ? '0' : '1');
            ++counter;
         }
      }

      return counter;
   }

   size_t obliczSumeWartosciFunkcjiPrzystosowania(void)
   {
      // locals
      size_t sumaWartosciFunkcjiPrzystosowania = 0;

      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         sumaWartosciFunkcjiPrzystosowania += this->populacja[i]->ocena;
      }

      return sumaWartosciFunkcjiPrzystosowania;
   }

   void obliczWskaznikiPrzystosowan(size_t sumaWartosciFunkcjiPrzystosowania)
   {
      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         this->populacja[i]->wskaznikPrzystosowania =
            Rounding::nearest((double)this->populacja[i]->ocena /
                  sumaWartosciFunkcjiPrzystosowania, 2);
      }
   }

   void wypelnijKoloRuletki(void)
   {
      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         this->koloRuletki[i] = this->populacja[i]->wskaznikPrzystosowania;
      }
      std::sort(this->koloRuletki.begin(), this->koloRuletki.end());
      for (size_t i = 1; i < this->rozmiarPopulacji; i++)
      {
         this->koloRuletki[i] = this->koloRuletki[i - 1] + this->koloRuletki[i];
      }
   }

   void wyznaczenieWektoraSelekcji(void)
   {
      RandomGenerator& randomGenerator = RandomGenerator::getInstance();
      double p;
      std::fill(this->wektorSelekcji.begin(), this->wektorSelekcji.end(), 0);
      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         p = randomGenerator.getDouble(0, 1);
         for (size_t j = 0; j < this->rozmiarPopulacji; j++)
         {
            if (p <= this->koloRuletki[i])
            {
               ++this->wektorSelekcji[j];
               break;
            }
         }
      }
   }

   void utworzenieNowejPopulacji(void)
   {
      std::vector<std::unique_ptr<Osobnik<iloscGenow>>> nowaPopulacja(this->rozmiarPopulacji);
      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         for (size_t j = 0; j < this->wektorSelekcji[i]; j++)
         {
            nowaPopulacja.push_back(
               std::make_unique<Osobnik<iloscGenow>>(this->populacja[i]));
         }
      }
      this->populacja = std::move(nowaPopulacja);
   }

   void wyznaczenieNajlepszegoOsobnika(void)
   {
      *this->najlepszyOsobnik = *this->populacja[0];
      for (size_t i = 1; i < this->rozmiarPopulacji; i++)
      {
         if (this->populacja[i]->ocena > this->najlepszyOsobnik->ocena)
         {
            *this->najlepszyOsobnik = *this->populacja[i];
         }
      }
   }
   
private:
   uint8_t rozmiarPopulacji;
   uint8_t iloscPokolen;
   const double PRAWDOPODOBIENSTWO_KRZYZOWANIA;
   const double PRAWDOPODOBIENSTWO_MUTACJI;
   std::vector<std::unique_ptr<Osobnik<iloscGenow>>> populacja;
   TypFunkcjiPrzystosowania funkcjaPrzystosowania;
   std::vector<double> koloRuletki;
   std::vector<size_t> wektorSelekcji;
   std::shared_ptr<Osobnik<iloscGenow>> najlepszyOsobnik;
};

#endif // __POPULACJA_H__