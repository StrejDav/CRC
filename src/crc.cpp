#include"crc.h"
#include<string>
#include<stdexcept>

/*
* Zakoduje kod 'codeword' generujicim polynonem 'generatingPolynomial'
*
* Vstup:
*   codeword    - kodove slovo
*   generatingPolynomial    - generujici polynom
* 
* Vystup:
*   std::string - kodove slovo se zabezpecenim   
*/
std::string CRC::Encode(const std::string &codeword, const Polynomial &generatingPolynomial)
{
    const size_t r = generatingPolynomial.size() - 1;
    const size_t n = codeword.length() + r;

    Polynomial codewordVec = this->ConvertToVector(codeword);
    codewordVec.resize(n, '0');
    
    Polynomial coded = this->DividePolynomials(codewordVec, generatingPolynomial);
    coded.insert(coded.begin(), codewordVec.begin(), codewordVec.end() - r);

    return std::string(coded.begin(), coded.end());
}

/*
* Dekoduje kod 'encoded' zabezpecene generujicim polynonem 'generatingPolynomial'
*
* Vstup:
*   encoded    - zabezpeceny kod
*   generatingPolynomial    - generujici polynom
* 
* Vystup:
*   std::tuple<std::string, bool, size_t>
*       std::string - opraveny zabezpeceny kod
*       bool    - 'true' pokud kod obsahuje error, jinak 'false'
*       size_t  - pozice erroru
*/
std::tuple<std::string, bool, size_t> CRC::Decode(const std::string &encoded, const Polynomial &generatingPolynomial)
{
    const size_t r = generatingPolynomial.size() - 1;
    const size_t n = encoded.length();

    bool fixableError = pow(2, r) - 1 == n;
    size_t errorPosition = 0;

    Polynomial encodedVec = this->ConvertToVector(encoded);
    Polynomial errorVec = this->DividePolynomials(encodedVec, generatingPolynomial);

    bool error = !CheckIfOnlyZeroes(errorVec);

    if (error)
    {
        if (fixableError)
        {
            Polynomial poisitionTest(1, '1');

            while (errorPosition < n)
            {
                Polynomial temp = this->DividePolynomials(poisitionTest, generatingPolynomial);
                if (temp == errorVec) break;
                poisitionTest.insert(poisitionTest.end(), '0');
                errorPosition++;
            }
            if (errorPosition == n)
                throw std::runtime_error("Received code contains an unfixable error");
            this->RepairError(encodedVec, errorPosition);
        }
        else
            throw std::runtime_error("Received code contains an unfixable error");
    }

    return std::tuple<std::string, bool, size_t>{std::string(encodedVec.begin(), encodedVec.end()), error, errorPosition};
}

/*
 * Funkce najde vsechny mozne generujici polynomy g(z) na zaklade zadane delky celkove zpravy 'n' a stupne generujiciho polynomu 'r'
 * 
 * Vstup:
 *   n  - delka cele zpravy vcetne zabecpeni
 *   r  - stupen generujiciho polynomu
 * 
 * Vystup
 *   Polynomial - vektor reprezentujici generujici polynom
 */
std::vector<Polynomial> CRC::FindGeneratingPolynomials(const size_t &n, const size_t &r)
{
    std::vector<Polynomial> retVec;
    Polynomial possiblePoly(2, '1');

    Polynomial zn(n + 1, '0');
    zn[0] = '1';
    zn[zn.size() - 1] = '1';

    for (size_t i = 0; i < pow(2, r - 1); i++)
    {
        this->MakeMiddleCoefs(possiblePoly, i, r + 1);
        Polynomial temp = this->DividePolynomials(zn, possiblePoly);

        if (std::all_of(temp.begin(), temp.end(), [](unsigned char x){ return x == '0'; }))
        {
            retVec.push_back(possiblePoly);
        }
        possiblePoly.erase(possiblePoly.begin() + 1, possiblePoly.end() - 1);
    }
    
    this->generatingPolynomials.push_back({n, r, retVec});

    return retVec;
}

/**
 * Prevede textovy retezec na vektor tak, ze jeden znak v retezci se stane jednim prvkem ve vektoru
 * 
 * Vstup:
 *   str    - textovy retezec
 * 
 * Vystup:
 *   Polynomial - textovy retezec prevedeny na vektor
 */
Polynomial CRC::ConvertToVector(const std::string &str)
{
    Polynomial retVec;

    for (const auto &i: str) retVec.push_back(i);

    return retVec;
}

/**
 * Provede modulo 2 deleni polynomu a navrati zbytek
 * 
 * Vstup: 
 *   nom    - citatel
 *   denom  - jmenovatel
 * 
 * Vystup
 *   Polynomial - zbytek po deleni
 */
Polynomial CRC::DividePolynomials(Polynomial nom, const Polynomial &denom)
{
    size_t position = std::distance(nom.begin(), std::find(nom.begin(), nom.end(), '1'));
    //nom.erase(nom.begin(), std::find(nom.begin(), nom.end(), '1'));
    while (nom.size() - position  >= denom.size())
    {
        for (size_t i = position; i < position + denom.size(); i++)
            nom[i] = XOR(nom[i], denom[i - position]);

        position = std::distance(nom.begin(), std::find(nom.begin(), nom.end(), '1'));
        //nom.erase(nom.begin(), std::find(nom.begin(), nom.end(), '1'));
    }
/*
    while (nom.size() != denom.size() - 1)
        nom.insert(nom.begin(), '0');*/

    return Polynomial(nom.end() - denom.size() + 1, nom.end());
}

/**
 * Vygeneruje 'i'-tou kombinaci bitu o delce 'size'
 * 
 * Vstup:
 *   possiblePoly   -Mozny generujici polynom
 *   i  -i-ta kombinace (decimalni cislo i bude prevedeno na binarni cislo)
 *   size   -velikost polynomu (r + 1)
 */
const void CRC::MakeMiddleCoefs(Polynomial &possiblePoly, size_t i, const size_t &size)
{
    while (possiblePoly.size() < size)
    {
        possiblePoly.insert(possiblePoly.begin() + 1, i % 2 + '0');
        i = i / 2;
    }
}

/**
 * Zkontroluje, zda-li pro pro danou kombinaci delky kodoveho slova a stupne generujiciho polynomu existuji
 * generujici polynomy. Pokud ne, zavola funkci FindGeneratingPolynomial pro jejich vytvoreni.
 * 
 * Vstup:
 *   k  - delka kodoveho slova
 *   r  - stupen generujiciho polynomu
 * 
 * Vystup:
 *   std::vector<Polynomial>    - vektor obsahujici vsechny nalezene generujici polynomy
 */
std::vector<Polynomial> CRC::ReturnGeneratingPolynomials(const size_t &k, const size_t &r)
{
    std::vector<Polynomial> retVec;
    const size_t n = k + r;

    for (const auto &i: this->generatingPolynomials)
    {
        if (i.n == n && i.r == r)
        {
            if (i.polynomials.size() == 0)
                throw std::runtime_error("For a given combination of codeword length and degree of generating polynomial, no generating polynomial has been found");
            retVec = i.polynomials;
        }
    }

    if (retVec.size() == 0)
        retVec = this->FindGeneratingPolynomials(n, r);

    return retVec.size() == 0 ? throw std::runtime_error("For a given combination of codeword length and degree of generating polynomial, no generating polynomial has been found") : retVec;
}

/**
 * Prevede vektor reprezentujici polynom na matematicky zapis polynomu
 * 
 * Vstup:
 *   Polynomial - polynom
 * 
 * Vystup:
 *   std::string    - matematicky zapis polynomu
 */
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

    if (polynomial[len - 1] == '1')
        retStr.append("1");
    else retStr.erase(retStr.end() - 2, retStr.end());

    return retStr;
}

/**
 * Pro danou delku kodoveho slova nalezne Hamminguv kod a navrati stupen generujiciho polynomu
 * 
 * Vstup:
 *   k  - delka kodoveho slova
 * 
 * Vystup:
 *   size_t - stupen generujiciho polynomu
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
            throw std::runtime_error("Invalid codeword length for generating Hamming code");
        r++;
    }
}

/**
 * Opravi chybu v 'decodedPolynomial' na pozici 'position'
 * 
 * Vstup:
 *   decodedPolynomial  - polynom, obsahujici chybu
 *   position   - pozice chyby
 */
const void CRC::RepairError(Polynomial &decodedPolynomial, const size_t &position)
{
    unsigned char *repairElement = &decodedPolynomial[decodedPolynomial.size() - position - 1];
    *repairElement = *repairElement == '1' ? '0' : '1';
}
