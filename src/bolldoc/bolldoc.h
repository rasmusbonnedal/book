#pragma once

#include <istream>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "date.h"
#include "pengar.h"

class BollDoc {
   public:
    class Konto {
       public:
        Konto(int unid, std::string text, int typ, std::string normalt = "", std::string tagg = "");
        Konto(const Konto&) = default;
        Konto(Konto&&) = default;
        Konto& operator=(const Konto&) = default;
        Konto& operator=(Konto&&) = default;

        bool operator==(const Konto& other) const;

        int getUnid() const;

        const std::string& getText() const;

        std::string& getText();

        // 1: Balanskonto
        // 3: Resultatkonto
        int getTyp() const;

        int& getTyp();

        const std::string& getNormalt() const;

        std::string& getNormalt();

        const std::string& getTagg() const;

        std::string& getTagg();

        int getSRU() const;
        
        int& getSRU();

       private:
        int _unid;
        std::string _text;
        int _typ;
        std::string _normalt;
        std::string _tagg;
        int _sru;
    };

    class Rad {
       public:
        Rad(Date bokdatum, int konto, Pengar pengar, std::optional<Date> struken = std::nullopt);
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
        friend class BollDoc;

       public:
        Verifikat(int unid, std::string text, Date transdatum);
        Verifikat(const Verifikat&) = default;
        Verifikat(Verifikat&&) = default;
        Verifikat& operator=(const Verifikat&) = default;
        Verifikat& operator=(Verifikat&&) = default;

        bool operator==(const Verifikat& other) const;

        int getUnid() const;

        const std::string& getText() const;

        std::string& getText();

        void setText(const std::string& text);

        const Date& getTransdatum() const;

        void setTransdatum(const Date& date);

        void addRad(Rad&& rad);

        const Rad& getRad(int i) const;

        const std::vector<Rad>& getRader() const;

        bool getOmslutning(Pengar& omslutning) const;

       private:
        void update(const std::vector<Rad>& rader);

        int _unid;
        std::string _text;
        Date _transdatum;
        std::vector<Rad> _rader;
    };

    BollDoc(int version, std::string firma, std::string orgnummer, int bokforingsar, std::string valuta, bool avslutat);

    bool operator==(const BollDoc& other) const;

    int getVersion() const;

    const std::string& getFirma() const;

    const std::string& getOrgnummer() const;

    int getBokforingsar() const;

    const std::string& getValuta() const;

    bool getAvslutat() const;

    const Konto& getKonto(int unid) const;

    const std::map<int, Konto>& getKontoPlan() const;

    const std::string& getKontoPlanTyp() const;

    const std::vector<std::pair<std::string, std::string>>& getKontoGrupper() const;

    int getNextVerifikatId() const;

    const Verifikat& getVerifikat(int unid) const;

    const std::vector<Verifikat>& getVerifikationer() const;

    std::vector<const Verifikat*> getVerifikatRange(const Date& start, const Date& end) const;

    std::vector<const Verifikat*> getVerifikatRange(const int unidStart, const int unidEnd) const;

    BollDoc newYear() const;

    bool isDirty() const;

    void clearDirty();

    uint64_t getRevision() const;

    // Mutating operations
    void addOrUpdateKonto(Konto&& konto);

    void setKontoPlanTyp(const std::string& kptyp);

    void addKontoGrupp(const std::pair<std::string, std::string>& kontogrupp);

    void addVerifikat(Verifikat&& verifikat);

    void updateVerifikat(int unid, const std::vector<Rad>& rader);

    void setVerifikatTransdatum(int unid, const Date& date);

    void setVerifikatText(int unid, const std::string& text);

    void setMutated();

   private:
    Verifikat& getVerifikatMut(int unid);

    void checkYear(const Date& date) const;

    void checkVerifikatId(int unid) const;

    int _version;
    std::string _firma;
    std::string _orgnummer;
    int _bokforingsar;
    std::string _valuta;
    bool _avslutat;
    std::map<int, Konto> _kontoplan;
    std::vector<Verifikat> _verifikat;
    std::string _kptyp;
    std::vector<std::pair<std::string, std::string>> _kontogrupper;
    bool _dirty;
    uint64_t _revision;
};

std::ostream& operator<<(std::ostream& stream, const BollDoc::Rad& rad);
