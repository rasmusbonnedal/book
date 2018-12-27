#include <istream>
#include <map>
#include <optional>
#include <vector>

class BollDoc {
public:
    class Konto {
    public:
        Konto(int unid, std::string text, int typ, std::optional<std::string> normalt);
        Konto(Konto&&) = default;
        Konto& operator=(Konto&&) = default;

        int getUnid() const;

        const std::string getText() const;

        int getTyp() const;

        std::optional<std::string> getNormalt() const;

    private:
        int _unid;
        std::string _text;
        int _typ;
        std::optional<std::string> _normalt;
    };

    class Rad {
    public:
        Rad(std::string bokdatum, int konto, int64_t pengar);
        Rad(Rad&&) = default;
        Rad& operator=(Rad&&) = default;

        const std::string& getBokdatum() const;

        int getKonto() const;

        int64_t getPengar() const;

    private:
        std::string _bokdatum;
        int _konto;
        int64_t _pengar;
    };

    class Verifikat {
    public:
        Verifikat(int unid, std::string text, std::string transdatum);
        Verifikat(Verifikat&&) = default;
        Verifikat& operator=(Verifikat&&) = default;

        int getUnid() const;

        const std::string& getText() const;

        const std::string& getTransdatum() const;

        void addRad(Rad&& rad);

        const Rad& getRad(int i) const;

        const std::vector<Rad>& getRader() const;

    private:
        int _unid;
        std::string _text;
        std::string _transdatum;
        std::vector<Rad> _rader;
    };

    BollDoc(int version,
            std::string firma,
            std::string orgnummer,
            int bokforingsar,
            std::string valuta);

    int getVersion() const;

    const std::string& getFirma() const;

    const std::string& getOrgnummer() const;

    int getBokforingsar() const;

    const std::string& getValuta() const;

    void addKonto(Konto&& konto);

    const Konto& getKonto(int unid) const;

    void addVerifikat(Verifikat&& verifikat);

    const Verifikat& getVerifikat(int unid) const;

    const std::vector<Verifikat>& getVerifikat() const;

private:
    int _version;
    std::string _firma;
    std::string _orgnummer;
    int _bokforingsar;
    std::string _valuta;
    std::map<int, Konto> _kontoplan;
    std::vector<Verifikat> _verifikat;
};

BollDoc loadDocument(std::istream& input);
