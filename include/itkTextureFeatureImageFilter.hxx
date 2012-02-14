/*=========================================================================
 *
 *  Copyright Bradley Lowekamp
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkTextureFeatureImageFilter_hxx
#define __itkTextureFeatureImageFilter_hxx

#include "itkTextureFeatureImageFilter.h"

#include "vnl/vnl_math.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkProgressReporter.h"
#include "itkHistogramToTextureFeaturesFilter.h"

namespace itk
{
namespace External
{
template< class TImageType, class TOutputImage>
TextureFeatureImageFilter< TImageType, TOutputImage>
::TextureFeatureImageFilter()
{

  this->m_Min = NumericTraits< PixelType >::NonpositiveMin();
  this->m_Max = NumericTraits< PixelType >::max();

  this->m_NumberOfBinsPerAxis = DefaultBinsPerAxis;
  this->m_Normalize = false;

  m_WindowSize.Fill( 10 );
}

template< class TImageType, class TOutputImage>
void
TextureFeatureImageFilter< TImageType, TOutputImage >
::SetOffset(const OffsetType &offset)
{
  OffsetVectorType offsetVector;

  offsetVector.push_back(offset);
  this->SetOffsets(offsetVector);
}

template< class TImageType, class TOutputImage>
void
TextureFeatureImageFilter< TImageType, TOutputImage>
::SetPixelValueMinMax(PixelType min, PixelType max)
{
  itkDebugMacro("setting Min to " << min << "and Max to " << max);
  m_Min = min;
  m_Max = max;
  this->Modified();
}

template< class TImageType, class TOutputImage>
void
TextureFeatureImageFilter< TImageType, TOutputImage>
::GenerateOutputInformation()
{
  // this methods is overloaded so that if the output image is a
  // VectorImage then the correct number of components are set.

  Superclass::GenerateOutputInformation();
  OutputImageType* output = this->GetOutput();

  if ( !output )
    {
    return;
    }
  if ( output->GetNumberOfComponentsPerPixel() != this->GetNumberOfOutputComponents() )
    {
    output->SetNumberOfComponentsPerPixel( this->GetNumberOfOutputComponents() );
    }
}

template< class TImageType, class TOutputImage>
void
TextureFeatureImageFilter< TImageType, TOutputImage>
::BeforeThreadedGenerateData( )
{

  m_CooccurenceOffsetVector.clear();

  OffsetType o1;
  OffsetType o2;

  // calculate all offsets pairs
  for( unsigned int i = 0; i < InputImageType::ImageDimension; ++i )
    {
    o1[i] = -m_WindowSize[i];
    }


  // iterate over all point in the window
  do
    {
    typename OffsetVectorType::const_iterator iter;
    for( iter = this->m_Offsets.begin(); iter != this->m_Offsets.end(); ++iter )
      {
      o2 = o1 + *iter;

      bool ok = true;
      for( unsigned int i = 0; i < InputImageType::ImageDimension; ++i )
        {
        if ( vcl_abs(o2[i]) > m_WindowSize[i])
          {
          ok = false;
          break;
          }
        }

      if ( ok )
        {
        m_CooccurenceOffsetVector.push_back( std::make_pair( o1, o2 ) );

        std::cout << "offset: " << o1 << " " << o2 << std::endl;
        }
      }

    // increment the offset 1
    ++o1[0];
    for( unsigned int i = 1; i < InputImageType::ImageDimension; ++i )
      {
      if (o1[i-1] > static_cast<typename OffsetType::OffsetValueType>(m_WindowSize[i-1]) )
        {
        o1[i-1] = -m_WindowSize[i-1];
        ++o1[i];
        }
      }
    }  while ( o1[InputImageType::ImageDimension-1] <= static_cast<typename OffsetType::OffsetValueType>(m_WindowSize[InputImageType::ImageDimension-1])  );


}
template< class TImageType, class TOutputImage>
void
TextureFeatureImageFilter< TImageType, TOutputImage>
::ThreadedGenerateData(const RegionType& outputRegionForThread,
                            ThreadIdType threadId )
{
  const InputImageType *input = this->GetInput();
  OutputImageType *output = this->GetOutput();

  // constant for a coocurrence matrix.
  const unsigned int measurementVectorSize = 2;


  //
  // Create Histogram
  //
  typename HistogramType::Pointer histogram = HistogramType::New();

  histogram->SetMeasurementVectorSize(measurementVectorSize);


  //initialize parameters
  MeasurementVectorType lowerBound(measurementVectorSize);
  MeasurementVectorType upperBound(measurementVectorSize);

  lowerBound.Fill( this->GetMin() );
  upperBound.Fill( this->GetMax() +1 );


  // First, create an appropriate histogram with the right number of bins
  // and mins and maxs correct for the image type.
  typename HistogramType::SizeType size( histogram->GetMeasurementVectorSize() );
  size.Fill(m_NumberOfBinsPerAxis);
  histogram->Initialize(size, lowerBound, upperBound);

  typedef Statistics::HistogramToTextureFeaturesFilter<HistogramType> FeatureFilterType;
  typename FeatureFilterType::Pointer featureFilter = FeatureFilterType::New();


  // support progress methods/callbacks
  ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());


  typedef ImageRegionIterator<OutputImageType>             OutputIterator;
  typedef itk::ConstNeighborhoodIterator< InputImageType > NeighborhoodIteratorType;
  typename NeighborhoodIteratorType::RadiusType radius = m_WindowSize;

  typedef itk::NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< InputImageType > FaceCalculatorType;
  FaceCalculatorType faceCalculator;
  typename FaceCalculatorType::FaceListType faceList;
  typename FaceCalculatorType::FaceListType::iterator fit;

  faceList = faceCalculator( input, outputRegionForThread, radius );

  for ( fit = faceList.begin(); fit != faceList.end(); ++fit )
    {
    NeighborhoodIteratorType nIter( radius, input, *fit );
    OutputIterator           outIter(output, *fit);

    OutputPixelType out;
    NumericTraits<OutputPixelType>::SetLength( out, this->GetNumberOfOutputComponents() );

    while( !nIter.IsAtEnd() )
      {
      this->FillHistogram( *histogram, nIter );

      featureFilter->SetInput( histogram );
      featureFilter->Modified();
      featureFilter->Update();

      out[0] = featureFilter->GetEnergy();
      out[1] = featureFilter->GetEntropy();
      out[2] = featureFilter->GetCorrelation();
      out[3] = featureFilter->GetInverseDifferenceMoment();
      out[4] = featureFilter->GetInertia();
      out[5] = featureFilter->GetClusterShade();
      out[6] = featureFilter->GetClusterProminence();
      out[7] = featureFilter->GetHaralickCorrelation();

      outIter.Set( out );

      histogram->SetToZero();
      ++nIter;
      ++outIter;
      progress.CompletedPixel();
      }
    }


}


template< class TImageType, class TOutputImage>
void
TextureFeatureImageFilter< TImageType, TOutputImage>
::FillHistogram( HistogramType &histogram, const itk::ConstNeighborhoodIterator< InputImageType > &niter ) const
{


  MeasurementVectorType cooccur( histogram.GetMeasurementVectorSize() );

  typename std::vector<std::pair< OffsetType, OffsetType > >::const_iterator iter = m_CooccurenceOffsetVector.begin();
  while( iter != m_CooccurenceOffsetVector.end() )
    {

    const PixelType p1 = niter.GetPixel(iter->first);
    const PixelType p2 = niter.GetPixel(iter->second);

    if (    p1 >= m_Min
         && p2 >= m_Min
         && p1 <= m_Max
         && p2 <= m_Max )
      {
      cooccur[0] = p1;
      cooccur[1] = p2;
      histogram.IncreaseFrequencyOfMeasurement(cooccur, 1.0 );

      cooccur[1] = p1;
      cooccur[0] = p2;
      histogram.IncreaseFrequencyOfMeasurement(cooccur, 1.0 );
      }

    ++iter;
    }

}

template< class TImageType, class TOutputImage>
void
TextureFeatureImageFilter< TImageType, TOutputImage>
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

//fixme
//  os << indent << "Offsets: " << this->GetOffsets() << std::endl;
  os << indent << "Min: " << this->GetMin() << std::endl;
  os << indent << "Max: " << this->GetMax() << std::endl;
  os << indent << "NumberOfBinsPerAxis: " << this->GetNumberOfBinsPerAxis() << std::endl;
  os << indent << "Normalize: " << this->GetNormalize() << std::endl;
}
} // end of namespace External
} // end of namespace itk

#endif
