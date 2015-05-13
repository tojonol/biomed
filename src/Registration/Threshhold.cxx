#include "Threshold.h"

#include <iostream>
#include <vector>



ImageType::Pointer LoadImage( std::string path ) {
	ReaderType::Pointer reader = ReaderType::New();
	
	ImageIOType::Pointer dicomIO = ImageIOType::New();
	reader->SetImageIO( dicomIO );
	
	NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
	nameGenerator->SetInputDirectory( path );
	
	FileNamesContainer fileNames = nameGenerator->GetInputFileNames();
	reader->SetFileNames( fileNames );
	reader->Update();

	ImageType::Pointer image = reader->GetOutput();
	return image;
}


void WriteImage(std::string in, std::string out, float z, float t0, float t1, float t2,std::string match )
{
	//std::cout << " "<<z <<" " <<t0<< " " <<t1<< " "<<t2 <<" ";
 
  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
  inputNames->SetInputDirectory( in );
 
  const ReaderType::FileNamesContainer & filenames = inputNames->GetInputFileNames();
 
  ReaderType::Pointer reader = ReaderType::New();
 
  reader->SetImageIO( gdcmIO );
  reader->SetFileNames( filenames );
    
    reader->Update();
    

  InterpolatorType::Pointer interpolator = InterpolatorType::New();
 
  TransformType::Pointer transform = TransformType::New();
  transform->SetIdentity();
 
  const InputImageType::SpacingType& inputSpacing =  reader->GetOutput()->GetSpacing();
  const InputImageType::RegionType& inputRegion = reader->GetOutput()->GetLargestPossibleRegion();
  const InputImageType::SizeType& inputSize = inputRegion.GetSize();
 
 
  InputImageType::SpacingType outputSpacing;
  outputSpacing[0] = 0;
  outputSpacing[1] = 0;
  outputSpacing[2] = z;
 
  bool changeInSpacing = false;
  for (unsigned int i = 0; i < 3; i++)
    {
    if (outputSpacing[i] == 0.0)
      {
      outputSpacing[i] = inputSpacing[i];
      }
    else
      {
      changeInSpacing = true;
      }
    }
  InputImageType::SizeType   outputSize;
  typedef InputImageType::SizeType::SizeValueType SizeValueType;
  outputSize[0] = static_cast<SizeValueType>(inputSize[0] * inputSpacing[0] / outputSpacing[0] + .5);
  outputSize[1] = static_cast<SizeValueType>(inputSize[1] * inputSpacing[1] / outputSpacing[1] + .5);
  outputSize[2] = static_cast<SizeValueType>(inputSize[2] * inputSpacing[2] / outputSpacing[2] + .5);
 
	typedef itk::TranslationTransform<double,3> TranslationTransformType;
  TranslationTransformType::Pointer trans = TranslationTransformType::New();
  TranslationTransformType::OutputVectorType translation;
  translation[0] = t0;
  translation[1] = t1;
  translation[2] = t2;
  trans->Translate(translation);

  ResampleFilterType::Pointer resampler = ResampleFilterType::New();
  resampler->SetInput( reader->GetOutput() );
  resampler->SetTransform( trans.GetPointer() );
  resampler->SetInterpolator( interpolator );
  resampler->SetOutputOrigin ( reader->GetOutput()->GetOrigin());
  resampler->SetOutputSpacing ( outputSpacing );
  resampler->SetOutputDirection ( reader->GetOutput()->GetDirection());
  resampler->SetSize ( outputSize );
  resampler->Update ();

  itksys::SystemTools::MakeDirectory( out );
 
  OutputNamesGeneratorType::Pointer outputNames = OutputNamesGeneratorType::New();
  std::string seriesFormat(out);
  seriesFormat = seriesFormat + "/" + "%d.dcm";
  outputNames->SetSeriesFormat (seriesFormat.c_str());
  outputNames->SetStartIndex (1);
  outputNames->SetEndIndex (outputSize[2]);
 
  SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();

  seriesWriter->SetInput( resampler->GetOutput() );
    seriesWriter->SetImageIO( gdcmIO );
    seriesWriter->SetFileNames( outputNames->GetFileNames() );
    //seriesWriter->SetMetaDataDictionaryArray( &outputArray );
	seriesWriter->SetMetaDataDictionaryArray(reader->GetMetaDataDictionaryArray() );

    seriesWriter->Update();
}


std::vector<ImageType::IndexType> Threshold(ImageType::Pointer image, PointSetType::Pointer pointSet, int numpoints) {
	IteratorType it( image, image->GetRequestedRegion() );
	IteratorType it1( image, image->GetRequestedRegion() );

	ImageType::SizeType reqSize = image->GetRequestedRegion().GetSize();
	int size = reqSize[0] * reqSize[1] * reqSize[2];
	std::vector<ImageType::IndexType> voxels(size);

	int i = 0;
	unsigned long pointId = 0;
	typedef PointSetType::PointType PointType;
    PointType point;
	int pointIdreal = 0;
	int numb = 0;
	it1.GoToBegin();
	while ( !it1.IsAtEnd() ) {
			if (it1.Get() != -1000) {
				numb++;
			}
			++it1;
	}
	int num = numb/numpoints;
	it.GoToBegin();
	while ( !it.IsAtEnd() ) {
			if (it.Get() != -1000 && pointIdreal <numpoints) {
				ImageType::IndexType idx = it.GetIndex();
				//voxels[i] = idx;
				//printf("  -stored point %ld, %ld, %ld with val %d at index[%d]\n", voxels[i][0], voxels[i][1], voxels[i][2], it.Get(), i);
				//++i;
				if (pointId%num ==0){
					image->TransformIndexToPhysicalPoint( it.GetIndex() , point );
					pointSet->SetPoint( pointIdreal, point );
					pointSet->SetPointData( pointIdreal, it.Get() );
					pointIdreal++;
				}
				++pointId;
			}
			++it;
	}
	//voxels.resize(i);
	
	return voxels;
}

int main(int argc, char* argv[]) {
	  if( argc < 4 )
    {
    std::cerr << "Usage: "
              << argv[0]
              << " FixedDicomDirectory MovingDicomDirectory RegisteredDicomDirectory numberOfSamplePoints"
              << std::endl;
    return EXIT_FAILURE;
    }

	ImageType::Pointer image = LoadImage( argv[1] );
	ImageType::Pointer image2 = LoadImage( argv[2] );
	 
	PointSetType::Pointer pointSet = PointSetType::New();
	PointSetType::Pointer pointSet2 = PointSetType::New();

	std::vector<ImageType::IndexType> voxels1 = Threshold(image, pointSet, atoi(argv[4]));
	std::vector<ImageType::IndexType> voxels2 = Threshold(image2, pointSet2, atoi(argv[4]));

	std::cout << "Number of FixedPoints = " << pointSet->GetNumberOfPoints() << std::endl;
	std::cout << "Number of MovingPoints = " << pointSet2->GetNumberOfPoints() << std::endl;
	
	ImageType::RegionType region = image->GetLargestPossibleRegion();
	ImageType::SizeType size = region.GetSize();

	ImageType::RegionType region2 = image2->GetLargestPossibleRegion();
	ImageType::SizeType size2 = region2.GetSize();

	float z = (float)size2[2]/(float)size[2];

  typedef itk::EuclideanDistancePointMetric< PointSetType, PointSetType> MetricType;
  MetricType::Pointer  metric = MetricType::New();

  typedef itk::TranslationTransform< double, 3 >      TransformType;
  TransformType::Pointer transform = TransformType::New();

  typedef itk::LevenbergMarquardtOptimizer OptimizerType;
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  
  typedef itk::PointSetToPointSetRegistrationMethod< PointSetType,PointSetType >RegistrationType;
  RegistrationType::Pointer   registration  = RegistrationType::New();

  optimizer->SetUseCostFunctionGradient(false);
  optimizer->SetNumberOfIterations( 100 );
  optimizer->SetValueTolerance( 1e-4 );
  optimizer->SetGradientTolerance( 1e-4 );
  optimizer->SetEpsilonFunction( 1e-5 );
  
  transform->SetIdentity();
  registration->SetInitialTransformParameters( transform->GetParameters() );

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetFixedPointSet( pointSet );
  registration->SetMovingPointSet(   pointSet2   );
  registration->Update();
  //std::cout << "Solution = " << transform->GetParameters() << std::endl;
  //std::cout<<" "<<size[2]<<" "<<size2[2]<<" ";
  float t0 = transform->GetParameters()[0];
  float t1 = transform->GetParameters()[1];
  float t2 = transform->GetParameters()[2];
  WriteImage(argv[2], argv[3],z,t0,t1,t2,argv[1]);
  return  0;
}