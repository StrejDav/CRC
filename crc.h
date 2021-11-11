#include<vector>

class CRC
{
private:
    std::vector<unsigned char> ConvertToVector(const std::string &str);
    std::vector<unsigned char> FindGeneratingPoly(const size_t &n, const size_t &r);
    std::vector<std::vector<unsigned char>> GenerateGrayArr(const size_t &n);
    std::vector<unsigned char> DividePolys(std::vector<unsigned char> nom, const std::vector<unsigned char> &denom);
public:
    static std::string Encode(const std::string &codeword, const size_t &genDeg);
};

inline unsigned char XOR(const unsigned char &a, const unsigned char &b)
{
    return a == b ? '0' : '1';
}
