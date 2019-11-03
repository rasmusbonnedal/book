#pragma once

#include "bolldoc.h"
#include "pengar.h"

class Resultat {
public:
    Resultat(int unid, Pengar pengar);

    int getUnid() const;

    Pengar getPengar() const;

private:
    int _unid;
    Pengar _pengar;
};

std::vector<Resultat>
calcResultat(const std::vector<const BollDoc::Verifikat*>& verifikat);
