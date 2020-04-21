#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <iterator>

class Texture
{
private:
    std::string FILE_PATH;
    uint16_t signature;  
    uint32_t fileSize;   
    uint32_t dataOffset; 
    uint32_t width;      
    uint32_t height;     
    uint16_t depth;
    std::vector<char> textureData;       
public:
    Texture();
   ~Texture();

    int operator [] (int i)
    {
		return (int)textureData[i];
	}

    void loadImage(const std::string &file);
};

Texture::Texture()
{
}
Texture::~Texture()
{
}


void Texture::loadImage(const std::string &file)
{	// I asssumed texture in bmp format
    FILE_PATH=file;
	// HEADER 
	static constexpr size_t HEADER_SIZE = 54;
    std::ifstream bmp(file,std::ios::binary);
	// read data
    std::array <char, HEADER_SIZE> header;
    bmp.read(header.data(),header.size());
    
    signature  = *reinterpret_cast<uint16_t*>(&header[0]);
    fileSize   = *reinterpret_cast<uint32_t*>(&header[2]); // includes resereved
    dataOffset = *reinterpret_cast<uint32_t*>(&header[10]);
    width      = *reinterpret_cast<uint32_t*>(&header[18]);
    height     = *reinterpret_cast<uint32_t*>(&header[22]);
    depth      = *reinterpret_cast<uint16_t*>(&header[28]);

	// for debugging
    //std::cout << "fileSize: "    << fileSize   << std::endl;
    //std::cout << "dataOffset: "  << dataOffset << std::endl;
    //std::cout << "width: "   << width  << std::endl;
    //std::cout << "height: "  << height << std::endl;
    //std::cout << "depth: "   << depth  << "-bit" << std::endl;

    // GET RID OF REMAINING DATA AND JUMP TO PIXELS 
    std::vector<char> img(dataOffset - HEADER_SIZE);
    bmp.read(textureData.data(),textureData.size());
    
	
	// Get TO PIXELS
    auto dataSize = ((width * 3 + 3) & (~3)) * height;
    textureData.resize(dataSize);
    bmp.read(textureData.data(),textureData.size());
	
    // Swapping/Convert (B, G, R) to (R, G, B)
    char temp = 0;
    for (auto i = 0; i<dataSize; i+=3) 
    {
        temp     		 = textureData[i];
        textureData[i]   = textureData[i+2];
        textureData[i+2] = temp;
        /* for debugging
		std::cout << " R: " << int(img[i+0] & 0xff) 
				  << " G: " << int(img[i+1] & 0xff) 
				  << " B: " << int(img[i+2] & 0xff) 
				  << std::endl;*/
    }
}


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