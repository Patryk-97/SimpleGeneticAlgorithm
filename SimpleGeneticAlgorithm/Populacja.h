#pragma once

#ifndef __POPULACJA_H__
#define __POPULACJA_H__

#define LOG_FILENAME "log.csv"

#include "Osobnik.h"
#include "Rounding.h"
#include "Logger.h"

#include <vector>
#include <stdint.h>
#include <memory>
#include <string>
#include <utility>
#include <math.h>

template <size_t iloscGenow, typename TypFunkcjiPrzystosowania>
class Populacja
{
public:

   enum class RodzajKolaRuletki
   {
      LINIOWE = 0,
      KWADRATOWE = 1,
      LOGARYTMICZNE = 2
   };

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
      //this->logger = std::make_unique<Logger>(LOG_FILENAME);
   }

   void inicjacja(void)
   {
      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         populacja.push_back(std::make_unique<Osobnik<iloscGenow>>());
      }
   }

   void ustawRodzajKolaRuletki(RodzajKolaRuletki rodzajKolaRuletki = RodzajKolaRuletki::LINIOWE)
   {
      this->rodzajKolaRuletki = rodzajKolaRuletki;
   }

   void iteracjaPokolen(void)
   {
      // locals
      size_t sumaWartosciFunkcjiPrzystosowania = 0;
      //double sredniFenotyp = 0.;

      this->ocenaPrzystosowania();
      for (size_t i = 0; i < this->iloscPokolen; i++)
      {
         sumaWartosciFunkcjiPrzystosowania = this->obliczSumeWartosciFunkcjiPrzystosowania();
         //sredniFenotyp = this->obliczSredniFenotyp();

         //this->logger->write(std::to_string(sumaWartosciFunkcjiPrzystosowania) + ",");
         //this->logger->writeln(std::to_string(sredniFenotyp) + ",");

         this->obliczWskaznikiPrzystosowan(sumaWartosciFunkcjiPrzystosowania);
         this->wypelnienieKolaRuletki();
         this->wyznaczenieWektoraSelekcji();
         this->utworzenieNowejPopulacji();
         for (size_t i = 0; i < this->rozmiarPopulacji; i += 2)
         {
            this->krzyzowanie(this->populacja[i], this->populacja[i + 1]);
         }
         this->mutacja();
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
         this->populacja[i]->przystosowanie =
            this->funkcjaPrzystosowania(this->populacja[i]->fenotyp);
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

         rodzic1->obliczFenotyp(); rodzic2->obliczFenotyp();
         rodzic1->przystosowanie = 0; rodzic2->przystosowanie = 0;
         rodzic1->wskaznikPrzystosowania = 0.; rodzic2->wskaznikPrzystosowania = 0.;

         rV = true;
      }
      return rV;
   }

   size_t mutacja(void)
   {
      // locals
      size_t counter = 0;
      double p;
      int ktoryGen;
      RandomGenerator& randomGenerator = RandomGenerator::getInstance();

      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         p = randomGenerator.getDouble(0., 1.);
         if (p <= this->PRAWDOPODOBIENSTWO_MUTACJI)
         {
            ktoryGen = randomGenerator.getInteger(0, iloscGenow - 1);
            this->populacja[i]->ustawGen(ktoryGen,
               this->populacja[i]->pobierzGen(ktoryGen) == '1' ? '0' : '1');
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
         sumaWartosciFunkcjiPrzystosowania += this->populacja[i]->przystosowanie;
      }

      return sumaWartosciFunkcjiPrzystosowania;
   }

   double obliczSredniFenotyp(void)
   {
      // locals
      double sredniFenotyp = 0.;

      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         sredniFenotyp += (double)this->populacja[i]->fenotyp;
      }

      sredniFenotyp /= this->rozmiarPopulacji;

      return sredniFenotyp;
   }

   void obliczWskaznikiPrzystosowan(size_t sumaWartosciFunkcjiPrzystosowania)
   {
      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         this->populacja[i]->wskaznikPrzystosowania =
            Rounding::nearest((double)this->populacja[i]->przystosowanie /
                  sumaWartosciFunkcjiPrzystosowania, 2);
      }
   }

   void wypelnienieKolaRuletki(void)
   {
      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         this->koloRuletki[i] = std::make_pair(i, this->populacja[i]->wskaznikPrzystosowania);
         if (this->rodzajKolaRuletki == RodzajKolaRuletki::KWADRATOWE)
         {
            this->koloRuletki[i].second = pow(this->koloRuletki[i].second * 100.0, 2);
         }
         else if (this->rodzajKolaRuletki == RodzajKolaRuletki::LOGARYTMICZNE)
         {
            this->koloRuletki[i].second = log2(this->koloRuletki[i].second * 100.0 + 1.);
         }
      }
      std::sort(this->koloRuletki.begin(), this->koloRuletki.end(),
         [](const std::pair<size_t, double>& pair1, const std::pair<size_t, double>& pair2)
         {
            return pair1.second < pair2.second;
         });
      for (size_t i = 1; i < this->rozmiarPopulacji; i++)
      {
         this->koloRuletki[i].second = koloRuletki[i - 1].second + this->koloRuletki[i].second;
      }
      if(this->rodzajKolaRuletki == RodzajKolaRuletki::LINIOWE)
      {
         this->koloRuletki[this->rozmiarPopulacji - 1].second = 1.;
      }
   }

   void wyznaczenieWektoraSelekcji(void)
   {
      RandomGenerator& randomGenerator = RandomGenerator::getInstance();
      double p, max = this->koloRuletki[this->rozmiarPopulacji - 1].second;
      std::fill(this->wektorSelekcji.begin(), this->wektorSelekcji.end(), 0);
      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         p = randomGenerator.getDouble(0, max);
         for (size_t j = 0; j < this->rozmiarPopulacji; j++)
         {
            if (p <= this->koloRuletki[j].second)
            {
               ++this->wektorSelekcji[this->koloRuletki[j].first];
               break;
            }
         }
      }
   }

   void utworzenieNowejPopulacji(void)
   {
      RandomGenerator& randomGenerator = RandomGenerator::getInstance();
      std::vector<std::unique_ptr<Osobnik<iloscGenow>>> nowaPopulacja;
      for (size_t i = 0; i < this->rozmiarPopulacji; i++)
      {
         for (size_t j = 0; j < this->wektorSelekcji[i]; j++)
         {
            nowaPopulacja.push_back(
               std::make_unique<Osobnik<iloscGenow>>(this->populacja[i]));
         }
      }
      this->populacja = std::move(nowaPopulacja);
      std::shuffle(this->populacja.begin(), this->populacja.end(), randomGenerator.getGenerator());
   }

   void wyznaczenieNajlepszegoOsobnika(void)
   {
      *this->najlepszyOsobnik = *this->populacja[0];
      for (size_t i = 1; i < this->rozmiarPopulacji; i++)
      {
         if (this->populacja[i]->przystosowanie > this->najlepszyOsobnik->przystosowanie)
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
   std::vector<std::pair<size_t, double>> koloRuletki;
   std::vector<size_t> wektorSelekcji;
   std::shared_ptr<Osobnik<iloscGenow>> najlepszyOsobnik;
   //std::unique_ptr<Logger> logger;
   RodzajKolaRuletki rodzajKolaRuletki;
};

#endif // __POPULACJA_H__