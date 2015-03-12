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
 
typedef short PixelType;
typedef itk::Image<PixelType, 3> DICOMImage;
typedef itk::ImageSeriesReader<DICOMImage> ReaderType;
typedef itk::GDCMImageIO ImageIOType;
typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
typedef itk::NumericSeriesFileNames OutputNamesGeneratorType;
typedef std::map<std::string, std::list<DICOMImage::IndexType> >
  RegionSeeds;
typedef std::map<std::string, std::list<DICOMImage::OffsetType> >
  SpacialRegionSeeds;

class SeededRegionGrower {
  public:
    static DICOMImage::Pointer LoadImage(std::string path);
    static DICOMImage::Pointer SegmentSpacial(
        DICOMImage::Pointer image, SpacialRegionSeeds seeds);
    static DICOMImage::Pointer Segment(
        DICOMImage::Pointer image, RegionSeeds seeds);
    static DICOMImage::IndexType ConvertOffset(
        DICOMImage::OffsetType offset, DICOMImage::SpacingType spacing);
};
#endif
