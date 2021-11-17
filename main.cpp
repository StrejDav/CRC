#include<iostream>
#include<bitset>
#include"crc.cpp"

using namespace std;

int main()
{
    CRC crc;

    int flag;

    string message;
    size_t r;
    size_t n;
    size_t choice;


    cout << "Zadejte zpravu: ";
    cin >> message;
    cout << "Zadejte stupen generujiciho polynomu nebo zadejte 0 pro automaticke urceni Hammingovym kodem: ";
    cin >> r;

    if (r == 0)
    {
        try
        {
            r = crc.FindHammingCode(message.size());
        }
        catch(const std::exception& e)
        {
            cout << e.what() << '\n';
            cin >> flag;
            return -1;
        }
    }

    n = r + message.size();
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

    cin >> flag;

    return EXIT_SUCCESS;
}
