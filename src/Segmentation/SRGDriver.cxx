/* Just some junk code to call into the better organized SeededRegionGrower
 * class. Will probably be nixed at the point we write tests and/or get a
 * GUI working
 */
#include "SeededRegionGrower.h"

int main(int argc, char* argv[]) {
  printf("main?\n");
  if (argc != 4) {
    printf("Usage: %s <seed_file> <stack_dir> <output_dir>\n", argv[0]);
    return 1;
  }

  std::string seed_file = argv[1];
  std::string series_dir = argv[2]; 
  std::string end_dir = argv[3];

  printf("starting to read\n");
  RegionSeeds seeds = SeededRegionGrower::ReadSeedFile(seed_file);
  printf("done reading\n");

  DICOMImage::Pointer image;
  try {
    printf("start read dicom\n");
    image = SeededRegionGrower::LoadImage(series_dir);
    printf("done read dicom\n");
  } catch (itk::ExceptionObject &excp) {
    fprintf(stderr, "Exception thrown while reading the series\n");
    return 1;
  }

  printf("Starting segmentation\n");
  SegmentationResults results; 
  results = SeededRegionGrower::Segment(image, seeds);

  for (SegmentationResults::iterator it=results.begin(); it!=results.end();
      it++) {
    DICOMImageP mask = it->second->RenderMask();
    SeededRegionGrower::WriteImage(mask, end_dir + "/" + it->first);
  }

  return 0;
}
