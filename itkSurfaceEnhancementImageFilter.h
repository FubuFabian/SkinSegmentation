/* 
 * File:   itkSurfaceEnhancementImageFilter.h
 * Author: Fabian
 *
 * Created on July 29, 2013, 4:06 PM
 */

#ifndef ITKSurfaceEnhancementIMAGEFILTER_H
#define	ITKSurfaceEnhancementIMAGEFILTER_H

#include "itkImageToImageFilter.h"
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>
#include <itkHessianRecursiveGaussianImageFilter.h>
#include <itkSymmetricEigenAnalysis.h>
#include <itkSymmetricEigenAnalysisImageFilter.h> 
#include <itkSymmetricSecondRankTensor.h> 

namespace itk {

/** \class SurfaceEnhancementImageFilter. This class generates a texture image, 
 * by processing the texture value of each pixel using 
 * itkScalarImageToCooccurrenceMatrixFilter.h and itkHistogramToTextureFeaturesFilter.h
 * classes.  
 */
template<class TInputImage, class TMaskImage = Image<unsigned char, 
  ::itk::GetImageDimension<TInputImage>::ImageDimension>, 
  class TOutputImage = TInputImage>
class ITK_EXPORT SurfaceEnhancementImageFilter :
    public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef SurfaceEnhancementImageFilter                      Self;
  typedef ImageToImageFilter<TInputImage, TOutputImage>   Superclass;
  typedef SmartPointer<Self>                              Pointer;
  typedef SmartPointer<const Self>                        ConstPointer;

  /** Standard New method. */
  itkNewMacro( Self );

  /** ImageDimension constants */
  itkStaticConstMacro( ImageDimension, unsigned int,
                       TInputImage::ImageDimension );

  /** Some convenient typedefs. */
  typedef float                                   RealType;
  typedef TInputImage                             InputImageType;
  typedef TMaskImage                              MaskImageType;
  typedef TOutputImage                            OutputImageType;

  /** Runtime information support. */
  itkTypeMacro( SurfaceEnhancementImageFilter,
                ImageToImageFilter );

  void SetMaskImage( const MaskImageType *mask )
    {
    this->SetNthInput( 1, const_cast<MaskImageType *>( mask ) ); 
    }
  const MaskImageType* GetMaskImage() const
    {
    return static_cast<MaskImageType*>( const_cast<DataObject *>
      ( this->ProcessObject::GetInput( 1 ) ) ); 
    }  
  void SetInput1( const TInputImage *input )
    {
    this->SetInput( input ); 
    }  
  void SetInput2( const TMaskImage *mask )
    {
    this->SetMaskImage( mask );
    }  
  void SetMaxScale(float maxScale)
    {
      this->m_MaxScale = maxScale;
    }
  void SetMinScale(float minScale)
    {
      this->m_MinScale = minScale;
    }
  void SetScaleStep(float scaleStep)
    {
      this->m_ScaleStep = scaleStep;
    }
  void SetAlpha(float alpha)
    {
      this->m_Alpha = alpha;
    }
  void SetGamma(float gamma)
    {
      this->m_Gamma = gamma;
    }
  
//  void SetImageDimension(unsigned int dimension)
//    {
//      this->m_Dimension = dimension;
//    }
  
  
  
  typedef ImageRegionConstIterator<InputImageType>
  ImageRegionIteratorType;
  typedef typename ImageRegionIteratorType::SizeType RegionSizeType;
  
  itkSetMacro( RegionSize, RegionSizeType );
  itkGetConstMacro( RegionSize, RegionSizeType );

protected:
  SurfaceEnhancementImageFilter();
  ~SurfaceEnhancementImageFilter() {};
  void PrintSelf( std::ostream& os, Indent indent ) const;

  void GenerateData();
  
  

private:
  SurfaceEnhancementImageFilter( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented
  
  //virtual double Weight(double ls, double lt, double alfa, double gamma) const;

  RegionSizeType                       m_RegionSize;
  typename MaskImageType::Pointer      m_MaskImage;
  float                                m_MinScale;
  float                                m_MaxScale;
  float                                m_ScaleStep;
  float                                m_Alpha;
  float                                m_Gamma;
//  const unsigned int                   m_Dimension;
  
  virtual vnl_matrix<float> EigenDecomposition(float sigma = 1);

  virtual vnl_vector<float> Surfaceness(vnl_matrix<float>,float);
  
  virtual float W(float,float,float,float);

}; // end of class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSurfaceEnhancementImageFilter.txx"
#endif

#endif	/* ITKSurfaceEnhancementIMAGEFILTER_H */

