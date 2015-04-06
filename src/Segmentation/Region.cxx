#include "Region.h"
#include <list>

char Region::next_id = 1;

DICOMImageP Region::Render(DICOMImageP original_image) {
  DICOMImageP new_image = DICOMImage::New();
  DICOMImage::RegionType lpr = original_image->GetLargestPossibleRegion();
  new_image->SetRegions(lpr);
  new_image->Allocate();
  new_image->FillBuffer(-1000);

  int xm = lpr.GetSize(0),
    ym = lpr.GetSize(1),
    zm = lpr.GetSize(2);
  for (int x=0; x<xm; x++) {
    for (int y=0; y<ym; y++) {
      for (int z=0; z<zm; z++) {
        DICOMImage::IndexType idx = {x, y, z};
        if (this->IsMember(idx)) {
          new_image->SetPixel(idx, original_image->GetPixel(idx));
        }
      }
    }
  }

  return new_image;
}

DICOMImageP Region::RenderShell() {
  /* Returns a DICOMImage with all pixels being either 0 or 1. Set pixels 
   * form a hollow shell marking the outer boundry of the region.
   */
  DICOMImageP new_image = DICOMImage::New();
  DICOMImage::RegionType lpr = this->usop->GetLargestPossibleRegion();
  new_image->SetRegions(lpr);
  new_image->Allocate();
  new_image->FillBuffer(0);

  int xm = lpr.GetSize(0),
    ym = lpr.GetSize(1),
    zm = lpr.GetSize(2);
  for (int x=0; x<xm; x++) {
    for (int y=0; y<ym; y++) {
      for (int z=0; z<zm; z++) {
        bool done = false;

        for (int dx=-1; dx<2 && !done; dx++) {
          for (int dy=-1; dy<2; && !done dy++) {
            for (int dz=-1; dz<2; && !done dz++) {
              if (abs(dx) + abs(dy) + abs(dz) == 1) {
                DICOMImage::IndexType idx = {{x+dx, y+dy, z+dx}};
                if (lpr.IsInside(idx)) {
                  if (this->usop->GetPixel(idx) == this->id) {
                    new_image->SetPixel(idx, 1);
                    done = true;
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  return new_image;
}

Region::Region(std::string name, IndexList seeds, DICOMImageP image,
    USOP usop) {
  printf("Stats for region %s:\n", name.c_str());
  this->name = name;
  this->sum = this->mean = this->count = 0;
  this->id = Region::next_id++;
  this->usop = usop;

  for (IndexList::iterator it=seeds.begin(); it!=seeds.end(); it++) {
    DICOMImage::IndexType idx = *it;
    printf("  - seed point %ld, %ld, %ld with val %d\n",
        idx[0], idx[1], idx[2], image->GetPixel(idx));
    this->AddPixel(idx, image, true);
  }

  printf("- Mean: %d\n", this->mean);
};

void Region::AddPixel(DICOMImage::IndexType idx, DICOMImageP image) {
  this->AddPixel(idx, image, true);
}

void Region::AddPixel(
    DICOMImage::IndexType idx, DICOMImageP image, bool update) {
  this->usop->SetPixel(idx, this->id);

  if (update) {
    this->sum += image->GetPixel(idx);
    this->count++;
    this->mean = this->sum / this->count;
  }
}

bool Region::IsMember(DICOMImage::IndexType idx) {
  return this->usop->GetPixel(idx) == this->id;
}

