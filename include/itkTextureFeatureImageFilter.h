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
#ifndef __itkTextureFeatureImageFilter_h
#define __itkTextureFeatureImageFilter_h

#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkNumericTraits.h"
#include "itkHistogram.h"
#include "itkDenseFrequencyContainer2.h"

namespace itk
{
namespace External
{
/** \class TextureFeatureImageFilter
 *  \brief Computes Gray-Level Co-occurence Matrix texture feature per pixel
 *
 * \author Bradley Lowekamp
 *
 * \ingroup ITKTextureAnalysis
 */

template< class TInputImage, class TOutputImage>
class TextureFeatureImageFilter
  : public ImageToImageFilter< TInputImage, TOutputImage>
{
public:
  /** Standard typedefs */
  typedef TextureFeatureImageFilter                      Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage> Superclass;

  typedef SmartPointer< Self >                     Pointer;
  typedef SmartPointer< const Self >               ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(TextureFeatureImageFilter, ImageToImageFilter);

  /** standard New() method support */
  itkNewMacro(Self);

  typedef TInputImage                                  InputImageType;
  typedef TOutputImage                                 OutputImageType;
  typedef typename InputImageType::PixelType           PixelType;
  typedef typename InputImageType::RegionType          RegionType;
  typedef typename InputImageType::SizeType            RadiusType;
  typedef typename InputImageType::OffsetType          OffsetType;
  typedef std::vector< OffsetType >                    OffsetVectorType;
  typedef typename TOutputImage::PixelType             OutputPixelType;

  typedef typename NumericTraits< PixelType >::RealType                             MeasurementType;
  typedef Statistics::DenseFrequencyContainer2                                      HistogramFrequencyContainerType;
  typedef Statistics::Histogram< MeasurementType, HistogramFrequencyContainerType > HistogramType;
//   typedef typename HistogramType::Pointer                            HistogramPointer;
//   typedef typename HistogramType::ConstPointer                       HistogramConstPointer;
  typedef typename HistogramType::MeasurementVectorType              MeasurementVectorType;

  itkStaticConstMacro(DefaultBinsPerAxis, unsigned int, 64);

  /** Set the offset or offsets over which the co-occurrence pairs will be computed.
      Calling either of these methods clears the previous offsets. */
  itkGetConstReferenceMacro(Offsets, OffsetVectorType);
  void SetOffsets( const OffsetVectorType &offsets )
  {
    if ( this->m_Offsets != offsets )
      {
      this->m_Offsets.assign( offsets.begin(), offsets.end() );
      this->Modified();
      }
  }
  void SetOffset(const OffsetType &offset);

  /** Set number of histogram bins along each axis */
  itkSetMacro(NumberOfBinsPerAxis, unsigned int);
  itkGetConstMacro(NumberOfBinsPerAxis, unsigned int);

  /** Set/Get the size of the sliding window **/
  itkSetMacro( WindowSize, RadiusType );
  itkGetConstMacro( WindowSize, RadiusType );

  /** Set the min and max (inclusive) pixel value that will be placed in the
    histogram */
  void SetPixelValueMinMax(PixelType min, PixelType max);

  itkGetConstMacro(Min, PixelType);
  itkGetConstMacro(Max, PixelType);

  /** Set the calculator to normalize the histogram (divide all bins by the
    total frequency). Normalization is off by default. */
  itkSetMacro(Normalize, bool);
  itkGetConstMacro(Normalize, bool);
  itkBooleanMacro(Normalize);


  unsigned int GetNumberOfOutputComponents() { return 8;}

protected:
  TextureFeatureImageFilter();
  // virtual ~TextureFeatureImageFilter() {} empty implementation not needed

  virtual void GenerateInputRequestedRegion()
  {
    // currently we require the entire input image to process
    TInputImage *input = const_cast< TInputImage * >( this->GetInput() );
    input->SetRequestedRegionToLargestPossibleRegion();
  }



  virtual void ThreadedGenerateData(const RegionType& outputRegionForThread, ThreadIdType threadId );

  virtual void BeforeThreadedGenerateData();

  virtual void FillHistogram( HistogramType &histogram, const itk::ConstNeighborhoodIterator<InputImageType> &niter ) const;

  virtual void PrintSelf(std::ostream & os, Indent indent) const;

  void GenerateOutputInformation();

private:
  TextureFeatureImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);           //purposely not implemented

  OffsetVectorType m_Offsets;
  std::vector<std::pair< OffsetType, OffsetType > > m_CooccurenceOffsetVector;

  RadiusType m_WindowSize;

  PixelType        m_Min;
  PixelType        m_Max;

  unsigned int          m_NumberOfBinsPerAxis;
  bool                  m_Normalize;

};
} // end of namespace External
} // end of namespace itk

#include "itkTextureFeatureImageFilter.hxx"

#endif
