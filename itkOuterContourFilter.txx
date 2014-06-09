/* 
 * File:   itkOuterContourFilter.txx
 * Author: Fabian
 *
 * Created on July 29, 2013, 4:09 PM
 */

#ifndef ITKOuterContourFILTER_TXX
#define	ITKOuterContourFILTER_TXX

#include "itkOuterContourFilter.h"
#include <itkImage.h>
#include <itkImageRegionConstIterator.h>

namespace itk {

template <class TInputImage, class TMaskImage, class TOutputImage>
OuterContourFilter<TInputImage, TMaskImage, TOutputImage>
::OuterContourFilter()
{
    this->m_MaskImage = NULL;
    this->m_ContourValue = 255;
    this->m_Direction = 0;
}

template<class TInputImage, class TMaskImage, class TOutputImage>
void
OuterContourFilter<TInputImage, TMaskImage, TOutputImage>
::GenerateData()
{
  this->AllocateOutputs();

  ProgressReporter progress( this, 0, 
        this->GetInput()->GetRequestedRegion().GetNumberOfPixels(), 100 );
  
  ImageType::SizeType imageSize;
  imageSize = this->GetInput()->GetLargestPossibleRegion().GetSize();
  
  if(::itk::GetImageDimension<InputImageType>::ImageDimension == 2)
	imageSize[2] = 1;
	
  bool firstFlag;
  firstFlag = false;
  
  bool lastFlag;
  lastFlag = false;
  
  std::vector<InputImageType::IndexType> contourPixels;

  switch(m_Direction){
  
  case 2:
  
	for(int x=0;x<imageSize[0];x++){
		for(int y=0;y<imageSize[1];y++){
			for(int z=0;z<imageSize[2];z++){
		
				InputImageType::IndexType pixelIndex;
				pixelIndex[0] = x;
				pixelIndex[1] = y;
				pixelIndex[2] = z;
				
				InputImageType::IndexType finalIndex;
				
				float pixelValue = this->GetInput()->GetPixel(pixelIndex);
				
				if((pixelValue==m_ContourValue)&&(firstFlag==false)){
					contourPixels.push_back(pixelIndex);
					firstFlag = true;				
				}else if((pixelValue == m_ContourValue)&&(firstFlag == true)){
					finalIndex = pixelIndex;
					lastFlag = true;
				}
				
				if((z==imageSize[2]-1)&&(lastFlag==true)){
					contourPixels.push_back(finalIndex);
					firstFlag = false;
					lastFlag = false;	
				}	
			}
		} 
	  }
	  break;
	
	case 1:
	
	  for(int x=0;x<imageSize[0];x++){
		for(int z=0;z<imageSize[2];z++){
			for(int y=0;y<imageSize[1];y++){
		
				InputImageType::IndexType pixelIndex;
				pixelIndex[0] = x;
				pixelIndex[1] = y;
				pixelIndex[2] = z;
				
				InputImageType::IndexType finalIndex;
				
				float pixelValue = this->GetInput()->GetPixel(pixelIndex);
				
				if((pixelValue==m_ContourValue)&&(firstFlag==false)){
					contourPixels.push_back(pixelIndex);
					firstFlag = true;				
				}else if((pixelValue == m_ContourValue)&&(firstFlag == true)){
					finalIndex = pixelIndex;
					lastFlag = true;
				}
				
				if((y==imageSize[1]-1)&&(lastFlag==true)){
					contourPixels.push_back(finalIndex);
					firstFlag = false;
					lastFlag = false;	
				}	
			}
		} 
      }	
	  
	  break;
	  
	case 0:
	
	  for(int y=0;y<imageSize[1];y++){
		for(int z=0;z<imageSize[2];z++){
			for(int x=0;x<imageSize[0];x++){
		
				InputImageType::IndexType pixelIndex;
				pixelIndex[0] = x;
				pixelIndex[1] = y;
				pixelIndex[2] = z;
				
				InputImageType::IndexType finalIndex;
				
				float pixelValue = this->GetInput()->GetPixel(pixelIndex);
				
				if((pixelValue==m_ContourValue)&&(firstFlag==false)){
					contourPixels.push_back(pixelIndex);
					firstFlag = true;				
				}else if((pixelValue == m_ContourValue)&&(firstFlag == true)){
					finalIndex = pixelIndex;
					lastFlag = true;
				}
				
				if((x==imageSize[0]-1)&&(lastFlag==true)){
					contourPixels.push_back(finalIndex);
					firstFlag = false;
					lastFlag = false;	
				}	
			}
		} 
	  }
	  
	  break;
	
	}

	std::cout<<"Contours Finished"<<std::endl;
	
	typedef ImageRegionConstIterator<InputImageType>  ImageRegionIteratorType;
	
	ImageRegionIteratorType itOut(this->GetOutput(),this->GetOutput()->GetLargestPossibleRegion());
    itOut.GoToBegin();
    
    while(!itOut.IsAtEnd())
    {
      this->GetOutput()->SetPixel(itOut.GetIndex(),0);
      ++itOut;
      progress.CompletedPixel();
    }      

	for(int i=0;i<contourPixels.size();i++){
		this->GetOutput()->SetPixel(contourPixels.at(i),255);   
	}
  
}

template<class TInputImage, class TMaskImage, class TOutputImage>
void
OuterContourFilter<TInputImage, TMaskImage, TOutputImage>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  
}

}// end namespace itk

#endif	/* ITKOuterContourFILTER_TXX */

