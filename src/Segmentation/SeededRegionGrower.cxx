#include "SeededRegionGrower.h"

#include <algorithm>
#include <climits>
#include <sstream>
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
    printf("d: %d, {%ld, %ld, %ld}, val: %d\n",
        vox->delta, vox->idx[0], vox->idx[1], vox->idx[2],
        image->GetPixel(vox->idx));
  }
  printf("\n\n");
}

RegionSeeds SeededRegionGrower::ReadSeedFile(std::string path) {
  /* A painfully fragile parser for our crappy ad-hoc region seed 
   * specification format. */
  RegionSeeds seeds;
  char *buf = (char*)(malloc(16 * 1024));
  std::string region_name;

  std::ifstream *infile = new std::ifstream(path);
  while (!infile->eof()) {
    std::stringstream ss;
    infile->getline(buf, 16 * 1024);
    ss.write(buf, strlen(buf));
    ss >> region_name;

    IndexList *seed_points = new IndexList();

    while (!ss.eof()) {
      DICOMImage::IndexType idx = {0, 0, 0};
      ss >> idx[0];
      ss >> idx[1];
      ss >> idx[2];

      seed_points->push_front(idx);
    }

    seeds.insert(RegionSeeds::value_type(region_name, seed_points));
  }

  infile->close();
  free(infile);
  free(buf);
  return seeds;
}

void SeededRegionGrower::WriteSeedFile(RegionSeeds seeds, std::string path) {
  /* Like the last thing but in reverse */
  std::ofstream outfile;
  outfile.open(path);

  for (RegionSeeds::iterator it1=seeds.begin(); it1!=seeds.end(); it1++) {
    IndexList seed_points= *(it1->second);
    outfile << it1->first;
    for (IndexList::iterator it2=seed_points.begin(); it2!=seed_points.end();
        it2++) {
      DICOMImage::IndexType idx = *it2;
      outfile << ' ';
      outfile << idx[0] << ' ';
      outfile << idx[1] << ' ';
      outfile << idx[2];
    }

    outfile << '\n';
  }

  outfile.close();
}


void SeededRegionGrower::FilterTouched(
    USOP uso, IndexList *idx_list) {
  /* Takes the USO and a list of indicies into it/a DICOMImage and removes
   * items that are in the SSL or a region. Modifies the list. */
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
  /* Takes a 3-tuple of floats in DICOM measurements (mm I guess) and returns
   * an index that best fits that point in space. This is important because
   * these offset measrues will remain constant across rescales but indicies
   * will not. Also ImageJ gives you spacial offsets insteaf of indicies. */
  DICOMImage::IndexType idx = {
    (DICOMImage::IndexValueType)(offset[0] / spacing[0]),
    (DICOMImage::IndexValueType)(offset[1] / spacing[1]),
    (DICOMImage::IndexValueType)(offset[2] / spacing[2]) };
  return idx;
}

IndexList SeededRegionGrower::GetNeighbors(
    DICOMImageP image, DICOMImage::IndexType idx) {
  /* Returns a list of indicies that are neighbors to the argument index.
   * Removes indicies that are out of the image boundries and does not include
   * the argument index */
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
  /* Takes a DICOMImage and writes a series of images in the DICOM format
   * to the directory indicated by `path`. Does not attempt to create
   * directories if path doesn't exist */
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
  /* Returns a pointer to a the most similar region bordering a voxel. */
  IndexList neighbors = SeededRegionGrower::GetNeighbors(image, idx);
  Region *best_region;
  PixelType best_delta = SHRT_MAX,
    current_delta;

  // Iterate over the neighbors, if any is in a region we compute the current
  // vox's similarity to it (the region) tracking the minimum delta value
  // observed so far.
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
  /* Convenience method for loading a DICOM series from the directory at
   * `path` */
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
  /* Does all the magic of SRG segmentation. You can look at the typdef but 
   * seeds should be a map from strings (region names) to collections of seed
   * indicies that belong to that region. Returns a map from those region names
   * to Regions (that is our region object, not itk::Region). */
  std::set<Region*> regions;

  // The Unholy State Object (USO) is an image simply for performance reasons,
  // what it actually does is help us track the assignment state of all the
  // voxels in the image we're segmenting. Initially every voxel has a value of
  // 0 meaning that it hasn't been assigned to a region nor has it been placed
  // in the SSL. When a vox has been put in the SSL it's given the value of -1
  // and when it's been assigned to a region it's value is set to that region's
  // ID which is unique per colleciton of regions.
  USOP uso = USO::New();

  // Just the ceremony to make the USO the same size as the image being
  // segmented.
  DICOMImage::RegionType lpr = image->GetLargestPossibleRegion();
  uso->SetRegions(lpr);
  uso->Allocate();
  uso->FillBuffer(0);

  // "Sorted Seed List", the name is just so the code reads easier next to the
  // '94 paper. In reality it's a heap of RatedVoxs that have been scored. Note
  // that the SSL is _just_ a heap, adding a vox to it does not update the USO
  // which you'll need to do. If the two get out of sync bad things will happen
  std::vector<RatedVox*> SSL;

  // Initialize Regions based on our seed points.
  for (RegionSeeds::iterator it=seeds.begin(); it != seeds.end(); ++it) {
    std::string region_name = it->first;
    IndexList region_seeds = *(it->second);

    // The Region constructor will modify the USO and track its seeds
    Region *region = new Region(region_name, region_seeds, image, uso);
    regions.insert(region);

    // Add each seed point's neighbors to the SSL so we'll start by visiting
    // them.
    for(IndexList::iterator it2=region_seeds.begin();
        it2 != region_seeds.end(); it2++) {
      DICOMImage::IndexType idx = *it2;

      IndexList neighbors = SeededRegionGrower::GetNeighbors(image, idx);

      for (IndexList::iterator it3=neighbors.begin();
          it3!=neighbors.end(); it3++) {
        RatedVox *neighbor = new RatedVox(*it3, image, region);
        SSL.push_back(neighbor);
        std::push_heap(SSL.begin(), SSL.end(), VoxComp());
      }
    }
  }



  int i = 0,
    vox_count = lpr.GetSize(0) * lpr.GetSize(1) * lpr.GetSize(2);

  // Core of the algorithm right here. If you want to know how it works you
  // should go read the paper but the quick version is that we continually pull
  // the vox with the lowest delta value (that is, the voxel which is most
  // similar to one of the regions it borders on) and add it to its most
  // similar bordering region. Then we take all the neighbors of that vox that
  // aren't already inthe SSL or a region, assign them delta values, and put
  // them in the SSL (according to their delta values).
  while (!SSL.empty()) {
    if (!(i++ % 10000)) {
      printf("%d%% complete\n", i * 100 / vox_count);
    }

    // I know this is ugly but as far as I can tell it's just how STL is
    // supposed to be.
    std::pop_heap(SSL.begin(), SSL.end(), VoxComp());
    RatedVox *vox = SSL.back(),
      *candidate;
    SSL.pop_back();

    Region *best_region = SeededRegionGrower::GetBestBorderingRegion(
        regions, vox->idx, image);

    best_region->AddPixel(vox->idx, image);

    // We've added the voxel under consideration to a region, now add its
    // unvisited neighbors to the SSL
    IndexList neighbors = vox->GetNeighbors(image);
    SeededRegionGrower::FilterTouched(uso, &neighbors);

    // for each neighbor calculate delta and put in the the SSL
    for (IndexList::iterator it=neighbors.begin(); it!=neighbors.end(); it++) {
      best_region = SeededRegionGrower::GetBestBorderingRegion(
        regions, *it, image);

      candidate = new RatedVox(*it, image, best_region);
      SSL.push_back(candidate);
      std::push_heap(SSL.begin(), SSL.end(), VoxComp());
      uso->SetPixel(*it, -1);
    }
  }

  // Hey, the SSL is empty. That must mean that every voxel has been assigned
  // to a region. Great, return results.

  SegmentationResults results;
  std::set<Region*>::iterator it;
  for (it=regions.begin(); it!=regions.end(); it++) {
    Region *region = *it;
    results.insert(std::pair<std::string, Region*>(region->name, region));
  }

  return results;
}
