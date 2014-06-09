/* 
 * File:   itkOuterContourFilter.h
 * Author: Fabian
 *
 * Created on July 29, 2013, 4:06 PM
 */

#ifndef ITKOUTERCONTOURFILTER_H
#define	ITKOUTERCONTOURFILTER_H

#include <itkImageToImageFilter.h>

namespace itk {

/** \class OuterContourFilter. This class generates a conventional
 * histogram equalization.
 * 
 */
template<class TInputImage, class TMaskImage = Image<unsigned char, 
  ::itk::GetImageDimension<TInputImage>::ImageDimension>, 
  class TOutputImage = TInputImage>
class ITK_EXPORT OuterContourFilter :
    public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef OuterContourFilter                      Self;
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
  itkTypeMacro( OuterContourFilter,
                ImageToImageFilter );
  
  itkSetMacro( ContourValue, int );
  itkGetConstMacro( ContourValue, int );

  itkSetMacro( Direction, int );
  itkGetConstMacro( Direction, int );

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

protected:
  OuterContourFilter();
  ~OuterContourFilter() {};
  void PrintSelf( std::ostream& os, Indent indent ) const;

  void GenerateData();

private:
  OuterContourFilter( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  typename MaskImageType::Pointer        m_MaskImage;
  int								     m_ContourValue;
  int								     m_Direction;


}; // end of class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkOuterContourFilter.txx"
#endif

#endif	/* ITKOuterContourFILTER_H */

