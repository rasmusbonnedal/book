#pragma once

#include <istream>
#include <map>
#include <optional>
#include <vector>

#include "date.h"
#include "pengar.h"

class BollDoc {
public:
    class Konto {
    public:
        Konto(int unid, std::string text, int typ,
              std::optional<std::string> normalt = std::nullopt,
              std::optional<std::string> tagg = std::nullopt);
        Konto(Konto&&) = default;
        Konto& operator=(Konto&&) = default;

        bool operator==(const Konto& other) const;

        int getUnid() const;

        const std::string& getText() const;

        int getTyp() const;

        const std::optional<std::string>& getNormalt() const;

        const std::optional<std::string>& getTagg() const;

    private:
        int _unid;
        std::string _text;
        int _typ;
        std::optional<std::string> _normalt;
        std::optional<std::string> _tagg;
    };

    class Rad {
    public:
        Rad(Date bokdatum, int konto, Pengar pengar,
            std::optional<Date> struken = std::nullopt);
        Rad(const Rad&) = default;
        Rad(Rad&&) = default;
        Rad& operator=(const Rad&) = default;
        Rad& operator=(Rad&&) = default;

        bool operator==(const Rad& other) const;

        const Date& getBokdatum() const;

        int getKonto() const;

        Pengar getPengar() const;

        const std::optional<Date>& getStruken() const;

    private:
        Date _bokdatum;
        int _konto;
        Pengar _pengar;
        std::optional<Date> _struken;
    };

    class Verifikat {
    public:
        Verifikat(int unid, std::string text, Date transdatum);
        Verifikat(Verifikat&&) = default;
        Verifikat& operator=(Verifikat&&) = default;

        bool operator==(const Verifikat& other) const;

        int getUnid() const;

        const std::string& getText() const;

        const Date& getTransdatum() const;

        void addRad(Rad&& rad);

        void update(const std::vector<Rad>& rader);

        const Rad& getRad(int i) const;

        const std::vector<Rad>& getRader() const;

        bool getOmslutning(Pengar& omslutning) const;

    private:
        int _unid;
        std::string _text;
        Date _transdatum;
        std::vector<Rad> _rader;
    };

    BollDoc(int version, std::string firma, std::string orgnummer,
            int bokforingsar, std::string valuta, bool avslutat);

    bool operator==(const BollDoc& other) const;

    int getVersion() const;

    const std::string& getFirma() const;

    const std::string& getOrgnummer() const;

    int getBokforingsar() const;

    const std::string& getValuta() const;

    bool getAvslutat() const;

    void addKonto(Konto&& konto);

    const Konto& getKonto(int unid) const;

    const std::map<int, Konto>& getKontoPlan() const;

    void addVerifikat(Verifikat&& verifikat);

    void updateVerifikat(int unid, const std::vector<Rad>& rader);

    const Verifikat& getVerifikat(int unid) const;

    const std::vector<Verifikat>& getVerifikationer() const;

    std::vector<const Verifikat*> getVerifikatRange(const Date& start,
                                                    const Date& end) const;

    std::vector<const Verifikat*> getVerifikatRange(const int unidStart,
                                                    const int unidEnd) const;

private:
    int _version;
    std::string _firma;
    std::string _orgnummer;
    int _bokforingsar;
    std::string _valuta;
    bool _avslutat;
    std::map<int, Konto> _kontoplan;
    std::vector<Verifikat> _verifikat;
};

std::ostream& operator<<(std::ostream& stream, const BollDoc::Rad& rad);
