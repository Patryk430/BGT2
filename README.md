# (Pa)prasta Blokų Grandinė
  Suparastintas Blockchain'as sukurtas naudojant pagerintą mano [_Cezario Hash'as_](https://github.com/Patryk430/BGT) maišos funkcijos versiją. 

> "Bet kas gali gerai padaryti, bet tik aš galiu padaryti taip kaip aš..."
  ### Trumpai apie maišos funkcija
* Deterministinė
* Neefektyvi
* Atspari kolizijoms atmetus testų, parodžiusiu jos neatsparumą kolizijoms, rezultatus.
         
_Kuo skiriasi nuo pradinės versijos?_ Įdiegtas paralelizmas ir kodo optimizacija.
    
## Kaip veikia?
Programa:
1. Sugeneruoja `User` tipo klasės vektorių `Users`.
2. Pagal jį sugeneruoja `Transaction` klasės vektorių `Transactions`.
3. Formuoja `Block` klasės bloką iš _100_ atsitiktinių transakcijų.
4. 'Kasa' bloką hash'uojant pagrindinius jo antraštės elementus.
5. Prideda bloką prie grandinės ir pagal jį atnaujina `Users` balansus.
### v0.2 versijoje:
* Paraleliškai kasami 5 blokai nauduojant `std::thread` funkciją.
* Bloko header'yje vietoje visų transakcijų hash'o implementuotas `merkle_root_hash`.
* Implementuota transakcijos verifikacija pagal _Merkle Tree_.

## Įspėjimas: programoje buvo panauduotas <ins>***Vibe Coding***</ins> metodas
Ponas _Žė-pė-te_ padėjo su:
* Hash'avimo optimizacija.
* Merkle Tree implementacija.
* Kasimo paralelizavimu.
