#include "resultat.h"

#include <map>

Resultat::Resultat(int unid, Pengar pengar)
    : _unid(unid)
    , _pengar(pengar) {
}

int Resultat::getUnid() const {
    return _unid;
}

Pengar Resultat::getPengar() const {
    return _pengar;
}

std::vector<Resultat> calcResultat(const std::vector<const BollDoc::Verifikat*>& verifikat) {
    std::map<int, Pengar> kontopengar;
    for (auto& v : verifikat) {
        for (auto& r : v->getRader()) {
            if (!r.getStruken()) {
                kontopengar[r.getKonto()] += r.getPengar();
            }
        }
    }

    std::vector<Resultat> resultat;
    for (auto& kv : kontopengar) {
        resultat.emplace_back(kv.first, kv.second);
    }
    return resultat;
}
