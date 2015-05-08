
#include "itkImageFileReader.h"
#include "itkImageRegionConstIterator.h"

#include <vector>
#include <string>

const unsigned Dimension = 3;
typedef short PixelType;
typedef itk::Image< PixelType, Dimension > ImageType;
typedef ImageType::SpacingType SpacingType;
typedef itk::ImageFileReader< ImageType > ReaderType;
typedef itk::ImageRegionConstIterator< ImageType > IteratorType;

float get_percentage(const std::string& inputFileName) {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFileName.c_str());
    
    try {
        reader->Update();
    } catch( itk::ExceptionObject & excp ) {
        std::cerr << "ERROR: caught ITK exception while reading image "
        << inputFileName << "." << std::endl;
        std::cerr << excp << std::endl;
        return 0;
    }
    
    IteratorType it( reader->GetOutput(),
                    reader->GetOutput()->GetLargestPossibleRegion() );
    it.GoToBegin();
    std::size_t counter = 0;
    std::size_t counterInTotal = 0;
    
    while ( !it.IsAtEnd() ) {
        ++counterInTotal;
        auto val = it.Value();
        //std::cout << val << std::endl;
        if( val > -1000) {
            counter++;
        }
        ++it;
    }
    return 100.0f * counter / counterInTotal;
}

int main(int argc, char *argv[]) {
    
    std::string base_dir = "/Users/yanxingwang/Projects/SFSU/CSC821/biomed/assets/SegmentationResults/ABD_LYMPH_001/";
    std::string file = "/segment/000382.dcm";
    
    #define NUM_SEGS 13
    std::string SEGS[] = {
        "heart",
        "left-kidney",
        "left-lung",
        "liver",
        "not-heart-top",
        "not-left-kidney",
        "not-liver-1",
        "not-liver-2",
        "not-right-kidney",
        "right-kidney",
        "right-lung",
        "space",
        "torso-n-stuff"
    };
    
    for(int i = 0; i < NUM_SEGS; ++i) {
        auto p = get_percentage(base_dir + SEGS[i] + file);
        std::cout << "Size percentage of " + SEGS[i] + ": " << p << "%" << std::endl;
    }

    return EXIT_SUCCESS;
    
} // end main