#pragma once
#include "helper.hpp"

using namespace std;
using namespace cv;

struct SEGMENT {
  float x1, y1, x2, y2, angle;
  int label;
};

class LineDetector {
 public:
  LineDetector(int lt, float dt, float md = 5.0, float ma = 5.0,
               float canny_thres_1 = 50, float canny_thres_2 = 200,
               int canny_ap = 3)
      : init_label_(0),
        threshold_length_(lt),
        threshold_dist_(dt),
        margin_dist_(md),
        margin_angle_(ma),
        canny_thres_1_(canny_thres_1),
        canny_thres_2_(canny_thres_2),
        canny_ap_(canny_ap) {}
  ~LineDetector() = default;
  void detect(Mat& src, vector<SEGMENT>& segments_all, bool merge = true);
  void draw_arrow(Mat& mat, const SEGMENT* seg, Scalar bgr = Scalar(0, 255, 0),
                  int thickness = 1, bool directed = true);

 private:
  template <class tType>
  void incident_point(tType* pt, Mat& l);
  void merge_lines(SEGMENT* Seg1, SEGMENT* Seg2, SEGMENT* SegMerged);
  bool get_point_chain(const Mat& img, const Point& pt, Point* chained_pt,
                       int& direction, int step);
  double dist_point_to_line(const Mat& p, Mat& l);
  bool merge_segments(SEGMENT* seg1, SEGMENT* seg2, SEGMENT* seg_merged);
  void extract_segments(vector<Point2i>* points, vector<SEGMENT>* segments);
  void point_in_board_test(Mat& src, Point2i* pt);
  void get_angle(SEGMENT* seg);
  void additional_operations_on_segments(Mat& src, SEGMENT* seg);

  int init_label_, image_width_, image_height_, threshold_length_;
  float threshold_dist_;
  float margin_dist_;
  float margin_angle_;
  float canny_thres_1_, canny_thres_2_;
  int canny_ap_;
};
