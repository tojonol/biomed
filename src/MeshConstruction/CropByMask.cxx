#include "DICOMImage.h"
#include "SeededRegionGrower.h"

int main(int argc, char* argv[]) {
  if (argc != 4) {
    printf("Usage: %s png_stack mask_stack output_dir\n", argv[0]);
    return 1;
  }

  std::string pngs_dir = argv[1];
  std::string mask_dir = argv[2];
  std::string output_dir = argv[3];

  PNGStackP pngs = SeededRegionGrower::LoadPNGStack(pngs_dir);
  DICOMImageP mask = SeededRegionGrower::LoadImage(mask_dir);

  DICOMImage::RegionType lpr = mask->GetLargestPossibleRegion();
  int xm = lpr.GetSize(0),
    ym = lpr.GetSize(1),
    zm = lpr.GetSize(2);

  int max_x = 0, max_y = 0, max_z = 0, min_x = xm, min_y = ym, min_z = zm;

  for (int x=0; x<xm; x++) {
    for (int y=0; y<ym; y++) {
      for (int z=0; z<zm; z++) {
        DICOMImage::IndexType idx = {x, y, z};
        if (mask->GetPixel(idx) == 1) {
          if (x > max_x) max_x = x;
          if (y > max_y) max_y = y;
          if (z > max_z) max_z = z;

          if (x < min_x) min_x = x;
          if (y < min_y) min_y = y;
          if (z < min_z) min_z = z;
        }
      }
    }
  }

  int x_size = max_x - min_x,
    y_size = max_y - min_y,
    z_size = max_z - min_z;

  PNGStackP cropped_image = PNGStack::New();
  PNGStack::SizeType size = {x_size, y_size, z_size};
  PNGStack::IndexType corner = {0, 0, 0};
  PNGStack::RegionType cropped_lpr(corner, size);

  RGBAPixel transparent;

  transparent.SetRed(255);
  transparent.SetGreen(0);
  transparent.SetBlue(0);
  transparent.SetAlpha(0);

  cropped_image->SetRegions(cropped_lpr);
  cropped_image->Allocate();
  cropped_image->FillBuffer(transparent);

  for (int x=0; x<x_size; x++) {
    for (int y=0; y<y_size; y++) {
      for (int z=0; z<z_size; z++) {
        PNGStack::IndexType cropped_idx = {x, y, z};
        DICOMImage::IndexType source_idx = {x + min_x, y + min_y, z + min_z};

        if (mask->GetPixel(source_idx) == 1) {
          cropped_image->SetPixel(cropped_idx, pngs->GetPixel(source_idx));
        }
      }
    }
  }

  SeededRegionGrower::WriteImage(cropped_image, output_dir);

  return 0;
}
