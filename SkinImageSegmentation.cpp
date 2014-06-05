#include "SkinImageSegmentation.h"
#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkSurfaceEnhancementImageFilter.h"

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
#include <itkRGBPixel.h>

#include <vtkImageLuminance.h>

void SkinImageSegmentation::computeSurfaceImage()
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
    
/////////////////       SATO        ////////////////////////
    
    typedef itk::SurfaceEnhancementImageFilter<FloatImageType, FloatImageType> SurfaceFilterType;
    SurfaceFilterType::Pointer surfaceFilter = SurfaceFilterType::New();
    
    surfaceFilter->SetInput(anisotropicFilter->GetOutput());
    surfaceFilter->SetMinScale(10);
    surfaceFilter->SetMaxScale(10);
    surfaceFilter->SetAlpha(1);
    surfaceFilter->SetGamma(0.1);
    surfaceFilter->SetScaleStep(1);
   
    surfaceFilter->Update();

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
//    MultiScaleHessianFilterType::Pointer multiScaleHessianFilterFilter = MultiScaleHessianFilterType::New();
//
//    multiScaleHessianFilterFilter->SetInput(anisotropicFilter->GetOutput());
//    multiScaleHessianFilterFilter->SetHessianToMeasureFilter(objectnessFilter);
//    multiScaleHessianFilterFilter->SetSigmaStepMethodToEquispaced();
//    multiScaleHessianFilterFilter->SetSigmaMinimum(10);
//    multiScaleHessianFilterFilter->SetSigmaMaximum(10);
//    multiScaleHessianFilterFilter->SetNumberOfSigmaSteps(1);
//    multiScaleHessianFilterFilter->Update();   
    
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
    
    this->thresholdOverlayImage = maximumImageFilter->GetOutput();
    
    this->vtkThresholdOverlayImage = this->convertToVTKImage(thresholdOverlayImage);
    
}

void SkinImageSegmentation::computeRegionGrowing()
{
//    this->contourPixels.clear();
//    
//    typedef itk::ConnectedThresholdImageFilter<FloatImageType,FloatImageType> RegionGrowingType;
//    RegionGrowingType::Pointer regionGrowing = RegionGrowingType::New();
//    
//    regionGrowing->SetInput(probabilityImage);
//    
//    FloatImageType::SizeType imageSize;
//    imageSize = probabilityImage->GetLargestPossibleRegion().GetSize();
//    
//    seedPoint[1] = imageSize[1] - seedPoint[1]; 
//    
//    regionGrowing->SetSeed(seedPoint);
//    regionGrowing->SetReplaceValue(255);
//    
//    typedef itk::NeighborhoodIterator<FloatImageType> NeighborhoodIterator;
//    NeighborhoodIterator::RadiusType radius;
//    radius.Fill(1);
//    
//    FloatImageType::RegionType seedRegion;
//    
//    FloatImageType::RegionType::IndexType seedRegionIndex;
//    seedRegionIndex[0] = seedPoint[0] - 1;
//    seedRegionIndex[1] = seedPoint[1] - 1;
//    seedRegion.SetIndex(seedRegionIndex);
//    
//    FloatImageType::RegionType::SizeType seedRegionSize;
//    seedRegionSize.Fill(3); 
//    
//    NeighborhoodIterator it(radius,probabilityImage,seedRegion);
//    
//    float accum = 0;
//    for (unsigned int i = 0; i < it.Size(); ++i){    
//        accum += it.GetPixel(i);  
//    }
//    
//    float seedRegionMean = accum/(float)it.Size();
//    
//    float upperThreshold = seedRegionMean + seedRegionMean*0.55;
//    float lowerThreshold = seedRegionMean - seedRegionMean*0.55;
//    
//    regionGrowing->SetUpper(upperThreshold);
//    regionGrowing->SetLower(lowerThreshold);
//
//    regionGrowing->Update();
//    
//    typedef itk::GrayscaleFillholeImageFilter<FloatImageType,FloatImageType> FillHolesType;
//    FillHolesType::Pointer fillHoles = FillHolesType::New();
//    fillHoles->SetInput(regionGrowing->GetOutput());
//    fillHoles->Update();
//    
//    this->vtkRegionGrowingImage = this->convertToVTKImage(fillHoles->GetOutput());
// 
//    typedef itk::BinaryContourImageFilter <FloatImageType, FloatImageType >
//                                                binaryContourImageFilterType;
// 
//    binaryContourImageFilterType::Pointer binaryContourFilter
//                                        = binaryContourImageFilterType::New ();
//    binaryContourFilter->SetInput(fillHoles->GetOutput());
//    binaryContourFilter->SetForegroundValue(255);
//    binaryContourFilter->SetBackgroundValue(0);
//    binaryContourFilter->SetFullyConnected(false);
//    binaryContourFilter->Update();
//    
//    
//    FloatImageType::Pointer contourImage = binaryContourFilter->GetOutput();
//    
//    typedef itk::ImageRegionIterator<FloatImageType>  FloatImageRegionIteratorType;
//    
//    FloatImageRegionIteratorType itContour(contourImage, 
//          contourImage->GetLargestPossibleRegion());
//    itContour.GoToBegin();
//    
//    ImageType::IndexType pixel;
//    
//    while(!itContour.IsAtEnd())
//    {
//        if(itContour.Value()!=0){
//            pixel[0] = itContour.GetIndex()[0];
//            pixel[1] = itContour.GetIndex()[1];
//            
//            this->contourPixels.push_back(pixel);
//        }
//        ++itContour;
//    }
//    
//    typedef itk::ImageDuplicator<FloatImageType> DuplicatorType;
//    DuplicatorType::Pointer duplicator = DuplicatorType::New();
//    duplicator->SetInputImage(this->itkImage);
//    duplicator->Update();
//    
//    FloatImageType::Pointer itkContourImage = duplicator->GetOutput();
//    
//    for(unsigned int i=0;i<contourPixels.size();i++){
//        itkContourImage->SetPixel(contourPixels.at(i),255);
//    }
//        
//    this->vtkContourImage = this->convertToVTKImage(itkContourImage);  
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

void SkinImageSegmentation::setSeed(ImageType::IndexType seedPoint)
{
	this->seedPoint = seedPoint;
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
	return this->vtkContourImage;
}

typedef itk::Image<unsigned char, 2> ImageType;
std::vector<ImageType::IndexType> SkinImageSegmentation::getContourPixels()
{
	return this->contourPixels;
}
