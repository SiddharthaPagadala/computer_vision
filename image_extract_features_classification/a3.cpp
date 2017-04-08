
#include "CImg.h"
#include <ctime>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string>
#include <vector>
#include <Sift.h>
#include <sys/types.h>
#include <dirent.h>
#include <map>
#include <numeric>
#include <sstream>
#include <fstream>
#include <iterator>
#include <cstdlib>

//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;

// Dataset data structure, set up so that e.g. dataset["bagel"][3] is
// filename of 4th bagel image in the dataset
typedef map<string, vector<string> > Dataset;

#include <Classifier.h>

typedef map<string, vector< vector<SiftDescriptor> > > Data;
typedef map<string,vector< vector<int> > > Data_Histograms;
typedef map<string,vector< vector<double> > > Data_Haar;
typedef map<string,vector< string> > Data_Deep;

typedef map<string,int> class_encode;


#include "HaarFeatures.h"
#include "KMeans.h"
#include "BagOfVisualWords.h"
#include "DeepFeatures.h"
#include "SVM.h"
#define k 50
// Figure out a list of files in a given directory.
//
vector<string> files_in_directory(const string &directory, bool prepend_directory = false)
{
  vector<string> file_list;
  DIR *dir = opendir(directory.c_str());
  if(!dir)
    throw std::string("Can't find directory " + directory);
  
  struct dirent *dirent;
  while ((dirent = readdir(dir))) 
    if(dirent->d_name[0] != '.')
      file_list.push_back((prepend_directory?(directory+"/"):"")+dirent->d_name);

  closedir(dir);
  return file_list;
}

int main(int argc, char **argv)
{
  try {
    if(argc < 3)
      throw string("Insufficent number of arguments");

    string mode = argv[1];
    string algo = argv[2];
	
	class_encode class_encoding ;
	

    // Scan through the "train" or "test" directory (depending on the
    //  mode) and builds a data structure of the image filenames for each class.
    Dataset filenames; 
    vector<string> class_list = files_in_directory(mode);
    for(vector<string>::const_iterator c = class_list.begin(); c != class_list.end(); ++c)
      filenames[*c] = files_in_directory(mode + "/" + *c, true);
	
	//creates map of classes with index from class_list to encode class names with integers
	for(int i=0;i<class_list.size();i++){
		class_encoding[class_list[i]] = i+1;
	}
    // set up the classifier based on the requested algo
    Classifier *classifier=0;
	classifier = new SVM(class_list);
    if(algo == "baseline"){
		cout << "svm baseline" << endl;
	}else if(algo == "haar"){
			HaarFeatures *haar = new HaarFeatures(1);
			Data_Haar data_haar = haar->get_haar_features(filenames);
			cout << "Writing Features to file Started..." << endl;
			cout << endl;
			ofstream out;
			if(mode == "train") out.open("data_haar.dat");
			else out.open("test_data.dat");
			for(Data_Haar::const_iterator it=data_haar.begin();it!=data_haar.end();++it){
				for(int i=0; i<it->second.size();i++){
					out << class_encoding[it->first] << " ";
					for(int j = 0; j < it->second[i].size(); j++)
					{
						out<<j+1<<":"<<it->second[i][j] << " ";
					}
					
					out << endl;
				}
				
			}
			out.close();
			cout << "Writing Features to file Completed!" << endl;
			cout << endl;
		
		
	}else if(algo == "bow"){
		cout << "Creating Bag Of Words features..." << endl;
		if(mode == "train"){
			BagOfVisualWords bovw(k);
			Data_Histograms data_hist = bovw.get_bov(filenames);
			
			ofstream out("data_histogram.dat");
			for(Data_Histograms::const_iterator it=data_hist.begin();it!=data_hist.end();++it){
				for(int i=0; i<it->second.size();i++){
					out << class_encoding[it->first] << " ";
					for(int j = 0; j < it->second[i].size(); j++)
					{
						out<<j+1<<":"<<it->second[i][j]<< " ";
					}
					//std::copy(it->second[i].begin(), it->second[i].end(), ostream_iterator<int>(out, " "));
					out << endl;
				}
				
			}
			out.close();
			cout << endl;
			cout << "Bag Of Words Features Construction Completed!" << endl;
			ofstream out1("vocabulary.txt");
			vector< vector<float> > vocabulary = bovw.get_vocabulary();
			
			for(int i=0; i<vocabulary.size();i++){
				
				std::copy(vocabulary[i].begin(), vocabulary[i].end(), ostream_iterator<float>(out1, " "));
				out1 << endl;
			}
			out1.close();
		}
		
	}else if(algo == "deep"){
		
			DeepFeatures deep_feat;
			Data_Deep deep_features = deep_feat.get_deep_features(filenames);
			cout << "Features Extraction Completed..." << endl;
			ofstream out;
			if(mode == "train") out.open("deep_features.dat");
			else out.open("test_data.dat");
			for(Data_Deep::const_iterator it=deep_features.begin();it!=deep_features.end();++it){
				 
				for(int i=0;i<it->second.size();i++){
					int d = it->second.size();
					
					cout << "Size:" << d << endl;
					
					out << class_encoding[it->first] << " ";
						
					std::istringstream buf(it->second[i]);
					std::istream_iterator<std::string> beg(buf), end;
					
					std::vector<std::string> tokens(beg, end);
					
					for(int m=0;m<tokens.size();m++){
						float t = strtof(tokens[m].c_str(),0);
						out << m+1 << ":" << t << " ";
					}
					
					out << endl;
										
				}		
			}
			out.close();
			cout << "Writing to file completed" << endl;
		
		
	}
    else
      throw std::string("unknown classifier " + algo);

    // now train or test!
    if(mode == "train"){
      classifier->train(filenames,algo);
	  cout << "train" << endl;
		}
    else if(mode == "test")
      classifier->test(filenames,algo);
    else
      throw std::string("unknown mode!");
  }
  catch(const string &err) {
    cerr << "Error: " << err << endl;
  }
}