#include "Region.h"

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

