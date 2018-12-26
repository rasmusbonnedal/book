#include <istream>
#include <map>
#include <optional>

class BollDoc {
public:
    class Konto {
    public:
        Konto(int unid, std::string text, int typ, std::optional<std::string> normalt);
        Konto(const Konto&) = default;
        Konto(Konto&&) = default;
        Konto& operator=(Konto&&) = default;
        Konto& operator=(const Konto&) = default;

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

private:
    int _version;
    std::string _firma;
    std::string _orgnummer;
    int _bokforingsar;
    std::string _valuta;
    std::map<int, Konto> _kontoplan;
};

BollDoc loadDocument(std::istream& input);
