/*
 * File:   SkinSegmentation.h
 * Author: Fabian
 *
 * Created on June 4, 2014, 4:14 PM
 */

#ifndef SKINVOLUMESEGMENTATION_H
#define	SKINVOLUMESEGMENTATION_H

#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include <itkImage.h>
#include <itkCastImageFilter.h>

class SkinVolumeSegmentation
{
    typedef itk::Image<unsigned char, 3> ImageType;
    typedef itk::Image<float, 3> FloatImageType;
    typedef itk::CastImageFilter<FloatImageType, ImageType> CastFilterType;

public:


    /**
     * \brief Class constructor
     */
    static SkinVolumeSegmentation *New()
        {
                return new SkinVolumeSegmentation;
        }

    /**
     * \brief Class destructor
     */
    ~SkinVolumeSegmentation()
        {
                delete this;
        }

    /**
        * \brief set the ultrasound Volume
        */
    void setVolume(vtkSmartPointer<vtkImageData> Volume);

    /**
     * \brief compute the surface Volume 1 highest probablity
     */
    void computeSurfaceVolume(int);

    /**
     * \brief compute the threshold Volume 1 highest probablity
     */
    void computeThresholdVolume(float,float);

    /**
        * \brief computes the region growing algortithm on the probability Volume
        * to obtain the tumor region
        */
    void computeRegionGrowing();

    /**
        * \brief computes the outer contour algortithm on the probability Volume
        * to obtain the tumor region
        */
    void computeOuterContour();

    /**
        * \brief set the seed for the region growing algorithm
        * [in] index of the pixel
        */
    void setSeeds(std::vector<ImageType::IndexType>);

    /**
        * \brief returns the probability Volume
        */
    vtkSmartPointer<vtkImageData> getSurfaceVolume();

    /**
        * \brief returns the region growing Volume
        */
    vtkSmartPointer<vtkImageData> getThresholdVolume();

    /**
        * \brief returns the region growing Volume
        */
    vtkSmartPointer<vtkImageData> getSegmentedVolume();

    /**
        * \brief returns the region growing Volume
        */
    vtkSmartPointer<vtkImageData> getRegionGrowingVolume();

     /**
        * \brief returns the contour Volume
        */
    vtkSmartPointer<vtkImageData> getContourVolume();

    /**
        * \brief returns the coordinates of the tumor contour
        */
    std::vector<ImageType::IndexType> getContourPixels();

private:

    FloatImageType::Pointer itkVolume; ///< the itk ultrasound Volume

    vtkSmartPointer<vtkImageData> vtkVolume; ///<  the vtk ultrasound Volume

    vtkSmartPointer<vtkImageData> vtkSurfaceVolume; ///<the vtk surface Volume

    vtkSmartPointer<vtkImageData> vtkThresholdOverlayVolume; ///< the vtk region growing Volume

    vtkSmartPointer<vtkImageData> vtkSegmentedVolume; ///< the vtk region growing Volume

    vtkSmartPointer<vtkImageData> vtkRegionGrowingVolume; ///< the vtk region growing Volume

    vtkSmartPointer<vtkImageData> vtkContourVolume; ///< the vtk tumor contour Volume

    vtkSmartPointer<vtkImageData> vtkContourOverlayVolume; ///< the vtk region growing Volume

    std::vector<FloatImageType::IndexType> contourPixels; ///< the tumor contour pixels coordinates

    std::vector<ImageType::IndexType> seedPoints; ///< the region growing seed point

    FloatImageType::Pointer surfaceVolume; ///< the itk surface Volume

    FloatImageType::Pointer thresholdVolume; ///< the itk threshold Volume

    FloatImageType::Pointer regionGrowingVolume; ///< the itk regionGrowing Volume

    FloatImageType::Pointer contourVolume;

    FloatImageType::Pointer segmentedVolume;

    /**
    * \brief convert a vtk Volume to an itk Volume
    * [out] an itk Volume
    */
    FloatImageType::Pointer convertToITKImage(vtkSmartPointer<vtkImageData> vtkImage);

    /**
    * \brief convert an itk image to a vtk image
    * [out] a vtk image
    */
    vtkSmartPointer<vtkImageData> convertToVTKImage(FloatImageType::Pointer itkImage);
};


#endif	/* SKINVOLUMESEGMENTATION_H */

