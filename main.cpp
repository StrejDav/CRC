#include<iostream>
#include<bitset>
#include"crc.cpp"

using namespace std;

int main()
{
    CRC crc;

    string message;
    string r_string;
    int r;
    size_t n;
    size_t choice;

    cout << "Zadejte zpravu: ";
    cin >> message;
    cout << "Zadejte stupen generujiciho polynomu nebo zadejte -1 pro automaticke urceni: ";
    cin >> r;

    if (r == -1)
    {
        try
        {
            r = crc.FindHammingCode(message.size());
        }
        catch(const std::exception& e)
        {
            cout << e.what() << '\n';
            return -1;
        }   
    }

    n = r + message.size();
    crc.FindGeneratingPolynomials(n, r);
    std::vector<Polynomial> gens = crc.ReturnGeneratingPolynomials(n, r);
    int c = 1; 

    cout << "Generujici polynomy jsou:\n";

    for (const auto &i: gens)
    {
        cout << c << ". " << std::string(i.begin(), i.end()) << "\n";
        c++;
    }

    cout << "Zvolte prosim pozadovany generujici polynom z vyse uvedenych: ";
    cin >> choice;

    cout << "Vysledna odeslana zprava: " << crc.Encode(message, gens[choice - 1]);
}
