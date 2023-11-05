Moms
====

## Momsredovisning
|Konto|Beskrivning                                   |Saldo  |
|-----|----------------------------------------------|-------|
|2611 |Utgående moms på försäljning inom Sverige 25% |7634,25|
|2641 |Debiterad ingående moms                       |-272,96|
|2650 |Redovisningskonto för moms                    |-7362  |
|3740 |Öres- och kronutjämning                       |0,71   |

## Moms betalning
|Konto|Beskrivning                |Saldo |
|-----|---------------------------|------|
|1630 |Skattekonto                |-32484|
|2650 |Redovisningskonto för moms |32484 |

## Momsrapport

### Momspliktig försäljning
|Fält |Beskrivning                                                   |Mappade konton   |
|-----|--------------------------------------------------------------|-----------------|
|05   |Momspliktig försäljning som inte ingår i ruta 06, 07 eller 08 |3000-3003, 3011-3013, 3106, 3730, 3731, 3732, 3920, 3921, 3922, 3925, 3950|
|06   |Momspliktiga uttag                                            |3401, 3402, 3403 |
|07   |Beskattningsunderlag vid vinstmarginalbeskattning             |3200, 3211       |
|08   |Hyresinkomster vid frivillig skattskyldighet                  |3913, 3914       |
|10   |Utgående moms 25%                                             |2610, 2611, 2612, 2613, 2616|
|11   |Utgående moms 12%                                             |2620, 2621, 2622, 2623, 2626|
|12   |Utgående moms 6%                                              |2630, 2631, 2632, 2633, 2636|

### Momspliktliga inköp vid omvänd skatteskyldighet
|Fält |Beskrivning                                                   |Mappade konton   |
|-----|--------------------------------------------------------------|-----------------|
|20   |Inköp av varor från ett annat EU-land                         |4515, 4516, 4517 |
|21   |Inköp av tjänster från ett annat EU-land enligt huvudregeln   |4535, 4536, 4537 |
|22   |Inköp av tjänster från ett land utanför EU                    |4531, 4532, 4533 |
|23   |Inköp av varor i Sverige                                      |4415, 4416, 4417 |
|24   |Övriga inköpt av tjänster                                     |4425, 4426, 4427 |
|30   |Utgående moms 25%                                             |2614, 2617       |
|31   |Utgående moms 12%                                             |2624, 2627       |
|32   |Utgående moms 6%                                              |2634, 2637       |

### Import
|Fält |Beskrivning                                                   |Mappade konton   |
|-----|--------------------------------------------------------------|-----------------|
|50   |Beskattningsunderlag vid import                               |4545, 4546, 4547 |
|60   |Utgående moms 25%                                             |2615             |
|61   |Utgående moms 12%                                             |2625             |
|62   |Utgående moms 6%                                              |2635             |

### Försäljning m.m som är undantagen från moms
|Fält |Beskrivning                                                   |Mappade konton   |
|-----|--------------------------------------------------------------|-----------------|
|35   |Försäljning av varor till ett annat EU-land                   |3108 |
|36   |Försäljning av varor utanför EU                               |3105 |
|37   |Mellanmans inköp av varor vid trepartshandel                  |4512 |
|38   |Mellanmans försäljning av varor vid trepartshandel            |3107 |
|39   |Försäljning av tjänster till näringsidkare i annat EU-land enligt huvudregeln|3308 |
|40   |Övrig försäljning av tjänster omsatta utanför Sverige         |3305 |
|41   |Försäljning när köparen är skattskyldig i Sverige             |3231 |
|42   |Övrig försäljning m.m                                         |3004, 3014, 3089, 3404, 3911, 3912, 3980, 3981, 3985, 3987-3999 |

### Moms att dra av
|Fält |Beskrivning                |Mappade konton |
|-----|---------------------------|---------------|
|48   |Ingående moms att dra av   |2641           |

## Mappning Fält -> SKV XML-fält
|Fält |SKV XML |
|-----|--------|
|05   |ForsMomsEjAnnan?,
|06   |UttagMoms?,
|07   |UlagMargbesk?,
|08   |HyrinkomstFriv?,
|20   |InkopVaruAnnatEg?,
|21   |InkopTjanstAnnatEg?,
|22   |InkopTjanstUtomEg?,
|23   |InkopVaruSverige?,
|24   |InkopTjanstSverige?,
|50   |MomsUlagImport?,
|35   |ForsVaruAnnatEg?,
|36   |ForsVaruUtomEg?,
|37   |InkopVaruMellan3p?,
|38   |ForsVaruMellan3p?,
|39   |ForsTjSkskAnnatEg?,
|40   |ForsTjOvrUtomEg?,
|41   |ForsKopareSkskSverige?,
|42   |ForsOvrigt?,
|10   |MomsUtgHog?,
|11   |MomsUtgMedel?,
|12   |MomsUtgLag?,
|30   |MomsInkopUtgHog?,
|31   |MomsInkopUtgMedel?,
|32   |MomsInkopUtgLag?,
|60   |MomsImportUtgHog?,
|61   |MomsImportUtgMedel?,
|62   |MomsImportUtgLag?,
|48   |MomsIngAvdr?,
|49   |MomsBetala?,
TextUpplysningMoms?