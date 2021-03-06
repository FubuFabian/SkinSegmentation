#include "SkinImageSegmentation.h"
#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkSurfaceEnhancementImageFilter.h"
#include "itkSobelEdgeDetectionImageFilter5.h"
#include "itkOuterContourFilter.h"

#include <itkImageRegionIterator.h>
#include <itkImageDuplicator.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkConnectedThresholdImageFilter.h>
#include <itkNeighborhoodIterator.h>
#include <itkGrayscaleFillholeImageFilter.h>
#include <itkHessianToObjectnessMeasureImageFilter.h>
#include <itkMultiScaleHessianBasedMeasureImageFilter.h>
#include <itkSymmetricSecondRankTensor.h>
#include <itkGradientAnisotropicDiffusionImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkMaximumImageFilter.h>
#include <itkSobelEdgeDetectionImageFilter.h>
#include <itkCannyEdgeDetectionImageFilter.h>
#include <itkLaplacianRecursiveGaussianImageFilter.h>
#include <itkRGBPixel.h>

#include <vtkImageLuminance.h>

void SkinImageSegmentation::computeSurfaceImage(int sigmaSize)
{
    
    itkImage = this->convertToITKImage(vtkImage); 
    
    typedef itk::GradientAnisotropicDiffusionImageFilter<FloatImageType,FloatImageType> AnisotropicFilterType;
    
    float timeStep = 0.125;

    AnisotropicFilterType::Pointer anisotropicFilter = AnisotropicFilterType::New();
    anisotropicFilter->SetInput(itkImage);
    anisotropicFilter->SetNumberOfIterations(25);
    anisotropicFilter->SetTimeStep(timeStep);
    anisotropicFilter->SetConductanceParameter(3);
    anisotropicFilter->Update();
    
/////////////////       SOBEL       ////////////////////////
    
//    typedef itk::SobelEdgeDetectionImageFilter5<FloatImageType, FloatImageType> SobelEdgeDetectionImageFilterType5;
//    SobelEdgeDetectionImageFilterType5::Pointer surfaceFilter = SobelEdgeDetectionImageFilterType5::New();
//    surfaceFilter->SetInput(anisotropicFilter->GetOutput());
//    surfaceFilter->Update();
    
///////////////////////////////////////////////////////////
    
/////////////////       CANNY       ////////////////////////
    
//    typedef itk::CannyEdgeDetectionImageFilter<FloatImageType,FloatImageType> CannyFilterType;
//    CannyFilterType::Pointer surfaceFilter = CannyFilterType::New();
//    
//    surfaceFilter->SetInput(anisotropicFilter->GetOutput());
//    surfaceFilter->SetVariance(sigmaSize);
//    surfaceFilter->SetUpperThreshold(15);
//    surfaceFilter->SetLowerThreshold(1);
//    surfaceFilter->Update();
    
//////////////////////////////////////////////////////////// 
    
    typedef itk::LaplacianRecursiveGaussianImageFilter<FloatImageType,FloatImageType >  LogFilterType;
    LogFilterType::Pointer surfaceFilter = LogFilterType::New();
      
    surfaceFilter->SetInput(anisotropicFilter->GetOutput());
    surfaceFilter->SetNormalizeAcrossScale( false );
    surfaceFilter->SetSigma(sigmaSize);
    surfaceFilter->Update();
    
/////////////////       SATO        ////////////////////////
    
//    typedef itk::SurfaceEnhancementImageFilter<FloatImageType, FloatImageType> SurfaceFilterType;
//    SurfaceFilterType::Pointer surfaceFilter = SurfaceFilterType::New();
//    
//    surfaceFilter->SetInput(anisotropicFilter->GetOutput());
//    surfaceFilter->SetMinScale(1);
//    surfaceFilter->SetMaxScale(sigmaSize);
//    surfaceFilter->SetAlpha(1);
//    surfaceFilter->SetGamma(0.1);
//    surfaceFilter->SetScaleStep(1);
//   
//    surfaceFilter->Update();

////////////////////////////////////////////////////////////////////////////////////////////////

////////////////        FRANGI      //////////////////////
    
//    typedef itk::SymmetricSecondRankTensor<float,2> TensorPixelType;
//
//    // Image Types
//    typedef itk::Image<TensorPixelType,2 > TensorImageType;
//    
//    typedef itk::HessianToObjectnessMeasureImageFilter<TensorImageType, FloatImageType > ObjectnessFilterType;
//    ObjectnessFilterType::Pointer objectnessFilter = ObjectnessFilterType::New();
//    
//    float alpha = .01;
//    float beta = 10;
//    float gamma = 10;
//
//    objectnessFilter->SetScaleObjectnessMeasure(true);
//    objectnessFilter->SetBrightObject(true);
//    
//    objectnessFilter->SetAlpha(alpha);
//    objectnessFilter->SetBeta(beta);
//    objectnessFilter->SetGamma(gamma);
//    
//    objectnessFilter->SetObjectDimension(1);
//    //objectnessFilter->SetObjectDimension(1);
//    //objectnessFilter->SetObjectDimension(0);
//
//
//    typedef itk::MultiScaleHessianBasedMeasureImageFilter<FloatImageType, TensorImageType, FloatImageType > MultiScaleHessianFilterType;
//    MultiScaleHessianFilterType::Pointer surfaceFilter = MultiScaleHessianFilterType::New();
//
//    surfaceFilter->SetInput(anisotropicFilter->GetOutput());
//    surfaceFilter->SetHessianToMeasureFilter(objectnessFilter);
//    surfaceFilter->SetSigmaStepMethodToEquispaced();
//    surfaceFilter->SetSigmaMinimum(1);
//    surfaceFilter->SetSigmaMaximum(sigmaSize);
//    surfaceFilter->SetNumberOfSigmaSteps(sigmaSize);
//    surfaceFilter->Update();   
    
////////////////////////////////////////////////////////////////////////////////////////////////

    
    typedef itk::RescaleIntensityImageFilter<FloatImageType,FloatImageType> RescaleFilterType;
    
    RescaleFilterType::Pointer normalizeFilter = RescaleFilterType::New();
    normalizeFilter->SetOutputMaximum(255);
    normalizeFilter->SetOutputMinimum(0);
    normalizeFilter->SetInput(surfaceFilter->GetOutput());
    normalizeFilter->Update(); 
    
    this->surfaceImage = normalizeFilter->GetOutput();
    
    this->vtkSurfaceImage = this->convertToVTKImage(surfaceImage);
}

void SkinImageSegmentation::computeThresholdImage(float lowerThreshold, float upperThreshold)
{
    
    typedef itk::BinaryThresholdImageFilter <FloatImageType, FloatImageType> BinaryThresholdImageFilterType;

    BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
    thresholdFilter->SetInput(surfaceImage);
    thresholdFilter->SetLowerThreshold(lowerThreshold);
    thresholdFilter->SetUpperThreshold(upperThreshold);
    thresholdFilter->SetInsideValue(255);
    thresholdFilter->SetOutsideValue(0);
    thresholdFilter->Update();
    
    this->thresholdImage = thresholdFilter->GetOutput();
    
    typedef itk::MaximumImageFilter <FloatImageType> MaximumImageFilterType;
 
    MaximumImageFilterType::Pointer maximumImageFilter = MaximumImageFilterType::New ();
    maximumImageFilter->SetInput(0, itkImage);
    maximumImageFilter->SetInput(1, thresholdImage);
    maximumImageFilter->Update();
    
    FloatImageType::Pointer thresholdOverlayImage = maximumImageFilter->GetOutput();
    
    this->vtkThresholdOverlayImage = this->convertToVTKImage(thresholdOverlayImage);
}

void SkinImageSegmentation::computeRegionGrowing()
{ 
    this->regionGrowingImage = FloatImageType::New();
    
    FloatImageType::RegionType region;
    region = thresholdImage->GetLargestPossibleRegion();
    
    regionGrowingImage->SetRegions(region);
    regionGrowingImage->Allocate();
    
    for(int i=0;i<seedPoints.size();i++){
        
      ImageType::IndexType seedPoint = seedPoints.at(i);
      
      typedef itk::ConnectedThresholdImageFilter<FloatImageType,FloatImageType> RegionGrowingType;
      RegionGrowingType::Pointer regionGrowing = RegionGrowingType::New();
      
      regionGrowing->SetInput(thresholdImage);
    
      FloatImageType::SizeType imageSize;
      
      imageSize = thresholdImage->GetLargestPossibleRegion().GetSize();
    
      std::cout<<"Seed: "<<seedPoint[0]<<","<<seedPoint[1]<<","<<thresholdImage->GetPixel(seedPoint)<<std::endl;
      
      regionGrowing->SetSeed(seedPoint);
      regionGrowing->SetReplaceValue(255);
      regionGrowing->SetUpper(255);
      regionGrowing->SetLower(255);
      regionGrowing->Update();
      
      typedef itk::GrayscaleFillholeImageFilter<FloatImageType,FloatImageType> FillHolesType;
      FillHolesType::Pointer fillHoles = FillHolesType::New();
      fillHoles->SetInput(regionGrowing->GetOutput());
      fillHoles->Update();
      
      typedef itk::MaximumImageFilter<FloatImageType> MaximumImageFilterType;
      
      MaximumImageFilterType::Pointer maximumImageFilter = MaximumImageFilterType::New ();
      maximumImageFilter->SetInput(0, fillHoles->GetOutput());
      maximumImageFilter->SetInput(1, regionGrowingImage);
      maximumImageFilter->Update();
    
      this->regionGrowingImage = maximumImageFilter->GetOutput();    
   
    }
    
    this->vtkRegionGrowingImage = this->convertToVTKImage(regionGrowingImage);
}

void SkinImageSegmentation::computeOuterContour()
{
    this->contourPixels.clear(); 
    
    typedef itk::OuterContourFilter<FloatImageType,FloatImageType> OuterContourType;
    OuterContourType::Pointer outerContourFilter = OuterContourType::New();

    outerContourFilter->SetInput(regionGrowingImage);
    outerContourFilter->SetDirection(0);
    outerContourFilter->Update();
    
    FloatImageType::Pointer contourImageX = outerContourFilter->GetOutput();
    
    outerContourFilter->SetDirection(1);
    outerContourFilter->Update();
    
    FloatImageType::Pointer contourImageY = outerContourFilter->GetOutput();
    
    typedef itk::MaximumImageFilter<FloatImageType> MaximumImageFilterType;
      
    MaximumImageFilterType::Pointer maximumImageFilter = MaximumImageFilterType::New ();
    maximumImageFilter->SetInput(0, contourImageY);
    maximumImageFilter->SetInput(1, contourImageX);
    maximumImageFilter->Update();
    
    this->contourImage = maximumImageFilter->GetOutput();
    
    this->vtkContourImage = this->convertToVTKImage(contourImage);
    
    maximumImageFilter->SetInput(0, contourImage);
    maximumImageFilter->SetInput(1, itkImage);
    maximumImageFilter->Update();
    
    FloatImageType::Pointer contourOverlayImage = maximumImageFilter->GetOutput();
    
    this->vtkContourOverlayImage = this->convertToVTKImage(contourOverlayImage);

    typedef itk::ImageRegionIterator<FloatImageType>  FloatImageRegionIteratorType;
    
    FloatImageRegionIteratorType itContour(contourImage, 
          contourImage->GetLargestPossibleRegion());
    itContour.GoToBegin();

    FloatImageType::IndexType pixel;
    
    while(!itContour.IsAtEnd())
    {
        if(itContour.Value()!=0){
            pixel[0] = itContour.GetIndex()[0];
            pixel[1] = itContour.GetIndex()[1];
            
            this->contourPixels.push_back(pixel);
        }
        
        ++itContour;
    } 
}

typedef itk::Image<float, 2> FloatImageType;
FloatImageType::Pointer SkinImageSegmentation::convertToITKImage(vtkSmartPointer<vtkImageData> vtkImage)
{
    int numberOfScalars = vtkImage->GetNumberOfScalarComponents();
    
    typedef itk::VTKImageToImageFilter<ImageType> ITKConverterType;
    ITKConverterType::Pointer itkConverter = ITKConverterType::New();
    
    if(numberOfScalars==3){
        vtkSmartPointer<vtkImageLuminance> imageLuminance = vtkSmartPointer<vtkImageLuminance>::New();
        imageLuminance->SetInput(vtkImage);
        imageLuminance->Update();
        itkConverter->SetInput(imageLuminance->GetOutput());
    }else{
        itkConverter->SetInput(vtkImage);
    }

    itkConverter->Update();
    
    typedef itk::ImageDuplicator<ImageType> DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage(itkConverter->GetOutput());
    duplicator->Update();
        
    typedef itk::CastImageFilter<ImageType,FloatImageType> CastImageType;
    CastImageType::Pointer castImage = CastImageType::New();
    castImage->SetInput(duplicator->GetOutput());
    castImage->Update();
    
    return castImage->GetOutput();    
}

vtkSmartPointer<vtkImageData> SkinImageSegmentation::convertToVTKImage(FloatImageType::Pointer itkImage)
{  
    CastFilterType::Pointer castProbabilityFilter = CastFilterType::New();
    castProbabilityFilter->SetInput(itkImage);
    castProbabilityFilter->Update(); 
    
    typedef itk::ImageToVTKImageFilter<ImageType> VTKConverterType;
    VTKConverterType::Pointer vtkConverter = VTKConverterType::New();
    vtkConverter->SetInput(castProbabilityFilter->GetOutput());
    vtkConverter->Update();
    
    vtkSmartPointer<vtkImageData> tempImage = vtkSmartPointer<vtkImageData>::New();
    tempImage->DeepCopy(vtkConverter->GetOutput());
    
    return tempImage;
}

void SkinImageSegmentation::setImage(vtkSmartPointer<vtkImageData> image)
{
    this->vtkImage = image;
}

void SkinImageSegmentation::setSeeds(std::vector<ImageType::IndexType> seedPoints)
{
	this->seedPoints = seedPoints;
}

vtkSmartPointer<vtkImageData> SkinImageSegmentation::getSurfaceImage()
{
	return this->vtkSurfaceImage;
}

vtkSmartPointer<vtkImageData> SkinImageSegmentation::getThresholdImage()
{
	return this->vtkThresholdOverlayImage;
}

vtkSmartPointer<vtkImageData> SkinImageSegmentation::getSegmentedImage()
{
	return this->vtkSegmentedImage;
}

vtkSmartPointer<vtkImageData> SkinImageSegmentation::getRegionGrowingImage()
{
	return this->vtkRegionGrowingImage;
}

vtkSmartPointer<vtkImageData> SkinImageSegmentation::getContourImage()
{
	return this->vtkContourOverlayImage;
}

typedef itk::Image<unsigned char, 2> ImageType;
std::vector<ImageType::IndexType> SkinImageSegmentation::getContourPixels()
{
	return this->contourPixels;
}
