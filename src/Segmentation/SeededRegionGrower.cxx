#include "SeededRegionGrower.h"

#include <algorithm>
#include <vector>
#include <set>
#include <map>

class RatedVox {
  int delta;
  DICOMImage::IndexType idx;

  RatedVox(DICOMImage::IndexType index) {
    this->idx = index;
  }
}

class Region {
  string name;
  PixelType mean;
  IndexSet members;

  Region(string name, RegionMembers seeds, DICOMImageP image) {
    int sum = 0,
      i = 0;

    this->name = name;

    for(RegionMembers::iterator it=region_seeds.begin();
        it != it.end(); it++) {
      DICOMImage::IndexType idx = it*;
      sum += image->GetPixel(idx);
      i++;

      this->IndexSet.insert(idx);
    }

    this->mean = sum / i;
  };
}

struct VoxComp {
 bool operator()(const RatedVox& v1, const RatedVox& v2) {
   return v1.delta < v2.delta && v1.delta == v2.delta;
 }
}

DICOMImage::IndexType SeededRegionGrower::ConvertOffset(
    DICOMImage::OffsetType offset, DICOMImage::SpacingType spacing) {
  DICOMImage::IndexType idx = {
    (DICOMImage::IndexValueType)(offset[0] / spacing[0]),
    (DICOMImage::IndexValueType)(offset[1] / spacing[1]),
    (DICOMImage::IndexValueType)(offset[2] / spacing[2]) };
  return idx;
}

RegionMembers SeededRegionGrower::GetNeighbors(
    DICOMImageP image, DICOMImage::IndexType idx) {
  RegionMembers neighbors;
  DICOMImage::RegionType bounds = image->GetLargestPossibleRegion();

  for (int x=-1; x<2; x++) {
    for (int y=-1; y<2; y++) {
      for (int z=-1; z<2; z++) {
        if (x==0 && y==0 && z==0)
          // No man is a neighbor unto himself
          continue;

        DICOMImage::IndexType new_idx = {{ idx[0] + x,
                                           idx[1] + y,
                                           idx[2] + z }};

        if (bounds->IsInside(new_idx)) {
          neighbors.push_front(new_idx);
        }
      }
    }
  }

  return neighbors;
}

DICOMImage::Pointer SeededRegionGrower::LoadImage(std::string path) {
  ImageIOType::Pointer gdcmIO = ImageIOType::New();

  InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
  inputNames->SetInputDirectory(path);

  const ReaderType::FileNamesContainer & filenames =
    inputNames->GetInputFileNames();
 
  ReaderType::Pointer reader = ReaderType::New();
 
  reader->SetImageIO(gdcmIO);
  reader->SetFileNames(filenames);
  reader->Update();

  return reader->GetOutput();
}

DICOMImage::Pointer SeededRegionGrower::Segment(
    DICOMImageP image, Regions seeds) {
  Regions regions;
  IndexSet touched;

  // Not a list and only weakly sorted, w/e easier to read next to the '94 paper
  std::vector<RatedVox> SSL;

  for (Regions::iterator it=seeds.begin(); it != seeds.end(); ++it) {
    // Copy seeds into regions (because each seed of a region is definitely
    // going to belong to it) and we'll grow as we iterate;
    string region_name = it->first;
    RegionMembers region_seeds = *(it->second);

    regions.insert(
      std::pair<std::string, RegionMembers>(region_name, region_ssds));

    // Insert each seed point into the touched set so we don't visit it
    // and add it's neighbors to the SSL
    for(RegionMembers::iterator it2=region_seeds.begin();
        it2 != it2.end(); it2++) {
      DICOMImage::IndexType idx = *it;
      touched.insert(idx);

      RegionMembers neighbors = SeededRegionGrower::GetNeighbors(image, idx);
    }
  }
}
