#ifndef SEEDEDREGIONGROWER_H
#define SEEDEDREGIONGROWER_H
#include <map>
#include <list>
#include <string>

#include "itkImage.h"
 
#include "itkGDCMImageIO.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

#include "itkNumericSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"

 
typedef short PixelType;
typedef itk::Image<PixelType, 3> DICOMImage;
typedef itk::Image<PixelType, 2> DICOMSlice;
typedef std::list<DICOMImage::IndexType> IndexList;
typedef DICOMImage::Pointer DICOMImageP;
typedef itk::ImageSeriesReader<DICOMImage> ReaderType;
typedef itk::GDCMImageIO ImageIOType;
typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
typedef itk::NumericSeriesFileNames OutputNamesGeneratorType;
typedef std::map<std::string, IndexList*> RegionSeeds;
typedef itk::ImageSeriesWriter<DICOMImage, DICOMSlice> SeriesWriterType;

struct IndexComp {
 bool operator()(
     const DICOMImage::IndexType &i1,
     const DICOMImage::IndexType &i2) const;
};

typedef std::set<DICOMImage::IndexType, IndexComp> IndexSet;

class Region {
  public:
    std::string name;
    PixelType mean;
    IndexSet members;
  private:
    int count, sum;

  public:
    Region(
        std::string name,
        IndexList seeds,
        DICOMImageP image);
    DICOMImageP Render(
        DICOMImageP original_image);
    void AddPixel(
        DICOMImage::IndexType idx,
        DICOMImageP image);
    bool IsMember(
        DICOMImage::IndexType idx);
};

typedef std::map<std::string, Region*> SegmentationResults;

class SeededRegionGrower {
  public:
    static DICOMImageP LoadImage(
        std::string path);
    static SegmentationResults Segment(
        DICOMImageP image,
        RegionSeeds seeds);
    static DICOMImage::IndexType ConvertOffset(
        DICOMImage::OffsetType offset,
        DICOMImage::SpacingType spacing);
    static IndexList GetNeighbors(
        DICOMImageP image,
        DICOMImage::IndexType idx);
    static void FilterTouched(
        IndexSet &touched,
        IndexList *idx_list);
    static void WriteImage(
        DICOMImageP image,
        std::string path);
  private:
    static Region *GetBestBorderingRegion(
        std::set<Region*> &regions,
        DICOMImage::IndexType idx,
        DICOMImageP image);
};

#endif
