#include"crc.h"
#include<string>
#include<stdexcept>

/**
 * @brief Zakoduje zadanou zpravu se zabecpenim zadaneho stupne
 * 
 * @param codeword Zprava k zakodovani
 * @param genDeg Stupen generujiciho polynomu
 * @return std::string Zakodovana zprava
 */
std::string CRC::Encode(const std::string &codeword, const Polynomial &generatingPolynomial)
{
    const size_t r = generatingPolynomial.size() - 1;
    const size_t n = codeword.length() + r;

    Polynomial codewordVec = CRC::ConvertToVector(codeword);
    codewordVec.resize(n, '0');
    
    Polynomial coded = CRC::DividePolynomials(codewordVec, generatingPolynomial);
    coded.insert(coded.begin(), codewordVec.begin(), codewordVec.end() - r);

    return std::string(coded.begin(), coded.end());
}

std::pair<Polynomial, size_t> CRC::Decode(const std::string &encoded, const Polynomial &generatingPolynomial)
{
    const size_t r = encoded.size() - generatingPolynomial.size() - 1;
    const size_t n = encoded.length();

    size_t errorPos = 0;

    Polynomial encodedVec = CRC::ConvertToVector(encoded);
    Polynomial decoded(encodedVec.begin(), encodedVec.end() - generatingPolynomial.size() - 1);
    
    Polynomial error = CRC::DividePolynomials(encodedVec, generatingPolynomial);
    error.erase(error.begin(), std::find(error.begin(), error.end(), '1'));

    if (error.size() != 0)
    {
        Polynomial temp = CRC::DividePolynomials(error, generatingPolynomial);
        if (!std::all_of(temp.begin(), temp.end(), [](unsigned char x){ return x == '0'; }))
            errorPos = -1;
        else
        {
            errorPos = CRC::BinToDec(error);
            CRC::RepairError(decoded, errorPos);
        }
    }

    return std::pair<Polynomial, size_t>{decoded, errorPos};
}

/**
 * @brief Funkce najde vsechny mozne generujici polynomy g(z) na zaklade zadane delky celkove zpravy 'n' a stupne generujiciho polynomu 'r'
 * 
 * @param n delka cele zpravy (samotna zprava + generujici polynom)
 * @param r stupen generujiciho polynomu
 * @return Polynomial vektor reprezentujici generujici polynom
 */
std::vector<Polynomial> CRC::FindGeneratingPolynomials(const size_t &n, const size_t &r)
{
    std::vector<Polynomial> retVec;
    bool undefined = true;
    Polynomial possiblePoly(2, '1');

    Polynomial zn(n + 1, '0');
    zn[0] = '1';
    zn[zn.size() - 1] = '1';

    for (size_t i = 0; i < pow(2, r - 1); i++)
    {
        CRC::MakeMiddleCoefs(possiblePoly, i, r + 1);
        Polynomial temp = CRC::DividePolynomials(zn, possiblePoly);

        if (std::all_of(temp.begin(), temp.end(), [](unsigned char x){ return x == '0'; }))
        {
            CRC::generatingPolynomials.push_back({n, r, possiblePoly});
            retVec.push_back(possiblePoly);
            undefined = false;
        }
        possiblePoly.erase(possiblePoly.begin() + 1, possiblePoly.end() - 1);
    }

    if (undefined) throw std::runtime_error("Pro zadanou kombinaci delky zpravy a stupne generujiciho polynomu nebyl nalezen generujici polynom");

    return retVec;
}

/**
 * @brief Prevede textovy retezec na vektor tak, ze jeden znak v retezci se stane jednim prvkem ve vektoru
 * 
 * @param str textovy retezec
 * @return Polynomial textovy retezec prevedeny na vektor
 */
Polynomial CRC::ConvertToVector(const std::string &str)
{
    Polynomial retVec;

    for (const auto &i: str) retVec.push_back(i);

    return retVec;
}

/**
 * @brief Provede deleni polynomu nad telesem GF(2) a navrati zbytek
 * 
 * @param nom citatel
 * @param denom jmenovatel
 * @return Polynomial zbytek po deleni
 */
Polynomial CRC::DividePolynomials(Polynomial nom, const Polynomial &denom)
{
    while (nom.size() >= denom.size())
    {
        for (size_t i = 0; i < denom.size(); i++)
            nom[i] = XOR(nom[i], denom[i]);
        
        nom.erase(nom.begin(), std::find(nom.begin(), nom.end(), '1'));
    }

    while (nom.size() != denom.size() - 1)
        nom.insert(nom.begin(), '0');

    return nom;
}

/**
 * @brief Vygeneruje 'i'-tou kombinaci bitu o delce 'size'
 * 
 * @param possiblePoly Mozny generujici polynom
 * @param i i-ta kombinace (decimalni cislo i bude prevedeno na binarni cislo)
 * @param size velikost polynomu (r + 1)
 */
const void CRC::MakeMiddleCoefs(Polynomial &possiblePoly, size_t i, const size_t &size)
{
    while (possiblePoly.size() < size)
    {
        possiblePoly.insert(possiblePoly.begin() + 1, i % 2 + '0');
        i = i / 2;
    }
}

std::vector<Polynomial> CRC::ReturnGeneratingPolynomials(const size_t &n, const size_t &r)
{
    std::vector<Polynomial> retVec;

    for (const auto &i: CRC::generatingPolynomials)
    {
        if (i.n == n && i.r == r) retVec.push_back(i.polynomial);
    }

    if (retVec.size() == 0)
        retVec = CRC::FindGeneratingPolynomials(n, r);

    return retVec;
}

/* Unused for now
std::string CRC::PrettifyPolynomialVector(const Polynomial &polynomial)
{
    std::string retStr;
    const size_t len = polynomial.size();

    for (size_t i = 0; i < len - 2; i++)
    {
        if (polynomial[i] == '1')
            retStr.append("x^" + std::to_string(len - i - 1) + " + ");
    }

    if (polynomial[len - 2] == '1')
        retStr.append("x + ");
    retStr.append("1");

    return retStr;
}
*/

size_t CRC::FindHammingCode(const size_t &k)
{
    size_t r = 1;
    size_t n;
    while(true)
    {
        n = pow(2, r) - 1;
        if (n - r == k)
            return r;
        else if (n - r > k)
            throw std::runtime_error("Neplatna delka kodoveho slova pro vygenerovani Hammingova kodu");
        r++;
    }
}

size_t CRC::BinToDec(const Polynomial &errorBinaryPosition)
{
    size_t dec = 0;
    size_t len = errorBinaryPosition.size();

    for (size_t i = 0; i < len; i++)
    {
        dec += (errorBinaryPosition[i] - '0')*pow(2, len - i - 1);
    }

    return dec;
}

const void CRC::RepairError(Polynomial &decodedPolynomial, const size_t &position)
{
    unsigned char *repairElement = &decodedPolynomial[decodedPolynomial.size() - position - 1];
    *repairElement = *repairElement == '1' ? '0' : '1';
}