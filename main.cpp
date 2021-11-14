#include<iostream>
#include<bitset>
#include"crc.cpp"

using namespace std;

int main()
{
    string out = CRC::Encode("10110111100000111111111110001", 16);

    cout << out << "\n";
}
