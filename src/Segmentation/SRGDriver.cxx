/* Just some junk code to call into the better organized SeededRegionGrower
 * class. Will probably be nixed at the point we write tests and/or get a
 * GUI working
 */
#include "SeededRegionGrower.h"

void print_usage(char **argv) {
  fprintf(stderr, "Usage: %s dicom_series_dir\n", argv[0]);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    print_usage(argv);
    return 1;
  }

  DICOMImage::Pointer image;
  try {
    image = SeededRegionGrower::LoadImage(argv[1]);
  } catch (itk::ExceptionObject &excp) {
    fprintf(stderr, "Exception thrown while reading the series\n");
    return 1;
  }

  DICOMImage::OffsetType p1s = { 242, 291, 208 };
  DICOMImage::SpacingType spacing = image->GetSpacing();
  DICOMImage::IndexType p1 = SeededRegionGrower::ConvertOffset(p1s, spacing);

  printf("p1 is: %ld, %ld, %ld\n", p1[0], p1[1], p1[2]);
  PixelType p1v = image->GetPixel(p1);
  printf("p1 should be %d, is: %d\n", 30, p1v);
  return 0;
}

