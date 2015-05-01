#ifndef SEEDEDREGIONGROWER_H
#define SEEDEDREGIONGROWER_H
#include "DICOMImage.h"
#include "Region.h"
#include "RatedVox.h"

class SeededRegionGrower {
  public:
    static DICOMImageP LoadImage(
        std::string path);
    static PNGStackP LoadPNGStack(
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
    static void WriteImage(
        DICOMImageP image,
        std::string path);
    static void WriteImage(
        PNGStackP image,
        std::string path);
    static RegionSeeds ReadSeedFile(
        std::string path);
    static void WriteSeedFile(
        RegionSeeds seeds, std::string path);
  private:
    static void FilterTouched(
        USOP uso,
        IndexList *idx_list);
    static Region *GetBestBorderingRegion(
        std::set<Region*> &regions,
        DICOMImage::IndexType idx,
        DICOMImageP image);
};
#endif
