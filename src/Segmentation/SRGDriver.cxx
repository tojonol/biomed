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


  DICOMImage::IndexType i1 = {{1, 2, 3}},
    i2 = {{4, 5, 6}},
    i3 = {{7, 8, 9}},
    i4 = {{3, 1, 6}};

  RatedVox *v1 = new RatedVox(i1),
    *v2 = new RatedVox(i2),
    *v3 = new RatedVox(i3),
    *v4 = new RatedVox(i4);

  v1->delta = 1;
  v2->delta = 2;
  v3->delta = 3;
  v4->delta = 4;

  std::vector<RatedVox *> SSL;
  SSL.push_back(v3);
  push_heap(SSL.begin(), SSL.end(), VoxComp());
  SSL.push_back(v1);
  push_heap(SSL.begin(), SSL.end(), VoxComp());
  SSL.push_back(v2);
  push_heap(SSL.begin(), SSL.end(), VoxComp());
  SSL.push_back(v4);
  push_heap(SSL.begin(), SSL.end(), VoxComp());

  int d1, d2, d3, d4;
  std::pop_heap(SSL.begin(), SSL.end(), VoxComp());
  d1 = SSL.back()->delta;
  SSL.pop_back();
  std::pop_heap(SSL.begin(), SSL.end(), VoxComp());
  d2 = SSL.back()->delta;
  SSL.pop_back();
  std::pop_heap(SSL.begin(), SSL.end(), VoxComp());
  d3 = SSL.back()->delta;
  SSL.pop_back();
  std::pop_heap(SSL.begin(), SSL.end(), VoxComp());
  d4 = SSL.back()->delta;
  SSL.pop_back();

  printf("%d, %d, %d, %d\n", d1, d2, d3, d4);

  DICOMImage::SpacingType spacing = image->GetSpacing();

  IndexList brain_seeds, skull_seeds, space_seeds;

  IndexList::iterator brain_head = brain_seeds.begin(),
    skull_head = skull_seeds.begin(),
    space_head = space_seeds.begin();
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

  DICOMImage::OffsetType p7 = {96, 60, 97},
    p8 = {93, 83, 51};
  space_seeds.insert(space_head,
      SeededRegionGrower::ConvertOffset(p4, spacing));
  space_seeds.insert(space_head,
      SeededRegionGrower::ConvertOffset(p5, spacing));

  RegionSeeds seeds;
  seeds.insert(std::pair<std::string, IndexList*>("brain", &brain_seeds));
  seeds.insert(std::pair<std::string, IndexList*>("skull", &skull_seeds));
  seeds.insert(std::pair<std::string, IndexList*>("space", &space_seeds));

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

