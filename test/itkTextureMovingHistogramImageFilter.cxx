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
#include "itkTextureMovingHistogramImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


static void Test1( std::string inFileName )
{
  const unsigned int ImageDimension = 3;
  typedef itk::Image<unsigned char, ImageDimension > ImageType;
  typedef itk::Image<itk::FixedArray<float,6>, ImageDimension > OImageType;
  typedef itk::FlatStructuringElement< ImageDimension > KernelType;
  typedef itk::External::TextureMovingHistogramImageFilter< ImageType, OImageType, KernelType > TextureFilterType;


  typedef itk::ImageFileReader<ImageType> ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inFileName );
  reader->UpdateLargestPossibleRegion();

  KernelType::SizeType radius;
  radius.Fill( 5 );
  KernelType kernel = KernelType::Box( radius );
  TextureFilterType::Pointer filter = TextureFilterType::New();
  filter->SetKernel( kernel );
  filter->SetInput( reader->GetOutput() );
  filter->UpdateLargestPossibleRegion();
}


static void Test2( std::string inFileName )
{
  const unsigned int ImageDimension = 3;
  typedef itk::Image<unsigned char, ImageDimension > ImageType;
  typedef itk::VectorImage<float, ImageDimension > OImageType;
  typedef itk::FlatStructuringElement< ImageDimension > KernelType;
  typedef itk::External::TextureMovingHistogramImageFilter< ImageType, OImageType, KernelType > TextureFilterType;


  typedef itk::ImageFileReader<ImageType> ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inFileName );
  reader->UpdateLargestPossibleRegion();

  KernelType::SizeType radius;
  radius.Fill( 5 );
  KernelType kernel = KernelType::Box( radius );
  TextureFilterType::Pointer filter = TextureFilterType::New();
  filter->SetKernel( kernel );
  filter->SetInput( reader->GetOutput() );
  filter->UpdateLargestPossibleRegion();

  std::cout << "filter..." << std::endl;

}

int itkTextureMovingHistogramImageFilter( int argc, char *argv[] )
{

  std::cout << argv[0] << " " << argv[1] << " " << argv[2] << std::endl;


  Test1( argv[1] );
  Test2( argv[2] );

  return EXIT_SUCCESS;
}
