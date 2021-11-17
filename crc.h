#include<math.h>
#include<algorithm>
#include<vector>

typedef std::vector<unsigned char> Polynomial;

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
    std::vector<Polynomial> FindGeneratingPolynomials(const size_t &n, const size_t &r);
    size_t BinToDec(const Polynomial &errorBinaryPosition);
    const void RepairError(Polynomial &decodedPolynomial, const size_t &position);
    
public:
    size_t FindHammingCode(const size_t &k);
    //std::string PrettifyPolynomialVector(const Polynomial &polynomial); Unused for now <-----
    std::vector<Polynomial> ReturnGeneratingPolynomials(const size_t &n, const size_t &r);
    std::string Encode(const std::string &codeword, const Polynomial &generatingPolynomial);
    std::pair<Polynomial, size_t> Decode(const std::string &codeword, const Polynomial &generatingPolynomial);
};

inline unsigned char XOR(const unsigned char &a, const unsigned char &b)
{
    return a != b ? '1' : '0';
}
