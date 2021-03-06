#if defined (_MSC_VER) && !defined (_WIN64)
#pragma warning(disable:4244) // boost::number_distance::distance()
                              // converts 64 to 32 bits integers
#endif

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Classification.h>
#include <CGAL/Classification/Random_forest_classifier.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Point_set_3/IO.h>

#include <CGAL/Real_timer.h>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Point_set_3<Point> Point_set;
typedef Kernel::Iso_cuboid_3 Iso_cuboid_3;

typedef typename Point_set::Point_map Pmap;
typedef typename Point_set::Property_map<int> Imap;
typedef typename Point_set::Property_map<unsigned char> UCmap;

namespace Classification = CGAL::Classification;

typedef Classification::Label_handle                                            Label_handle;
typedef Classification::Feature_handle                                          Feature_handle;
typedef Classification::Label_set                                               Label_set;
typedef Classification::Feature_set                                             Feature_set;

typedef Classification::Random_forest_classifier Classifier;

typedef Classification::Point_set_feature_generator<Kernel, Point_set, Pmap>    Feature_generator;


int main (int argc, char** argv)
{
  std::string filename (argc > 1 ? argv[1] : "data/b9_training.ply");
  std::ifstream in (filename.c_str());
  Point_set pts;

  std::cerr << "Reading input" << std::endl;
  in >> pts;
  
  Imap label_map;
  bool lm_found = false;
  boost::tie (label_map, lm_found) = pts.property_map<int> ("label");
  if (!lm_found)
  {
    std::cerr << "Error: \"label\" property not found in input file." << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<int> ground_truth;
  ground_truth.reserve (pts.size());
  std::copy (pts.range(label_map).begin(), pts.range(label_map).end(),
             std::back_inserter (ground_truth));

  Feature_set features;
  
  std::cerr << "Generating features" << std::endl;
  CGAL::Real_timer t;
  t.start();
  Feature_generator generator (features, pts, pts.point_map(),
                               5);  // using 5 scales
  t.stop();
  std::cerr << "Done in " << t.time() << " second(s)" << std::endl;

  // Add types
  Label_set labels;
  Label_handle ground = labels.add ("ground");
  Label_handle vegetation = labels.add ("vegetation");
  Label_handle roof = labels.add ("roof");

  Classifier classifier (labels, features);
  
  std::cerr << "Training" << std::endl;
  t.reset();
  t.start();
  classifier.train (ground_truth);
  t.stop();
  std::cerr << "Done in " << t.time() << " second(s)" << std::endl;

  t.reset();
  t.start();
  std::vector<int> label_indices(pts.size(), -1);
  Classification::classify_with_graphcut<CGAL::Sequential_tag>
    (pts, pts.point_map(), labels, classifier,
     generator.neighborhood().k_neighbor_query(12),
     0.2, 1, label_indices);
  t.stop();
  std::cerr << "Classification with graphcut done in " << t.time() << " second(s)" << std::endl;

  std::cerr << "Precision, recall, F1 scores and IoU:" << std::endl;
  Classification::Evaluation evaluation (labels, ground_truth, label_indices);
  
  for (std::size_t i = 0; i < labels.size(); ++ i)
  {
    std::cerr << " * " << labels[i]->name() << ": "
              << evaluation.precision(labels[i]) << " ; "
              << evaluation.recall(labels[i]) << " ; "
              << evaluation.f1_score(labels[i]) << " ; "
              << evaluation.intersection_over_union(labels[i]) << std::endl;
  }

  std::cerr << "Accuracy = " << evaluation.accuracy() << std::endl
            << "Mean F1 score = " << evaluation.mean_f1_score() << std::endl
            << "Mean IoU = " << evaluation.mean_intersection_over_union() << std::endl;

  // Color point set according to class
  UCmap red = pts.add_property_map<unsigned char>("red", 0).first;
  UCmap green = pts.add_property_map<unsigned char>("green", 0).first;
  UCmap blue = pts.add_property_map<unsigned char>("blue", 0).first;

  for (std::size_t i = 0; i < label_indices.size(); ++ i)
  {
    label_map[i] = label_indices[i]; // update label map with computed classification
    
    Label_handle label = labels[label_indices[i]];

    if (label == ground)
    {
      red[i] = 245; green[i] = 180; blue[i] =   0;
    }
    else if (label == vegetation)
    {
      red[i] =   0; green[i] = 255; blue[i] =  27;
    }
    else if (label == roof)
    {
      red[i] = 255; green[i] =   0; blue[i] = 170;
    }
  }

  // Write result
  std::ofstream f ("classification.ply");
  f.precision(18);
  f << pts;

  std::cerr << "All done" << std::endl;
  
  return EXIT_SUCCESS;
}
