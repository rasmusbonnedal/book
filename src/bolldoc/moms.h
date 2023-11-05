#pragma once

#include <map>
#include <vector>

#include "bolldoc.h"
#include "date.h"
#include "pengar.h"

using KontoMap = std::map<int, std::vector<int>>;
using FieldSaldo = std::map<int, Pengar>;
using Verifikat = std::map<int, Pengar>;
using FieldToSkv = std::map<int, std::string>;

// Month number [1, 12] to summarize for
FieldSaldo summarize_moms(const BollDoc& doc, int month, const KontoMap& konto_map, Verifikat& redovisning);

void sum_moms(FieldSaldo& field_saldo);

std::string gen_moms_eskd(const BollDoc& doc, int month, const FieldSaldo& field_saldo, const FieldToSkv& field_to_skv);
