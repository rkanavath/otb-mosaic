/*=========================================================================
 *
 *  Copyright 2015 Irstea
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

#ifndef StreamingMultibandFeatherMosaicFilter_H_
#define StreamingMultibandFeatherMosaicFilter_H_

#include "itkImageToImageFilter.h"
#include "otbStreamingMosaicFilterBase.h"
#include "otbStreamingFeatherMosaicFilter.h"
#include "itkSubtractImageFilter.h"
#include "otbSummingFilter.h"
#include "itkMacro.h"

// For gaussian filtering
#include "itkDiscreteGaussianImageFilter.h"
//#include "itkGPUDiscreteGaussianImageFilter.h" // TODO: Make this happen
#include "otbPerBandVectorImageFilter.h"

namespace otb
{

/** \class StreamingMultibandFeatherMosaicFilter
 * \brief Computes a nice seamless mosaic with no ghosting effect
 *
 * Mandatory inputs : images to mosaic + alpha channels
 *
 * Output: nice mosaic
 */
template <class TInputImage, class TOutputImage, class TDistanceImage>
class ITK_EXPORT StreamingMultibandFeatherMosaicFilter : public otb::StreamingFeatherMosaicFilter<TInputImage,
                                                                                                  TOutputImage,
                                                                                                  TDistanceImage>
{
public:

  /** Standard Self typedef */
  typedef StreamingMultibandFeatherMosaicFilter                                        Self;
  typedef otb::StreamingFeatherMosaicFilter<TInputImage, TOutputImage, TDistanceImage> Superclass;
  typedef itk::SmartPointer<Self>                                                      Pointer;
  typedef itk::SmartPointer<const Self>                                                ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(StreamingMultibandFeatherMosaicFilter, StreamingFeatherMosaicFilter);

  /** Images typedefs */
  typedef TInputImage                              ImageType;
  typedef typename ImageType::ConstPointer         ImageConstPointer;
  typedef TDistanceImage                           DistanceImageType;
  typedef typename DistanceImageType::ConstPointer DistanceImageConstPointer;
  typedef typename DistanceImageType::PixelType    DistanceImageValueType;
  typedef TOutputImage                             OutputImage;
  typedef typename OutputImage::SpacingType        OutputImageSpacingType;

  /** Gaussian filter typedefs */
  typedef typename TInputImage::InternalPixelType                                         InputImageInternalValueType;
  typedef otb::Image<InputImageInternalValueType>                                         SingleImageType;
  typedef itk::DiscreteGaussianImageFilter<SingleImageType, SingleImageType>              DiscreteGaussianFilterType;
  typedef typename DiscreteGaussianFilterType::Pointer                                    DiscreteGaussianFilterPointer;
  typedef otb::PerBandVectorImageFilter<ImageType, ImageType, DiscreteGaussianFilterType> PerBandFilterType;
  typedef typename PerBandFilterType::Pointer                                             PerBandFilterPointer;

  /** Other filters typedefs */
  typedef itk::SubtractImageFilter<TInputImage, TInputImage, TInputImage>              SubImageFilterType;
  typedef typename SubImageFilterType::Pointer                                         SubImageFilterPointer;
  typedef otb::StreamingFeatherMosaicFilter<TInputImage, TOutputImage, TDistanceImage> MosaicFilterType;
  typedef typename MosaicFilterType::Pointer                                           MosaicFilterPointer;
  typedef otb::SummingFilter<TOutputImage, TOutputImage>                               SummingFilterType;
  typedef typename SummingFilterType::Pointer                                          SummingFilterPointer;

  /** Accessors for the number of levels */
  itkSetMacro(NumberOfLevels, unsigned int);
  itkGetMacro(NumberOfLevels, unsigned int);

  /** Accessors for the first level variance & transition distance */
  itkSetMacro(FirstLevelTransitionDistance, DistanceImageValueType);
  itkGetMacro(FirstLevelTransitionDistance, DistanceImageValueType);
  itkSetMacro(FirstLevelVariance, double);
  itkGetMacro(FirstLevelVariance, double);

protected:
  StreamingMultibandFeatherMosaicFilter();
  virtual ~StreamingMultibandFeatherMosaicFilter() {
  }

  virtual void Modified();

  /** Overrided methods */
  virtual void GenerateData();

  virtual void GenerateOutputInformation()
  {
    itkDebugMacro("GenerateOutputInformation");

    Superclass::GenerateOutputInformation();
    this->Modified();
  }

private:
  StreamingMultibandFeatherMosaicFilter(const Self&); //purposely not
                                                      // implemented
  void operator=(const Self&);                        //purposely not
                                                      // implemented

  unsigned int m_NumberOfLevels;    // number of scales

  /* Variances */
  double              m_FirstLevelVariance; // variance of first scale
  std::vector<double> m_Variances;          // variances values

  /* Transitions parameters */
  DistanceImageValueType              m_FirstLevelTransitionDistance; // transition
                                                                      // distance
                                                                      // of the
                                                                      // first
                                                                      // scale
  std::vector<DistanceImageValueType> m_TransitionDistances;          // transition
                                                                      // distances
                                                                      // for
                                                                      // each
                                                                      // scale
  std::vector<DistanceImageValueType> m_TransitionOffsets;            // transition
                                                                      // offsets
                                                                      // for
                                                                      // each
                                                                      // scale

  /* Internal filters */
  std::vector<std::vector<SubImageFilterPointer> >          m_SubImageFilter; // subtract
                                                                              // filter
                                                                              // array
                                                                              // (2D)
  std::vector<std::vector<PerBandFilterPointer> >           m_Filter;         // smoothing
                                                                              // filter
                                                                              // array
                                                                              // (2D)
  std::vector<std::vector<DiscreteGaussianFilterPointer>  > m_SingleFilter;   // smoothing
                                                                              // filter
                                                                              // (per
                                                                              // band)
                                                                              // array
                                                                              // (2D)
  std::vector<MosaicFilterPointer>                          m_MosaicFilter;   // mosaicking
                                                                              // filter
                                                                              // array
                                                                              // (1D)
  SummingFilterPointer                                      m_SummingFilter;  // Summing
                                                                              // filter

};

} // end namespace gtb

#include "otbStreamingMultibandFeatherMosaicFilter.hxx"

#endif /* StreamingMultibandFeatherMosaicFilter_H_ */