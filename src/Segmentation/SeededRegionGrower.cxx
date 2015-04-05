#include "SeededRegionGrower.h"

#include <algorithm>
#include <climits>
#include <vector>
#include <set>
#include <map>

void SSLStatus(std::vector<RatedVox*> &SSL, std::set<Region*> &regions,
    DICOMImageP image) {
  /* Convenience function that will spit out some debug info about the SSL,
   * for debugging purposes only */ 
  std::vector<RatedVox*>::iterator it;
  RatedVox *vox;
  printf("Status of the SSL:\n");
  for (it=SSL.begin(); it!=SSL.end(); it++) {
    vox = *it;
    printf("d: %d, {%d, %d, %d}, val: %d\n",
        vox->delta, vox->idx[0], vox->idx[1], vox->idx[2],
        image->GetPixel(vox->idx));
  }
  printf("\n\n");
}

void SeededRegionGrower::FilterTouched(
    USOP uso, IndexList *idx_list) {
  IndexList::iterator it=idx_list->begin();

  while (it != idx_list->end()) {
    if (uso->GetPixel(*it) != 0) {
      it = idx_list->erase(it);
      continue;
    }

    it++;
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

IndexList SeededRegionGrower::GetNeighbors(
    DICOMImageP image, DICOMImage::IndexType idx) {
  IndexList neighbors;
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

        if (bounds.IsInside(new_idx)) {
          neighbors.push_front(new_idx);
        }
      }
    }
  }

  return neighbors;
}

void SeededRegionGrower::WriteImage(DICOMImageP image, std::string path) {
  int slices = image->GetLargestPossibleRegion().GetSize()[2];
  OutputNamesGeneratorType::Pointer outputNames =
    OutputNamesGeneratorType::New();
  std::string seriesFormat = path + "/" + "IM%d.dcm";
  outputNames->SetSeriesFormat(seriesFormat.c_str());
  outputNames->SetIncrementIndex(1);
  outputNames->SetStartIndex(1);
  outputNames->SetEndIndex(slices);
 
  SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();

  seriesWriter->SetInput(image);
  seriesWriter->SetImageIO(ImageIOType::New());
  seriesWriter->SetFileNames(outputNames->GetFileNames());
  // Pretty sure I'm shedding metadata here but uhh, ima make someone else 
  // deal with that. Sorry bro or broette upon whom dealing with this falls.
  //seriesWriter->SetMetaDataDictionaryArray(&outputArray);
  seriesWriter->Update();
};

Region *SeededRegionGrower::GetBestBorderingRegion(
    std::set<Region*> &regions,
    DICOMImage::IndexType idx,
    DICOMImageP image) {
  IndexList neighbors = SeededRegionGrower::GetNeighbors(image, idx);
  Region *best_region;
  PixelType best_delta = SHRT_MAX,
    current_delta;

  for (IndexList::iterator it=neighbors.begin(); it!=neighbors.end(); it++) {
    for (std::set<Region*>::iterator it2=regions.begin();
        it2!=regions.end(); ++it2) {
      if ((*it2)->IsMember(*it)) {
        current_delta = RatedVox::ComputeDelta(idx, image, *it2);

        if (current_delta < best_delta) {
          best_delta = current_delta;
          best_region = *it2;
        }
      }
    }
  }

  return best_region;
}


DICOMImageP SeededRegionGrower::LoadImage(std::string path) {
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

SegmentationResults SeededRegionGrower::Segment(
    DICOMImageP image, RegionSeeds seeds) {
  std::set<Region*> regions;
  USOP uso = USO::New();
  DICOMImage::RegionType lpr = image->GetLargestPossibleRegion();
  uso->SetRegions(lpr);
  uso->Allocate();
  uso->FillBuffer(0);

  // Not a list and only weakly sorted, w/e easier to read next to the '94 paper
  std::vector<RatedVox*> SSL;

  for (RegionSeeds::iterator it=seeds.begin(); it != seeds.end(); ++it) {
    // Copy seeds into regions (because each seed of a region is definitely
    // going to belong to it) and we'll grow as we iterate;
    std::string region_name = it->first;
    IndexList region_seeds = *(it->second);

    Region *region = new Region(region_name, region_seeds, image, uso);
    regions.insert(region);

    // Insert each seed point into the touched set so we don't visit it
    // and add them to the SSL so their neighbors get considered next iter
    for(IndexList::iterator it2=region_seeds.begin();
        it2 != region_seeds.end(); it2++) {
      DICOMImage::IndexType idx = *it2;

      //RatedVox *seed = new RatedVox(idx, image, region);
      //region->AddPixel(idx, image, true);

      // And add each seed point's neighbors to the SSL
      IndexList neighbors = SeededRegionGrower::GetNeighbors(image, idx);
      for (IndexList::iterator it3=neighbors.begin();
          it3!=neighbors.end(); it3++) {
        RatedVox *neighbor = new RatedVox(*it3, image, region);
        SSL.push_back(neighbor);
        std::push_heap(SSL.begin(), SSL.end(), VoxComp());
      }
    }
  }

  std::string bla;
  int i = 0;
  while (!SSL.empty()) {

    //SSLStatus(SSL, regions, image);

    if (!(i++ % 10000)) {
      printf("On cycle %d\n", i);
    }

    // Vectors are straight retarded, std is a pile of junk
    std::pop_heap(SSL.begin(), SSL.end(), VoxComp());
    RatedVox *vox = SSL.back(),
      *candidate;
    SSL.pop_back();

    Region *best_region = SeededRegionGrower::GetBestBorderingRegion(
        regions, vox->idx, image);

    best_region->AddPixel(vox->idx, image);

    IndexList neighbors = vox->GetNeighbors(image);
    SeededRegionGrower::FilterTouched(uso, &neighbors);

    for (IndexList::iterator it=neighbors.begin(); it!=neighbors.end(); it++) {
      best_region = SeededRegionGrower::GetBestBorderingRegion(
        regions, *it, image);

      candidate = new RatedVox(*it, image, best_region);
      SSL.push_back(candidate);
      std::push_heap(SSL.begin(), SSL.end(), VoxComp());
      uso->SetPixel(*it, -1);
    }
  }

  SegmentationResults results;
  std::set<Region*>::iterator it;
  for (it=regions.begin(); it!=regions.end(); it++) {
    Region *region = *it;
    results.insert(std::pair<std::string, Region*>(region->name, region));
  }

  return results;
}
