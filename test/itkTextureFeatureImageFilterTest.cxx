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
#include "itkTextureFeatureImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

int itkTextureFeatureImageFilterTest( int argc, char **argv )
{

  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile outputImageFile radius" << std::endl;
    return EXIT_FAILURE;
    }

  const std::string inFilename = argv[1];
  const std::string outFilename = argv[2];

  unsigned int radius = atoi( argv[3] );

  const unsigned int Dimension = 3;

  typedef unsigned char PixelType;


  typedef itk::Image< PixelType,  Dimension >       InputImageType;
  typedef itk::VectorImage< float, Dimension > VectorImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inFilename );

  typedef itk::External::TextureFeatureImageFilter< InputImageType, VectorImageType > CooccurenceFilterType;

  CooccurenceFilterType::OffsetType o1 = {{1,0,0}};


  CooccurenceFilterType::Pointer glcm = CooccurenceFilterType::New();

  CooccurenceFilterType::RadiusType window;
  window.Fill( radius );

  CooccurenceFilterType::OffsetVectorType offsets;
  offsets.push_back( o1 );


  glcm->SetInput( reader->GetOutput() );
  glcm->SetOffsets( offsets );
  glcm->SetWindowSize( window );
  glcm->SetNumberOfBinsPerAxis( 16 );

  typedef itk::ImageFileWriter< VectorImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outFilename );
  writer->SetInput( glcm->GetOutput() );
  writer->Update();

  return 0;
}
