#include "RatedVox.h"

bool VoxComp::operator()(const RatedVox *v1, const RatedVox *v2) const {
  return v1->delta > v2->delta;
}

RatedVox::RatedVox(DICOMImage::IndexType index) {
  this->idx = index;
}

RatedVox::RatedVox(
    DICOMImage::IndexType index, DICOMImageP image, Region *region) {
  this->idx = index;
  this->delta = RatedVox::ComputeDelta(index, image, region);
}

IndexList RatedVox::GetNeighbors(DICOMImageP image) {
  return SeededRegionGrower::GetNeighbors(image, this->idx);
}

int RatedVox::ComputeDelta(
    DICOMImage::IndexType idx, DICOMImageP image, Region *region) { 
  int intensity = image->GetPixel(idx),
    delta = std::abs(intensity - region->mean);

  return std::abs(intensity - region->mean);
}

