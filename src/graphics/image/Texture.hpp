#pragma once
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <iterator>
#include <algorithm>
#include"../../scripts/transform/gvec.hpp"


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


// bmp header size
static constexpr size_t HEADER_SIZE = 54;

class Texture
{
private:
    std::string FILE_PATH;
    uint16_t signature;  
    uint32_t fileSize;   
    uint32_t dataOffset;   
    uint16_t depth;
    int ROW_PADDED;
    unsigned char HEADER[54];
    std::vector<char> rasterData,headerData,offsetData;       
    std::array <char, HEADER_SIZE> header;

    std::vector<unsigned char> RASTER_DATA;

public:
    uint32_t width;      
    uint32_t height;   
    Texture(std::string);
   ~Texture();

    int operator [] (int i)
    {
		return (int)rasterData[i];
	}
  
    void read(char *);
    void write(const std::string &file);
    void getColor (float&, float&, float&);
};

Texture:: Texture(std::string str)
{
    read(const_cast<char*>(str.c_str()));
}
Texture::~Texture(){}

void Texture::getColor (float& i, float& j, float& k) 
{   // BGR to RGB
    int
	x = std::min(width -1,(uint32_t) ( (i+0)* width )),
	y = std::min(height-1,(uint32_t) ( (1-j)* height));
    
    i=(int)RASTER_DATA[3*(y*width+x)+0];
    j=(int)RASTER_DATA[3*(y*width+x)+1];
    k=(int)RASTER_DATA[3*(y*width+x)+2];
    //return y*width+x;//&0xff
}

void Texture::read(char* filename)
{
    int i;
    FILE* f = fopen(filename,"rb");

    if(f == NULL)
        throw "Argument Exception";

    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    width  = *(uint32_t*) &info[18];
    height = *(uint32_t*) &info[22];
    uint32_t dataOffset = *(uint32_t*) &info[10];
    int offsize = dataOffset-HEADER_SIZE;
    unsigned char* off = new unsigned char[offsize];
    fread(off, sizeof(unsigned char), offsize, f);
    delete[]off;
    //std::cout << std::endl;
    //std::cout << "  Name: " << filename << std::endl;
    //std::cout << " Width: " << width << std::endl;
    //std::cout << "Height: " << height << std::endl;

    int row_padded = (width*3 + 3) & (~3);
    unsigned char* data = new unsigned char[row_padded];
    unsigned char tmp;

    for(int i = 0; i < height; i++)
    {
        fread(data, 1, row_padded, f);
        for(int j=0;j<width*3;j+=3)
        {   
            // Convert (B, G, R) to (R, G, B)
            RASTER_DATA.push_back(data[j+2]&0xff);//&0xff);
            RASTER_DATA.push_back(data[j+1]&0xff);//&0xff);
            RASTER_DATA.push_back(data[j+0]&0xff);//&0xff);
            //std::cout<<(int)tmp<<' ';
            //std::cout << "R: " << int(data[i] & 0xff) << " G: " << int(data[i+1] & 0xff) << " B: " << int(data[i+2] & 0xff) << std::endl;
        }//
    }
    delete[]data;
    fclose(f);
 //   return data;
}