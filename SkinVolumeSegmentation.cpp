
#include "SkinVolumeSegmentation.h"
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

void SkinVolumeSegmentation::computeSurfaceVolume(int sigmaSize)
{

    itkVolume = this->convertToITKImage(vtkVolume);

    typedef itk::GradientAnisotropicDiffusionImageFilter<FloatImageType,FloatImageType> AnisotropicFilterType;

    float timeStep = 0.125;

    AnisotropicFilterType::Pointer anisotropicFilter = AnisotropicFilterType::New();
    anisotropicFilter->SetInput(itkVolume);
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

    this->surfaceVolume = normalizeFilter->GetOutput();

    this->vtkSurfaceVolume = this->convertToVTKImage(surfaceVolume);
}

void SkinVolumeSegmentation::computeThresholdVolume(float lowerThreshold, float upperThreshold)
{

    typedef itk::BinaryThresholdImageFilter <FloatImageType, FloatImageType> BinaryThresholdImageFilterType;

    BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
    thresholdFilter->SetInput(surfaceVolume);
    thresholdFilter->SetLowerThreshold(lowerThreshold);
    thresholdFilter->SetUpperThreshold(upperThreshold);
    thresholdFilter->SetInsideValue(255);
    thresholdFilter->SetOutsideValue(0);
    thresholdFilter->Update();

    this->thresholdVolume = thresholdFilter->GetOutput();

    typedef itk::MaximumImageFilter <FloatImageType> MaximumImageFilterType;

    MaximumImageFilterType::Pointer maximumVolumeFilter = MaximumImageFilterType::New ();
    maximumVolumeFilter->SetInput(0, itkVolume);
    maximumVolumeFilter->SetInput(1, thresholdVolume);
    maximumVolumeFilter->Update();

    FloatImageType::Pointer thresholdOverlayVolume = maximumVolumeFilter->GetOutput();

    this->vtkThresholdOverlayVolume = this->convertToVTKImage(thresholdOverlayVolume);
}

void SkinVolumeSegmentation::computeRegionGrowing()
{
    this->regionGrowingVolume = FloatImageType::New();

    FloatImageType::RegionType region;
    region = thresholdVolume->GetLargestPossibleRegion();

    regionGrowingVolume->SetRegions(region);
    regionGrowingVolume->Allocate();

    for(int i=0;i<seedPoints.size();i++){

      ImageType::IndexType seedPoint = seedPoints.at(i);

      typedef itk::ConnectedThresholdImageFilter<FloatImageType,FloatImageType> RegionGrowingType;
      RegionGrowingType::Pointer regionGrowing = RegionGrowingType::New();

      regionGrowing->SetInput(thresholdVolume);

      FloatImageType::SizeType volumeSize;
      volumeSize = thresholdVolume->GetLargestPossibleRegion().GetSize();

      seedPoint[1] = volumeSize[1] - seedPoint[1];

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

      MaximumImageFilterType::Pointer maximumVolumeFilter = MaximumImageFilterType::New ();
      maximumVolumeFilter->SetInput(0, fillHoles->GetOutput());
      maximumVolumeFilter->SetInput(1, regionGrowingVolume);
      maximumVolumeFilter->Update();

      this->regionGrowingVolume = maximumVolumeFilter->GetOutput();

    }

    this->vtkRegionGrowingVolume = this->convertToVTKImage(regionGrowingVolume);
}

void SkinVolumeSegmentation::computeOuterContour()
{
    this->contourPixels.clear();

    typedef itk::OuterContourFilter<FloatImageType,FloatImageType> OuterContourType;
    OuterContourType::Pointer outerContourFilter = OuterContourType::New();

    outerContourFilter->SetInput(regionGrowingVolume);
    outerContourFilter->SetDirection(0);
    outerContourFilter->Update();

    FloatImageType::Pointer contourVolumeX = outerContourFilter->GetOutput();

    outerContourFilter->SetDirection(1);
    outerContourFilter->Update();

    FloatImageType::Pointer contourVolumeY = outerContourFilter->GetOutput();

    typedef itk::MaximumImageFilter<FloatImageType> MaximumImageFilterType;

    MaximumImageFilterType::Pointer maximumVolumeFilter = MaximumImageFilterType::New ();
    maximumVolumeFilter->SetInput(0, contourVolumeY);
    maximumVolumeFilter->SetInput(1, contourVolumeX);
    maximumVolumeFilter->Update();

    this->contourVolume = maximumVolumeFilter->GetOutput();

    this->vtkContourVolume = this->convertToVTKImage(contourVolume);

    maximumVolumeFilter->SetInput(0, contourVolume);
    maximumVolumeFilter->SetInput(1, itkVolume);
    maximumVolumeFilter->Update();

    FloatImageType::Pointer contourOverlayVolume = maximumVolumeFilter->GetOutput();

    this->vtkContourOverlayVolume = this->convertToVTKImage(contourOverlayVolume);

    typedef itk::ImageRegionIterator<FloatImageType>  FloatImageRegionIteratorType;

    FloatImageRegionIteratorType itContour(contourVolume,
          contourVolume->GetLargestPossibleRegion());
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

typedef itk::Image<float, 3> FloatImageType;
FloatImageType::Pointer SkinVolumeSegmentation::convertToITKImage(vtkSmartPointer<vtkImageData> vtkImage)
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

vtkSmartPointer<vtkImageData> SkinVolumeSegmentation::convertToVTKImage(FloatImageType::Pointer itkImage)
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

void SkinVolumeSegmentation::setVolume(vtkSmartPointer<vtkImageData> volume)
{
    this->vtkVolume = volume;
}

void SkinVolumeSegmentation::setSeeds(std::vector<ImageType::IndexType> seedPoints)
{
        this->seedPoints = seedPoints;
}

vtkSmartPointer<vtkImageData> SkinVolumeSegmentation::getSurfaceVolume()
{
        return this->vtkSurfaceVolume;
}

vtkSmartPointer<vtkImageData> SkinVolumeSegmentation::getThresholdVolume()
{
        return this->vtkThresholdOverlayVolume;
}

vtkSmartPointer<vtkImageData> SkinVolumeSegmentation::getSegmentedVolume()
{
        return this->vtkSegmentedVolume;
}

vtkSmartPointer<vtkImageData> SkinVolumeSegmentation::getRegionGrowingVolume()
{
        return this->vtkRegionGrowingVolume;
}

vtkSmartPointer<vtkImageData> SkinVolumeSegmentation::getContourVolume()
{
        return this->vtkContourOverlayVolume;
}

typedef itk::Image<unsigned char, 3> ImageType;
std::vector<ImageType::IndexType> SkinVolumeSegmentation::getContourPixels()
{
        return this->contourPixels;
}
