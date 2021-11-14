#include"crc.h"

/**
 * @brief Zakoduje zadanou zpravu se zabecpenim zadaneho stupne
 * 
 * @param codeword Zprava k zakodovani
 * @param genDeg Stupen generujiciho polynomu
 * @return std::string Zakodovana zprava
 */
std::string CRC::Encode(const std::string &codeword, const size_t &genDeg)
{
    CRC crc;

    const size_t r = genDeg;
    const size_t n = codeword.length() + r;

    BinVec codewordVec = crc.ConvertToVector(codeword);
    codewordVec.resize(n, '0');

    BinVec genPoly = crc.FindGeneratingPoly(n, r);
    BinVec coded = crc.DividePolys(codewordVec, genPoly);
    coded.insert(coded.begin(), codewordVec.begin(), codewordVec.end() - r);
    return std::string(coded.begin(), coded.end());
}

std::string CRC::Decode(const std::string &encoded, const size_t &genDeg)
{
    CRC crc;

    const size_t r = genDeg;
    const size_t n = encoded.length();

    //const std::string codeword = encoded.substr(0, encoded.size() - r - 1);

    BinVec encodedVec = crc.ConvertToVector(encoded);
    BinVec genPoly = crc.FindGeneratingPoly(n, r);
    BinVec rem = crc.DividePolys(encodedVec, genPoly);
    //coded.insert(coded.begin(), codewordVec.begin(), codewordVec.end());
    //return std::string(coded.begin(), coded.end());
}

/**
 * @brief Funkce najde vsechny mozne generujici polynomy g(z) na zaklade zadane delky celkove zpravy 'n' a stupne generujiciho polynomu 'r'
 * 
 * @param n delka cele zpravy (samotna zprava + generujici polynom)
 * @param r stupen generujiciho polynomu
 * @return BinVec vektor reprezentujici generujici polynom
 */
BinVec CRC::FindGeneratingPoly(const size_t &n, const size_t &r)
{
    BinVec possiblePoly(2, '1');

    BinVec zn(n + 1, '0');
    zn[0] = '1';
    zn[zn.size() - 1] = '1';

    for (size_t i = 0; i < pow(2, r - 1); i++)
    {
        CRC::MakeMiddleCoefs(possiblePoly, i, r + 1);
        BinVec temp = CRC::DividePolys(zn, possiblePoly);

        if (std::all_of(temp.begin(), temp.end(), [](unsigned char x){ return x == '0'; }))
            return possiblePoly;

        possiblePoly.erase(possiblePoly.begin() + 1, possiblePoly.end() - 1);
    }

    throw std::runtime_error("Nebyl nalezen generujici polynom");
}

/**
 * @brief Prevede textovy retezec na vektor tak, ze jeden znak v retezci se stane jednim prvkem ve vektoru
 * 
 * @param str textovy retezec
 * @return BinVec textovy retezec prevedeny na vektor
 */
BinVec CRC::ConvertToVector(const std::string &str)
{
    BinVec retVec;

    for (const auto &i: str) retVec.push_back(i);

    return retVec;
}

/**
 * @brief Provede deleni polynomu nad telesem GF(2) a navrati zbytek
 * 
 * @param nom citatel
 * @param denom jmenovatel
 * @return BinVec zbytek po deleni
 */
BinVec CRC::DividePolys(BinVec nom, const BinVec &denom)
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
const void CRC::MakeMiddleCoefs(BinVec &possiblePoly, size_t i, const size_t &size)
{
    while (possiblePoly.size() < size)
    {
        possiblePoly.insert(possiblePoly.begin() + 1, i % 2 + '0');
        i = i / 2;
    }
}
