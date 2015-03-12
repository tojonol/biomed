//#include "itkGDCMImageIO.h"
#include "itkVersion.h"
 
#include "itkImage.h"
#include "itkMinimumMaximumImageFilter.h"
 
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"
 
//#include "itkImageSeriesReader.h"


// voxel value range (short) and dimensions (3, because it's CT)
typedef itk::Image<short, 3> ImageType;
//typedef itk::ImageSeriesReader<ImageType> ReaderType;
typedef itk::GDCMImageIO ImageIOType;
typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
typedef itk::NumericSeriesFileNames OutputNamesGeneratorType;

void print_usage(char **argv) {
  fprintf(stderr, "%s dicom_series", argv[0]);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    print_usage(argv);
    return 1;
  }

  printf("%d.%d\n", itk::Version::GetITKMajorVersion(), itk::Version::GetITKMinorVersion());

  ImageIOType::Pointer gdcmIO = ImageIOType::New();

  /*
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

  */
  return 0;
}
