/* Declares all the DICOM related types. Also the USO because Region needs it
 * and making a seperate header just for that would be a drag.
 */
#ifndef DICOMIMAGE_H
#define DICOMIMAGE_H
#include <list>
#include <string>

#include "itkImage.h"
 
#include "itkGDCMImageIO.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

#include "itkRegularExpressionSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"

typedef short PixelType;
typedef itk::Image<PixelType, 2> DICOMSlice;
typedef itk::Image<PixelType, 3> DICOMImage;
typedef std::list<DICOMImage::IndexType> IndexList;
typedef DICOMImage::Pointer DICOMImageP;
typedef itk::ImageSeriesReader<DICOMImage> ReaderType;
typedef itk::GDCMImageIO ImageIOType;
typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
typedef itk::NumericSeriesFileNames OutputNamesGeneratorType;
typedef itk::ImageSeriesWriter<DICOMImage, DICOMSlice> SeriesWriterType;

typedef itk::Image<char, 3> USO; // USO == Unholy State Object, you're welcome
typedef USO::Pointer USOP;

#endif
