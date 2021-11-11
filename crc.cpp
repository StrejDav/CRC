#include<math.h>
#include<algorithm>
#include "crc.h"

#include<iostream>

std::string CRC::Encode(const std::string &message, const size_t &genDeg)
{
    CRC crc;

    const size_t r = genDeg;
    const size_t k = message.length();
    const size_t n = k + r;

    std::vector<unsigned char> messageVec = crc.ConvertToVector(message);
    std::vector<unsigned char> genPoly = crc.FindGeneratingPoly(n, r);
    return "";
}

/**
 * @brief Funkce najde vsechny mozne generujici polynomy g(z) na zaklade zadane delky celkove zpravy 'n' a stupne generujiciho polynomu 'r'
 * 
 * @param n delka cele zpravy (samotna zprava + generujici polynom)
 * @param r stupen generujiciho polynomu
 * @return std::vector<unsigned char> vektor reprezentujici generujici polynom
 */
std::vector<unsigned char> CRC::FindGeneratingPoly(const size_t &n, const size_t &r)
{
    std::vector<std::vector<unsigned char>> possiblePolys(pow(2, r - 1), std::vector<unsigned char>(r + 1, '1')); // inicializuje vektor vsech moznych kombinaci generujiciho polynomu
    std::vector<std::vector<unsigned char>> middleCoefs = CRC::GenerateGrayArr(r - 1); // vytvori vektor vsech moznych kombinaci mezi krajnimi cleny polynomu g(z)
    /* Pozn.    Vzhledem k tomu, ze nalezeni ireduciblnich polynomu je pomerne slozita zalezitost, tak program vygeneruje vsechny mozne kombinace
                takove, ze z^r bude vzdy pritomno (jinak by se nejednalo o polynum stupne r) a bude vzdy pritomna 1 (tedy z^0), jinak by se
                nejednalo o ireducibilni polynom, jelikoz by bylo mozne vytknout x^1
    */

    std::vector<unsigned char> zn(n + 1, '0'); // inicializace z^n-1 polynom
    zn[0] = '1'; // clen z^n
    zn[zn.size() - 1] = '1'; // clen 1

    // vygeneruje polynomy vsech moznych kombinaci g(z)
    for (size_t i = 0; i < possiblePolys.size(); i++)
        for (size_t j = 1; j < possiblePolys[i].size() - 1; j++)
            possiblePolys[i][j] = middleCoefs[i][j - 1];

    // projde vsechny vygenerovane polynomy g(z) a najde takovy, ktery deli polynom z^n-1 beze zbytku
    for (const auto &i: possiblePolys)
    {
        std::vector<unsigned char> temp = CRC::DividePolys(zn, i);
        int c = 0;
        if (std::all_of(temp.begin(), temp.end(), [](unsigned char x){ return x == '0'; }) || temp.size() == 0) return i; // pokud jsou vsechny prvky v promenne 'temp' (vektor zbytku po deleni polynomu) rovny 0, vrati prislusny polynom
    }

    throw std::logic_error("Nebyl nalezen generujici polynom");
}

/**
 * @brief Prevede textovy retezec na vektor tak, ze jeden znak v retezci se stane jednim prvkem ve vektoru
 * 
 * @param str textovy retezec
 * @return std::vector<unsigned char> textovy retezec prevedeny na vektor
 */
std::vector<unsigned char> CRC::ConvertToVector(const std::string &str)
{
    std::vector<unsigned char> retVec;

    for (const auto &i: str) retVec.push_back(i);

    return retVec;
}

/**
 * @brief Provede deleni polynomu nad telesem GF(2) a navrati zbytek
 * 
 * @param nom citatel
 * @param denom jmenovatel
 * @return std::vector<unsigned char> zbytek po deleni
 */
std::vector<unsigned char> CRC::DividePolys(std::vector<unsigned char> nom, const std::vector<unsigned char> &denom)
{
    while (nom.size() >= denom.size())
    {
        for (size_t i = 0; i < denom.size(); i++)
            nom[i] = XOR(nom[i], denom[i]);
        
        nom.erase(nom.begin(), std::find(nom.begin(), nom.end(), '1'));
    }

    return nom;
}

/**
 * @brief Vygeneruje vsechny mozne permutace 'n' bitu
 * 
 * @param n pozadovany pocet bitu
 * @return std::vector<std::vector<unsigned char>> vektor obsahujici vektory vsech moznych permutaci
 */
std::vector<std::vector<unsigned char>> CRC::GenerateGrayArr(const size_t &n) 
{  
    std::vector<std::string> arr;
  
    arr.push_back("0"); 
    arr.push_back("1"); 
  
    // Every iteration of this loop generates 2*i codes from previously 
    // generated i codes. 
    size_t i, j; 
    for (i = 2; i < (1<<n); i = i<<1) 
    { 
        // Enter the prviously generated codes again in arr[] in reverse 
        // order. Nor arr[] has double number of codes. 
        for (j = i-1 ; j >= 0 ; j--) 
            arr.push_back(arr[j]); 
  
        // append 0 to the first half 
        for (j = 0 ; j < i ; j++) 
            arr[j] = "0" + arr[j]; 
  
        // append 1 to the second half 
        for (j = i ; j < 2*i ; j++) 
            arr[j] = "1" + arr[j]; 
    }
    
    std::vector<std::vector<unsigned char>> retVec;

    for (const auto &i: arr)
        retVec.push_back(CRC::ConvertToVector(i));

    return retVec;
}
