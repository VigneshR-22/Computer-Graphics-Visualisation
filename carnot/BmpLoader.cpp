#include "BmpLoader.h"
#include <iostream>

using namespace std;

BmpLoader::BmpLoader(const char *filename)
{
    std::cout<<"loading texture from file:";
    std::cout<<filename<<std::endl;
    FILE *file;
    file= fopen(filename,"rb");
    if(!file)
    {
        cout<<"ERROR : unable to find texture : "<<filename<<endl;
        exit(0);
    }
    fread(&bfh,sizeof(BITMAPFILEHEADER),1,file);
    if(bfh.bfType != 0x4D42)
    {
        cout<<"ERROR : invalid texture format : "<<filename<<endl;
    }
    fread(&bih,sizeof(BITMAPINFOHEADER),1,file);
    if(bih.biSizeImage==0)
        bih.biSizeImage= bih.biWidth*bih.biHeight*3;
    textureData = new unsigned char [bih.biSizeImage];
    fseek(file,bfh.bfOffBits,0);
    fread(textureData,1,bih.biSizeImage,file);
    width=bih.biWidth;
    height = bih.biHeight;
}
BmpLoader::~BmpLoader()
{
    delete [] textureData;
}
