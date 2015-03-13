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
typedef std::list<DICOMImage::IndexType> RegionMembers;
typedef std::set<DICOMImage::IndexType> IndexSet;
typedef itk::Image<PixelType, 3> DICOMImage;
typedef DICOMImage::Pointer DICOMImageP;
typedef itk::ImageSeriesReader<DICOMImage> ReaderType;
typedef itk::GDCMImageIO ImageIOType;
typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
typedef itk::NumericSeriesFileNames OutputNamesGeneratorType;
typedef std::map<std::string, *RegionMembers> Regions;
typedef std::map<std::string, std::list<DICOMImage::OffsetType> >
  SpacialRegions;

class SeededRegionGrower {
  public:
    static DICOMImageP LoadImage(std::string path);
    static DICOMImageP SegmentSpacial(
        DICOMImageP image, SpacialRegions seeds);
    static DICOMImageP Segment(
        DICOMImageP image, Regions seeds);
    static DICOMImage::IndexType ConvertOffset(
        DICOMImage::OffsetType offset, DICOMImage::SpacingType spacing);

  private:
    static RegionMembers GetNeighbors(
      DICOMImageP image, DICOMImage::IndexType idx);
};

#endif
