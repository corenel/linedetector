%module line_detector

%include <opencv.i>
%include <std_vector.i>
%cv_instantiate_all_defaults
%template(StringVector) std::vector<std::string>;
%template(MatVector) std::vector<cv::Mat>;

%{
#include "line_detector.hpp"
%}

%include "line_detector.hpp"

%template(SegmentVector) std::vector<SEGMENT>;
