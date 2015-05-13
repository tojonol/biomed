#ifndef THRESHOLD_H
#define THRESHOLD_H

#include "itkImage.h"
#include "itkImageRegistrationMethod.h"
#include "itkMinimumMaximumImageFilter.h"
#include "itkPointSet.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkTranslationTransform.h"
#include "itkImageFileReader.h"
#include "itkNormalizeImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkNumericSeriesFileNames.h"
#include "itkIdentityTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include <itksys/SystemTools.hxx>
#include "itkShiftScaleImageFilter.h"
#include "gdcmUIDGenerator.h"
#include "itkCastImageFilter.h"
#include "itkEllipseSpatialObject.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSpatialObjectToImageFilter.h"
#include "itkAffineTransform.h"
#include "itkEuclideanDistancePointMetric.h"
#include "itkLevenbergMarquardtOptimizer.h"
#include "itkPointSetToPointSetRegistrationMethod.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkPointSetToImageFilter.h"
#include "itkRegularStepGradientDescentOptimizerv4.h" 
#include "itkGradientDescentOptimizer.h"
#include "itkConjugateGradientOptimizer.h"
#include "itkCumulativeGaussianOptimizer.h"

const unsigned int InputDimension = 3;
const unsigned int OutputDimension = 2;
typedef signed short PixelType;
typedef itk::PointSet< PixelType, 3 > PointSetType;
typedef itk::Image< PixelType, 3 > ImageType;
typedef itk::Image< PixelType, 2 > OutputImageType;
typedef itk::ImageSeriesReader< ImageType > ReaderType;
typedef itk::ImageSeriesWriter<ImageType, OutputImageType > SeriesWriterType;
typedef itk::GDCMImageIO ImageIOType;
typedef itk::GDCMSeriesFileNames NamesGeneratorType;
typedef std::vector< std::string > FileNamesContainer;
typedef itk::ImageRegionConstIteratorWithIndex< ImageType > IteratorType;
typedef itk::Image< PixelType, 3 > InputImageType;
typedef itk::ResampleImageFilter< InputImageType, InputImageType > ResampleFilterType;
typedef itk::LinearInterpolateImageFunction< InputImageType, double > InterpolatorType;
typedef itk::IdentityTransform< double, InputDimension > TransformType;
typedef itk::ImageRegistrationMethod< ImageType, ImageType >    RegistrationType;
typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
typedef itk::NumericSeriesFileNames  OutputNamesGeneratorType;

#endif