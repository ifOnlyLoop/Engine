#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <cassert>
#include <exception>
#include <string>


class imgppm
{
public:
    
    struct Rgb
    {
        Rgb() : r(0), g(0), b(0)  {}
        Rgb(float rr) : r(rr), g(rr), b(rr) {}
        Rgb(float rr, float gg, float bb) : r(rr), g(gg), b(bb) {}
        float r, g, b;
    };
    
    imgppm() : w(0), h(0), pixels(nullptr) {}
    imgppm(const unsigned int &_w, const unsigned int &_h) :
        w(_w), h(_h), pixels(nullptr)
    {
        pixels = new Rgb[w * h];
        for (int i = 0; i < w * h; ++i) pixels[i] = 0;
    }
    ~imgppm() { if (pixels != nullptr) delete [] pixels; }
    unsigned int w, h;
    Rgb *pixels;
};

// Read a PPM image file
extern imgppm read(const std::string filename)
{
    // binary mode
    std::ifstream ifs;
    ifs.open(filename, std::ios::binary); 
    
    imgppm img;
    
    try 
    {
        // if file didn't open
        if (ifs.fail()) 
            throw("Can't open input file");

        std::string header;
             ifs >> header;
        
        // if not ppm     
        if (strcmp (header.c_str(), "P6") != 0) 
            throw("Can't read input file");
        
        // width, height and .. ?
        int w, h, b;
        ifs >> w >> h >> b;
        img.w = w; 
        img.h = h;

        // throw an exception if bad_alloc
        img.pixels = new imgppm::Rgb[w * h]; 
        
        // skip empty lines in necessary until we get to the binary data
        ifs.ignore(256, '\n'); 
        
        // pix rgb
        unsigned char pix[3];
        
        // read each pixel one by one and convert bytes to floats
        for (int i = 0; i < w * h; ++i) 
        {
            ifs.read(reinterpret_cast<char *>(pix), 3);
            img.pixels[i].r = pix[0] / 255.f;
            img.pixels[i].g = pix[1] / 255.f;
            img.pixels[i].b = pix[2] / 255.f;
        }
        ifs.close();
    }   
    catch (const char *err) 
    {   // display error
        fprintf(stderr, "%s\n", err);
        ifs.close();
    }

    return img;
}


// Save an image to PPM image file
extern void write(const imgppm &img, const std::string filename)
{
    if (img.w == 0 || img.h == 0) 
    { 
        fprintf(stderr, "Can't save an empty image\n"); return; 
    }
    
    std::ofstream ofs;
    try {
        ofs.open(filename, std::ios::binary); // binary mode
        if (ofs.fail()) 
            throw("Can't open output file");
        
        ofs << "P6\n" << img.w << " " << img.h << "\n255\n";
        unsigned char r, g, b;
        
        // loop pixels clamp and convert to byte format
        for (int i = 0; i < img.w * img.h; ++i) {
            r = static_cast<unsigned char>(std::min(1.f, img.pixels[i].r) * 255);
            g = static_cast<unsigned char>(std::min(1.f, img.pixels[i].g) * 255);
            b = static_cast<unsigned char>(std::min(1.f, img.pixels[i].b) * 255);
            ofs << r << g << b;
        }
        ofs.close();
    }
    catch (const char *err) 
    {   // display error
        fprintf(stderr, "%s\n", err);
        ofs.close();
    }
}

extern void getColor (const imgppm& img, float& i, float& j, float& k) 
{   // BGR to RGB
    int
	x = std::min(img.w-1,(uint32_t) ( (i+0)* img.w)),
	y = std::min(img.h-1,(uint32_t) ( (1-j)* img.h));
    
    i=img.pixels[y*img.w+x].r;
    j=img.pixels[y*img.w+x].g;
    k=img.pixels[y*img.w+x].b;
    //return y*width+x;//&0xff
}


/* // TEST
int main(int argc, char **argv)
{
    imgppm I = read("/home/oo/iOL/Engine/src/scripts/fixer/tests/text0.ppm");
    write(I, "./out.ppm");
  
    return 0;
}
/**/