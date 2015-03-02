itk_module_test()

#${itk-module} will be the name of this module and will not need to be
#changed when this module is renamed.

set( "${itk-module}_DATA_ROOT" "${CMAKE_CURRENT_SOURCE_DIR}/Data")

set(ITK${itk-module}Tests
  itkTextureMovingHistogramImageFilter.cxx
  itkTextureFeatureImageFilterTest.cxx
)

CreateTestDriver(${itk-module}  "${${itk-module}-Test_LIBRARIES}" "${ITK${itk-module}Tests}")

itk_add_test(NAME itkTextureMovingHistogramImageFilterTest1
      COMMAND ${itk-module}TestDriver
        itkTextureMovingHistogramImageFilter
          ${${itk-module}_DATA_ROOT}/Input/cthead1.png
          ${ITK_TEST_OUTPUT_DIR}/itkTextureMovingHistogramImageFilterTest1.mha
      )

itk_add_test(NAME itkTextureFeatureImageFilterTest1
      COMMAND ${itk-module}TestDriver itkTextureFeatureImageFilterTest
          ${${itk-module}_DATA_ROOT}/Input/cthead1.png
          ${ITK_TEST_OUTPUT_DIR}/itkTextureFeatureImageFilterTest1.mha
          5
      )
