itk_module_test()

#${itk-module} will be the name of this module and will not need to be
#changed when this module is renamed.

set(ITK${itk-module}Tests
  itkTextureMovingHistogramImageFilter.cxx
  itkTextureFeatureImageFilterTest.cxx
)

CreateTestDriver(${itk-module}  "${${itk-module}-Test_LIBRARIES}" "${${itk-module}Tests}")

itk_add_test(NAME itk${itk-module}Test
      COMMAND ITK${itk-module}TestDriver itkExternalTemplateTest)
