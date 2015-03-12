#include "itkGDCMImageIO.h"
 
#include "itkImage.h"
#include "itkMinimumMaximumImageFilter.h"
 
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"
 
#include "itkImageSeriesReader.h"


// voxel value range (short) and dimensions (3, because it's CT)
typedef short PixelType;
typedef itk::Image<PixelType, 3> DICOMImage;
typedef itk::ImageSeriesReader<DICOMImage> ReaderType;
typedef itk::GDCMImageIO ImageIOType;
typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
typedef itk::NumericSeriesFileNames OutputNamesGeneratorType;

void print_usage(char **argv) {
  fprintf(stderr, "Usage: %s dicom_series_dir\n", argv[0]);
}

DICOMImage::IndexType convertOffset(DICOMImage::OffsetType offset,
                                    DICOMImage::SpacingType spacing) {
  return { (DICOMImage::IndexValueType)(offset[0] / spacing[0]),
           (DICOMImage::IndexValueType)(offset[1] / spacing[1]),
           (DICOMImage::IndexValueType)(offset[2] / spacing[2]) };
};

int main(int argc, char* argv[]) {
  if (argc < 2) {
    print_usage(argv);
    return 1;
  }

  ImageIOType::Pointer gdcmIO = ImageIOType::New();

  InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
  inputNames->SetInputDirectory(argv[1]);

  const ReaderType::FileNamesContainer & filenames =
    inputNames->GetInputFileNames();
 
  ReaderType::Pointer reader = ReaderType::New();
 
  reader->SetImageIO(gdcmIO);
  reader->SetFileNames(filenames);
  try {
    reader->Update();
  } catch (itk::ExceptionObject &excp) {
    fprintf(stderr, "Exception thrown while reading the series\n");
    return 1;
  }

  DICOMImage *image = reader->GetOutput();

  DICOMImage::OffsetType p1s = { 242.188, 291.992, 208.800 };
  DICOMImage::IndexType p1 = convertOffset(p1s, spacing);

  printf("p1 is: %d, %d, %d\n", p1[0], p1[1], p1[2]);

  PixelType p1v = image->GetPixel(p1);

  printf("p1 should be %d, is: %d\n", 30, p1v);

  return 0;
}
