#include<math.h>
#include<algorithm>
#include<vector>

#define BinVec std::vector<unsigned char>

class CRC
{
private:
    BinVec ConvertToVector(const std::string &str);
    BinVec FindGeneratingPoly(const size_t &n, const size_t &r);
    std::vector<BinVec> GenerateGrayArr(const size_t &n) ;
    BinVec DividePolys(BinVec nom, const BinVec &denom);
    const void MakeMiddleCoefs(BinVec &possiblePoly, size_t i, const size_t &size);
public:
    static std::string Encode(const std::string &codeword, const size_t &genDeg);
    static std::string Decode(const std::string &codeword, const size_t &genDeg);
};

inline unsigned char XOR(const unsigned char &a, const unsigned char &b)
{
    return a != b ? '1' : '0';
}
