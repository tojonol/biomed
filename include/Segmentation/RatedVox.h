#ifndef RATEDVOX_H
#define RATEDVOX_H
#include "DICOMImage.h"
#include "Region.h"
#include "SeededRegionGrower.h"

class RatedVox {
  public:
    int delta;
    DICOMImage::IndexType idx;

    RatedVox(
        DICOMImage::IndexType index);
    RatedVox(
        DICOMImage::IndexType index,
        DICOMImageP image,
        Region *region);
    IndexList GetNeighbors(
        DICOMImageP image);

    static int ComputeDelta(
        DICOMImage::IndexType idx,
        DICOMImageP image,
        Region *region);
};

struct VoxComp {
  bool operator()(const RatedVox *v1, const RatedVox *v2) const;
};

#endif
