#include<iostream>
#include"crc.cpp"

using namespace std;

int main()
{
    string out = CRC::Encode("1000", 3);

    cout << out;
}
