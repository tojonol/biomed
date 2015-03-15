/* Just some junk code to call into the better organized SeededRegionGrower
 * class. Will probably be nixed at the point we write tests and/or get a
 * GUI working
 */
#include "SeededRegionGrower.h"

int main(int argc, char* argv[]) {
  std::string series_dir = "/Users/lanny/Downloads/Head-Neck Cetuximab-Demo/0522c0001/1.3.6.1.4.1.14519.5.2.1.5099.8010.199920086920823171706454903251/scaled/";

  DICOMImage::Pointer image;
  try {
    image = SeededRegionGrower::LoadImage(series_dir);
  } catch (itk::ExceptionObject &excp) {
    fprintf(stderr, "Exception thrown while reading the series\n");
    return 1;
  }

  DICOMImage::SpacingType spacing = image->GetSpacing();

  IndexList brain_seeds,
    skull_seeds;

  IndexList::iterator brain_head = brain_seeds.begin(),
    skull_head = skull_seeds.begin();
  DICOMImage::OffsetType p1 = {254, 270, 244},
    p2 = {230, 291, 271},
    p3 = {258, 302, 220}; 
  brain_seeds.insert(brain_head,
      SeededRegionGrower::ConvertOffset(p1, spacing));
  brain_seeds.insert(brain_head,
      SeededRegionGrower::ConvertOffset(p2, spacing));
  brain_seeds.insert(brain_head,
      SeededRegionGrower::ConvertOffset(p3, spacing));


  DICOMImage::OffsetType p4 = {207, 209, 271},
    p5 = {187, 270, 233},
    p6 = {304, 297, 196}; 
  skull_seeds.insert(skull_head,
      SeededRegionGrower::ConvertOffset(p4, spacing));
  skull_seeds.insert(skull_head,
      SeededRegionGrower::ConvertOffset(p5, spacing));
  skull_seeds.insert(skull_head,
      SeededRegionGrower::ConvertOffset(p6, spacing));

  RegionSeeds seeds;
  seeds.insert(std::pair<std::string, IndexList*>("brain", &brain_seeds));
  seeds.insert(std::pair<std::string, IndexList*>("skull", &skull_seeds));

  SegmentationResults results; 
  results = SeededRegionGrower::Segment(image, seeds);

  DICOMImageP brain = results.find("brain")->second->Render(image),
    skull = results.find("skull")->second->Render(image);

  printf("Brain contains %d voxels and the skull contains %d\n",
    results.find("brain")->second->members.size(),
    results.find("skull")->second->members.size());

  SeededRegionGrower::WriteImage(brain, "/Users/lanny/test_dir/brain");
  SeededRegionGrower::WriteImage(skull, "/Users/lanny/test_dir/skull");
  //SeededRegionGrower::WriteImage(image, "/Users/lanny/test_dir");

  return 0;
}

