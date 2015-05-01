#include <stdio.h>

#include "itkBinaryBallStructuringElement.h"
#include "itkFlatStructuringElement.h"
#include "itkBinaryMorphologicalOpeningImageFilter.h"
#include "itkBinaryErodeImageFilter.h"

#include "DICOMImage.h"
#include "SeededRegionGrower.h"

typedef itk::BinaryBallStructuringElement<DICOMImage::PixelType, 3> SEType;
typedef itk::BinaryErodeImageFilter <DICOMImage, DICOMImage, SEType> FilterType;

int main(int argc, char* argv[]) {
  if (argc != 4) {
    printf("Usage: %s radius input_image output_dir\n", argv[0]);
    return 1;
  }

  int radius = atoi(argv[1]);
  std::string input_path = argv[2];
  std::string output_path = argv[3];

  DICOMImageP image = SeededRegionGrower::LoadImage(input_path);

  SEType structuring_element;
  structuring_element.SetRadius(5);
  structuring_element.CreateStructuringElement();

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(image);
  filter->SetKernel(structuring_element);
  filter->Update();

  DICOMImageP new_image = filter->GetOutput();

  SeededRegionGrower::WriteImage(new_image, output_path);

  return 0;
}

