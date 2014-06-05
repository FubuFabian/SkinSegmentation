/* 
 * File:   SkinSegmentation.h
 * Author: Fabian
 *
 * Created on June 4, 2014, 4:14 PM
 */

#ifndef SKINIMAGESEGMENTATION_H
#define	SKINImageSEGMENTATION_H

#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include <itkImage.h>
#include <itkCastImageFilter.h>

class SkinImageSegmentation
{
    typedef itk::Image<unsigned char, 2> ImageType;    
    typedef itk::Image<float, 2> FloatImageType;
    typedef itk::CastImageFilter<FloatImageType, ImageType> CastFilterType;
    
public:
    
    
    /**
     * \brief Class constructor
     */
    static SkinImageSegmentation *New()
	{
		return new SkinImageSegmentation;
	}
    
    /**
     * \brief Class destructor
     */
    ~SkinImageSegmentation()
	{
		delete this;
	}
    
    /**
        * \brief set the ultrasound Image 
        */
    void setImage(vtkSmartPointer<vtkImageData> image);
    
    /**
     * \brief compute the surface image 1 highest probablity
     */
    void computeSurfaceImage();
    
    /**
     * \brief compute the threshold image 1 highest probablity
     */
    void computeThresholdImage(float,float);
        
    /**
        * \brief computes the region growing algortithm on the probability image
        * to obtain the tumor region 
        */
    void computeRegionGrowing();
    
    /**
        * \brief set the seed for the region growing algorithm 
        * [in] index of the pixel
        */
    void setSeed(ImageType::IndexType seedPoint);
    
    /**
        * \brief returns the probability image 
        */
    vtkSmartPointer<vtkImageData> getSurfaceImage();
                
    /**
        * \brief returns the region growing image 
        */
    vtkSmartPointer<vtkImageData> getThresholdImage();
    
    /**
        * \brief returns the region growing image 
        */
    vtkSmartPointer<vtkImageData> getSegmentedImage();

    /**
        * \brief returns the region growing image 
        */
    vtkSmartPointer<vtkImageData> getRegionGrowingImage();
    
     /**
        * \brief returns the contour image 
        */
    vtkSmartPointer<vtkImageData> getContourImage();
    
    /**
        * \brief returns the coordinates of the tumor contour 
        */
    std::vector<ImageType::IndexType> getContourPixels();
    
private:
    
    FloatImageType::Pointer itkImage; ///< the itk ultrasound image

    vtkSmartPointer<vtkImageData> vtkImage; ///<  the vtk ultrasound image
    
    vtkSmartPointer<vtkImageData> vtkSurfaceImage; ///<the vtk surface image

    vtkSmartPointer<vtkImageData> vtkThresholdOverlayImage; ///< the vtk region growing image
    
    vtkSmartPointer<vtkImageData> vtkSegmentedImage; ///< the vtk region growing image
    
    vtkSmartPointer<vtkImageData> vtkRegionGrowingImage; ///< the vtk region growing image
    
    vtkSmartPointer<vtkImageData> vtkContourImage; ///< the vtk tumor contour image
    
    std::vector<ImageType::IndexType> contourPixels; ///< the tumor contour pixels coordinates
    
    ImageType::IndexType seedPoint; ///< the region growing seed point
    
    FloatImageType::Pointer surfaceImage; ///< the itk surface image
    
    FloatImageType::Pointer thresholdImage; ///< the itk threshold image
    
    FloatImageType::Pointer thresholdOverlayImage; ///< the itk threshold overlay image
    
    FloatImageType::Pointer segmentedImage;
    
    /**
    * \brief convert a vtk image to an itk image
    * [out] an itk image
    */
    FloatImageType::Pointer convertToITKImage(vtkSmartPointer<vtkImageData> vtkImage);
    
    /**
    * \brief convert an itk image to a vtk image
    * [out] a vtk image
    */
    vtkSmartPointer<vtkImageData> convertToVTKImage(FloatImageType::Pointer itkImage);
};


#endif	/* SKINImageSEGMENTATION_H */

