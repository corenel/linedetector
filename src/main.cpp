#include <gflags/gflags.h>
#include <chrono>

DEFINE_string(i, "", "input file path");
DEFINE_string(o, "", "output file path");
DEFINE_double(dt, 1.5, "line fitting distance threshold");
DEFINE_int32(lt, 20, "line length threshold");

#include "../include/line_detector.hpp"

/** Main function*/
int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  if (FLAGS_i == "") {
    fprintf(stderr, "Define input file path.\n");
    std::cout
        << "Usage: " << argv[0]
        << " -i /input/file/path.png (optional)-o /output/file/path.txt "
           "(optional)-dt distance_threshold (optional)-lt length_threshold"
        << std::endl;
    return 0;
  }

  LineDetector ld(FLAGS_lt, float(FLAGS_dt));
  vector<SEGMENT> lines;
  Mat src_old;
  Mat src = imread(FLAGS_i, 1);
  if (src.cols == 0) {
    fprintf(stderr, "cannot open the input file!");
    return 0;
  }

  Mat src_gray;
#if (CV_VERSION_MAJOR >= 4)
  cvtColor(src, src_gray, cv::COLOR_RGB2GRAY);
#else
  cvtColor(src, src_gray, CV_RGB2GRAY);
#endif

  auto start = chrono::steady_clock::now();

  lines.clear();
  ld.detect(src_gray, lines);

  auto end = chrono::steady_clock::now();
  std::cout << "#Segments: " << lines.size() << std::endl;
  std::cout << "Elapsed time in nanoseconds : "
            << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
            << " ns" << std::endl;
  std::cout << "Elapsed time in microseconds : "
            << chrono::duration_cast<chrono::microseconds>(end - start).count()
            << " µs" << std::endl;
  std::cout << "Elapsed time in milliseconds : "
            << chrono::duration_cast<chrono::milliseconds>(end - start).count()
            << " ms" << std::endl;
  std::cout << "Elapsed time in seconds : "
            << chrono::duration_cast<chrono::seconds>(end - start).count()
            << " sec";

  FILE* fptr;
  string outfile;
  if (FLAGS_o == "") {
    outfile = FLAGS_i + "_lines.txt";
  } else {
    outfile = FLAGS_o;
  }
  std::cout << "Writing coordinates of lines to file " << outfile << std::endl;
  fptr = fopen(outfile.c_str(), "w");
  for (size_t i = 0; i < lines.size(); i++) {
    SEGMENT seg = lines.at(i);
    fprintf(fptr, "%d %.5f %.5f %.5f %.5f %.5f \n", seg.label, seg.angle,
            seg.x1, seg.y1, seg.x2, seg.y2);
  }
  fclose(fptr);

  Mat blank = Mat::ones(src.rows, src.cols, CV_8UC1);
  blank += 255;
  Mat blank_color;

#if (CV_VERSION_MAJOR >= 4)
  cvtColor(blank, blank_color, cv::COLOR_GRAY2BGR);
#else
  cvtColor(blank, blank_color, CV_GRAY2BGR);
#endif

  for (size_t i = 0; i < lines.size(); i++) {
    SEGMENT seg = lines.at(i);

    int b = (seg.label * 12337) % 256;
    int g = (seg.label * 24776) % 256;
    int r = (seg.label * 11491) % 256;

    ld.draw_arrow(src, &seg, Scalar(b, g, r));
    char cLabel[64];
    sprintf(cLabel, "%d", seg.label);
    Point2i pM;
    pM.x = (seg.x1 + seg.x2) / 2;
    pM.y = (seg.y1 + seg.y2) / 2;
    Point2i pe1;
    double dGap = 15.0;
    if (seg.angle < CV_PI / 1.5f) dGap = 5.0;
    double dAngle = (double)seg.angle;
    double dArrowAng = 89.0;
    pe1.x = cvRound(pM.x - dGap * cos(dArrowAng * CV_PI / 180.0 + dAngle));
    pe1.y = cvRound(pM.y - dGap * sin(dArrowAng * CV_PI / 180.0 + dAngle));
    putText(src, cLabel, pe1, FONT_HERSHEY_PLAIN, 1, Scalar(b, g, r), 1, 8,
            false);
    line(blank_color, Point2f(seg.x1, seg.y1), Point2f(seg.x2, seg.y2),
         Scalar(0, 0, 255));
  }

  imshow(FLAGS_i, src);

  string outimgpath = FLAGS_i + "_labeled.png";
  imwrite(outimgpath, src);
  std::cout << outimgpath << " saved. " << std::endl;
  outimgpath = FLAGS_i + "_only_lines.png";
  imwrite(outimgpath, blank_color);
  std::cout << outimgpath << " saved. " << std::endl;
  std::cout << "lines.size() = " << lines.size() << std::endl;

  return 0;
}
