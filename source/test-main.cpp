//
// Created by alan on 2/1/20.
//

#include <iostream>
#include <iomanip>
#include <bitset>

using namespace std;

int main(){
    auto bs = bitset<8>(~0x80);
    cout << bs << endl;

    return 0;
}