#include <stdio.h>

#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"

#include "DICOMImage.h"
#include "SeededRegionGrower.h"

typedef itk::BinaryBallStructuringElement<DICOMImage::PixelType, 3> SEType;
typedef itk::BinaryErodeImageFilter <DICOMImage, DICOMImage, SEType> ErodeFilterType;
typedef itk::BinaryDilateImageFilter <DICOMImage, DICOMImage, SEType> DilateFilterType;

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
  structuring_element.SetRadius(radius);
  structuring_element.CreateStructuringElement();

  ErodeFilterType::Pointer efilter = ErodeFilterType::New();
  efilter->SetErodeValue(1);
  efilter->SetForegroundValue(1);
  efilter->SetBackgroundValue(0);
  efilter->SetInput(image);
  efilter->SetKernel(structuring_element);
  efilter->Update();

  DilateFilterType::Pointer dfilter = DilateFilterType::New();
  dfilter->SetDilateValue(1);
  dfilter->SetForegroundValue(1);
  dfilter->SetBackgroundValue(0);
  dfilter->SetInput(efilter->GetOutput());
  dfilter->SetKernel(structuring_element);
  dfilter->Update();

  DICOMImageP new_image = dfilter->GetOutput();

  SeededRegionGrower::WriteImage(new_image, output_path);

  return 0;
}

