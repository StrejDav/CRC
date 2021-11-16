#include<math.h>
#include<algorithm>
#include<vector>

#define Polynomial std::vector<unsigned char>


class CRC
{
private:
    struct GeneratingPolynomial_t
    {
        size_t n;
        size_t r;
        Polynomial polynomial;
    };

    std::vector<GeneratingPolynomial_t> generatingPolynomials;

    Polynomial ConvertToVector(const std::string &str);
    std::vector<Polynomial> GenerateGrayArr(const size_t &n) ;
    Polynomial DividePolynomials(Polynomial nom, const Polynomial &denom);
    const void MakeMiddleCoefs(Polynomial &possiblePoly, size_t i, const size_t &size);
    
public:
    size_t FindHammingCode(const size_t &k);
    std::string PrettifyPolynomialVector(const Polynomial &polynomial);
    std::vector<Polynomial> ReturnGeneratingPolynomials(const size_t &n, const size_t &r);
    const void FindGeneratingPolynomials(const size_t &n, const size_t &r);
    std::string Encode(const std::string &codeword, const Polynomial &generatingPolynomial);
    static std::string Decode(const std::string &codeword, const size_t &genDeg);
};

inline unsigned char XOR(const unsigned char &a, const unsigned char &b)
{
    return a != b ? '1' : '0';
}
