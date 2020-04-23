#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <iterator>

#include "Texture.hpp"
/*
    BMP FILE FORMAT

    HEADER              14B
        Signature       2B
        file Size       4B
        Reserved        4B (unused=0)
        Data Offset     4B
    
    INFO HEADER         40B
        Size            4B
        Width           4B
        Height          4B
        Planes          2B (depth?)
        BitCount        2B (number of colors supported)
        Compression     4B (problem when != 0 -> encoded)
        Image Size      4B (0 if not compressed)
        Width           4B
        Height          4B
        ColorsUsed	    4B (number of actually used colors)
        ColorsImportant	4B (number of important colors 0 = all)
        
        Color Table     4*NumColors B
            1B Red
            1B Green
            1B Blue
            1B Reserved
    
    Raster Data
*/

#include <string>
int main()
{   
    freopen("output.out", "w", stdout);
    std::string str ="/home/oo/iOL/Engine/src/scripts/fixer/src/object/tests/test00.bmp";
    Texture bm(str);
    //bm.read(const_cast<char*>(str.c_str()));
    str="output.bmp";
    return 0;
}


