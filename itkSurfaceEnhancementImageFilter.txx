/* 
 * File:   itkSurfaceEnhancementImageFilter.txx
 * Author: Fabian
 *
 * Created on July 29, 2013, 4:09 PM
 */

#ifndef ITKSurfaceEnhancementIMAGEFILTER_TXX
#define	ITKSurfaceEnhancementIMAGEFILTER_TXX

#include "itkSurfaceEnhancementImageFilter.h"

#include <itkMirrorPadImageFilter.h>
#include <itkImage.h>
#include <itkSmartPointer.h>

#include <vector>
#include <math.h>
#include <iostream>

namespace itk {

template <class TInputImage, class TMaskImage, class TOutputImage>
SurfaceEnhancementImageFilter<TInputImage, TMaskImage, TOutputImage>
::SurfaceEnhancementImageFilter()
{
    this->m_RegionSize.Fill( 13 );
    this->m_MaskImage = NULL;
    
    this->m_MinScale = 0;
    this->m_MaxScale = 0;
    
}

template<class TInputImage, class TMaskImage, class TOutputImage>
void
SurfaceEnhancementImageFilter<TInputImage, TMaskImage, TOutputImage>
::GenerateData()
{
    
  this->AllocateOutputs(); 
  
  ProgressReporter progress( this, 0, 
        this->GetInput()->GetRequestedRegion().GetNumberOfPixels(), 100 );
  
  int volumeSize = this->GetInput()->GetLargestPossibleRegion().GetNumberOfPixels();
  
  vnl_vector<float> surfaceness(volumeSize, 0);

  float sigma = m_MinScale;

  while (sigma <= m_MaxScale)
  {
      vnl_matrix<float> eigenvalues = this->EigenDecomposition(sigma);
      vnl_vector<float> currentSurfaceness = this->Surfaceness(eigenvalues, sigma);

      for (int i = 0; i < surfaceness.size(); i++)
      {

          if (surfaceness[i] < currentSurfaceness[i])
          {
              if (currentSurfaceness[i] <= 0)
              {
                  std::cout << "(" << surfaceness[i] << ", " << currentSurfaceness[i] << ") " << std::endl;
              }
                surfaceness.put(i, currentSurfaceness[i]);
          }
      }
      
      sigma += m_ScaleStep;

  }

  
  ImageRegionIteratorType itOut(this->GetOutput(), 
          this->GetOutput()->GetLargestPossibleRegion());
  itOut.GoToBegin();

  int idx = 0;
  while(!itOut.IsAtEnd()){

      this->GetOutput()->SetPixel(itOut.GetIndex(),surfaceness[idx]);
      ++itOut;
      ++idx;      
      progress.CompletedPixel();

  }
  
}

template<class TInputImage, class TMaskImage, class TOutputImage>
vnl_matrix<float>
SurfaceEnhancementImageFilter<TInputImage, TMaskImage, TOutputImage>
::EigenDecomposition(float sigma)
{
    
  typedef typename InputImageType::PixelType PixelType;
  typedef itk::Vector< PixelType, itk::GetImageDimension<InputImageType>::ImageDimension > VectorPixelType;
  typedef itk::Vector< VectorPixelType,  itk::GetImageDimension<InputImageType>::ImageDimension > MatrixPixelType;
  typedef itk::SymmetricSecondRankTensor< PixelType,  itk::GetImageDimension<InputImageType>::ImageDimension > TensorPixelType;
  
  typedef itk::Image< TensorPixelType,  itk::GetImageDimension<InputImageType>::ImageDimension > TensorImageType;
  
  typedef itk::HessianRecursiveGaussianImageFilter
            < InputImageType, TensorImageType > HessianFilterType;
  
  std::cout << "compute hessian matrix at scale = " << sigma << std::endl;

  typename HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
  hessianFilter->SetSigma(sigma);
  hessianFilter->SetInput(this->GetInput());
  hessianFilter->Update();
  typename TensorImageType::Pointer hessianImage = hessianFilter->GetOutput();

  // Eigen Analysis 
  typedef itk::SymmetricEigenAnalysis
            < TensorPixelType, VectorPixelType, MatrixPixelType > EigenAnalysisType;

  // Find Eigenvalues and corresponding Eigenvectors
  EigenAnalysisType eigenAnalysis;
  eigenAnalysis.SetDimension(itk::GetImageDimension<InputImageType>::ImageDimension);
  eigenAnalysis.SetOrderEigenValues(true);

  // Construct output images 
  std::cout << "calculate eigen analysis" << std::endl;
  typedef itk::ImageRegionConstIteratorWithIndex< TensorImageType > TensorIteratorType;
  TensorIteratorType hess_It(hessianImage, hessianImage->GetLargestPossibleRegion());

  typename TensorImageType::IndexType hessianIndex;
  typename TensorImageType::PointType hessianPoint;
  TensorPixelType hessianMatrix;

  VectorPixelType currentEigenValues;
  MatrixPixelType currentEigenVectors;
  
  int volumeSize = this->GetInput()->GetLargestPossibleRegion().GetNumberOfPixels();

  vnl_matrix<float> eigenvalues(volumeSize, 3);


  // calculating the hessian matrix
  int idx = 0;
  while (!hess_It.IsAtEnd())
  {
      hessianIndex = hess_It.GetIndex();
      hessianImage->TransformIndexToPhysicalPoint(hessianIndex, hessianPoint);

      hessianMatrix = hess_It.Get();
      eigenAnalysis.ComputeEigenValuesAndVectors(hessianMatrix, currentEigenValues, currentEigenVectors);

      eigenvalues.put(idx, 0, currentEigenValues[2]);
      eigenvalues.put(idx, 1, currentEigenValues[1]);
      eigenvalues.put(idx, 2, currentEigenValues[0]);

      ++idx;
      ++hess_It;
  }

  return eigenvalues;
}

template<class TInputImage, class TMaskImage, class TOutputImage>
vnl_vector<float>
SurfaceEnhancementImageFilter<TInputImage, TMaskImage, TOutputImage>
::Surfaceness(vnl_matrix<float> eigenvalues, float sigma)
{
    vnl_vector<float> surfaceness(eigenvalues.rows());

    for (int i = 0; i < surfaceness.size(); i++)
    {
        double l1 = eigenvalues[i][0];
        double l2 = eigenvalues[i][1];
        double l3 = eigenvalues[i][2];

        if (l3 < 0)
        {
            double s = std::pow(sigma, 2) * std::abs(l3) *
                    W(l2, l3, m_Alpha, m_Gamma) * W(l1, l2, m_Alpha, m_Gamma);
            surfaceness.put(i, s);
        }
    }
    return surfaceness;
}

template<class TInputImage, class TMaskImage, class TOutputImage>
float
SurfaceEnhancementImageFilter<TInputImage, TMaskImage, TOutputImage>
::W(float ls, float lt, float alfa, float gamma)
{
    float a_lt = std::abs(lt);

    if (lt <= ls)
    {
        return std::pow(1 + (ls / a_lt), gamma);
    }
    else if ((a_lt / alfa) > ls)
    {
        return std::pow(1 - ((alfa) * (ls / a_lt)), gamma);
    }
    else
    {
        return 0.0;
    }
}



template<class TInputImage, class TMaskImage, class TOutputImage>
void
SurfaceEnhancementImageFilter<TInputImage, TMaskImage, TOutputImage>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "Region size: "
    << this->m_RegionSize << std::endl;

  
}

}// end namespace itk

#endif	/* ITKSurfaceEnhancementIMAGEFILTER_TXX */

