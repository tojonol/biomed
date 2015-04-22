#ifndef REGION_H
#define REGION_H
#include <map>
#include "DICOMImage.h"

typedef std::map<std::string, IndexList*> RegionSeeds;

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
    DICOMImageP Render(
        DICOMImageP original_image);
    DICOMImageP RenderShell();
    void AddPixel(
        DICOMImage::IndexType idx,
        DICOMImageP image);
    void AddPixel(
        DICOMImage::IndexType idx,
        DICOMImageP image,
        bool update);
    bool IsMember(
        DICOMImage::IndexType idx);
};

typedef std::map<std::string, Region*> SegmentationResults;
#endif
