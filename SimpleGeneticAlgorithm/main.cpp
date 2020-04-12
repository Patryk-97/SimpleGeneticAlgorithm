#include "Populacja.h"

#include <memory>
#include <iostream>

int main(void)
{
   std::shared_ptr<Osobnik<7>> najlepszyOsobnik = nullptr;
   auto funkcjaPrzystosowania = [](const size_t& fenotyp) {return 2 * (fenotyp * fenotyp + 1); };
   std::unique_ptr<Populacja<7, decltype(funkcjaPrzystosowania)>> populacja =
      std::make_unique<Populacja<7, decltype(funkcjaPrzystosowania)>>(8, 100, 0.8, 0.1, funkcjaPrzystosowania);

   populacja->inicjacja();
   populacja->iteracjaPokolen();
   najlepszyOsobnik = populacja->zwrocenieNajlepszegoOsobnika();

   /*std::cout << "Najlepszy osobnik: " << najlepszyOsobnik->fenotyp << "\n";
   std::cout << najlepszyOsobnik->genotyp << "\n";*/

   return 0;
}