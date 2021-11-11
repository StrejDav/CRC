#include<iostream>
#include "crc.cpp"

using namespace std;

int main()
{
    string out = CRC::Encode("1001", 3);
}