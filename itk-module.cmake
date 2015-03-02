
# the top-level README is used for describing this module, just
# re-used it for documentation here
get_filename_component( MY_CURENT_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file( READ "${MY_CURENT_DIR}/README" DOCUMENTATION )

# ITK version 4.5 changed it from EXCLUDE_FROM_ALL to EXCLUDE_FROM_DEFAULT
set( _EXCLUDE "EXCLUDE_FROM_ALL" )
if (NOT "${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_MINOR_PATCH}" VERSION_LESS "4.5")
  set( _EXCLUDE "EXCLUDE_FROM_DEFAULT" )
endif()

# define the dependencies of the include module and the tests
itk_module(ITKTextureAnalysis
  DEPENDS
    ITKCommon
    ITKStatistics
    ITKMathematicalMorphology
  TEST_DEPENDS
    ITKTestKernel
    ITKMetaIO
    ITKNIFTI
    ITKPNG
  DESCRIPTION
    "${DOCUMENTATION}"
  ${_EXCLUDE}
)
