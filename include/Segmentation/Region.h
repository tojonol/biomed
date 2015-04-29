#ifndef REGION_H
#define REGION_H
#include <map>
#include <itkPNGImageIO.h>
#include "DICOMImage.h"

typedef std::map<std::string, IndexList*> RegionSeeds;
typedef itk::RGBAPixel<unsigned char> RGBAPixel;
typedef itk::Image<RGBAPixel, 3> PNGStack;
typedef itk::Image<RGBAPixel, 2> PNGSlice;
typedef PNGStack::Pointer PNGStackP;
typedef itk::ImageSeriesWriter<PNGStack, PNGSlice> PNGSeriesWriterType;



class Region {
  public:
    std::string name;
    int mean;

  private:
    static char next_id;
    char id;
    int count, sum;
    USOP usop;

  public:
    Region(
        std::string name,
        IndexList seeds,
        DICOMImageP image,
        USOP usop);
    DICOMImageP Render(DICOMImageP original_image);
    DICOMImageP RenderShell();
    DICOMImageP RenderMask();
    PNGStackP RenderPNGStack(DICOMImageP original_image);
    void AddPixel(
        DICOMImage::IndexType idx,
        DICOMImageP image);
    void AddPixel(
        DICOMImage::IndexType idx,
        DICOMImageP image,
        bool update);
    bool IsMember(
        DICOMImage::IndexType idx);

  private:
    DICOMImageP BaseRender(DICOMImageP original_image, bool mask);
    
};

typedef std::map<std::string, Region*> SegmentationResults;
#endif
