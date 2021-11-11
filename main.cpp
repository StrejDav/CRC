#include<iostream>
#include "crc.cpp"

using namespace std;

int main()
{
    string out = CRC::Encode("111111", 3);
}
