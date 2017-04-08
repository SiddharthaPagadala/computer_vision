//Constructs Bag Of Visual Words features for the images 
#include <algorithm>

class BagOfVisualWords{
public:
	vector< vector<float> > vocabulary;
	int k;
	vector<Cluster> clusters;
	BagOfVisualWords(int k_inp):k(k_inp){}
	Data_Histograms data_histograms;
	Data train_data;
	Data test_data;
	vector< vector<float> > get_vocabulary(){
		return vocabulary;
	}
	
	//constructs test dataset vector with histograms based on vocabulary visual words
	Data_Histograms get_test_image_features(const Dataset &file_names){
		vector< vector<float> > vocabulary_read;
		read_vocabulary(vocabulary_read);
		cout << "Vocabulary loading completed" << endl;
		
		for(Dataset::const_iterator it=file_names.begin();it!=file_names.end();++it){
			vector< vector<SiftDescriptor> > data_1;
			cout << "Processing image class:" << it->first << endl;
			for(int i=0;i<it->second.size();i++){
				data_1.push_back(extract_features_test(it->second[i].c_str()));
			}
			test_data[it->first] = data_1;
		}
		
		for(Data::const_iterator it=test_data.begin();it!=test_data.end();++it){
			vector< vector<int> > class_img_hist;
			cout << "Constructing Histogram for image class:"<< it->first << endl;
			for(int i=0;i < it->second.size();i++){
				
				class_img_hist.push_back(construct_test_hist(it->second[i],vocabulary_read));
				
			}
			data_histograms[it->first] = class_img_hist;
		}
		cout << endl;
		cout << "Histograms construction completed!" << endl;
		return data_histograms;
		
	}
	
	//extracts sift descriptors of an image
	vector<SiftDescriptor> extract_features_test(const string &image_file_path){
		CImg<double> input_image(image_file_path.c_str());
		CImg<double> gray = input_image.get_RGBtoHSI().get_channel(2);
		
		vector<SiftDescriptor> descriptors = Sift::compute_sift(gray);
		
		return descriptors;
	}
	
	//
	vector<int> construct_test_hist(vector<SiftDescriptor> image_sift_desc, vector< vector<float> > &vocabulary_read){
		vector<int> image_hist_bov(vocabulary_read.size(), 0);
		
		for(int j=0; j<image_sift_desc.size(); j++){
			int min_cluster_index = get_nearest_feature(image_sift_desc[j].descriptor,vocabulary_read);
			image_hist_bov[min_cluster_index] = image_hist_bov[min_cluster_index]+1;
		}
		return image_hist_bov;
	}
	
	int get_nearest_feature(vector<float>  &data, vector< vector<float> > &vocabulary_read){
		int min_cluster_index;
		double min;
		
		min=9999999999999;
		min_cluster_index = 0;
		for(int i=0;i<vocabulary_read.size();i++){
			
			double temp = calculateEucidean(data,vocabulary_read[i]);
			if(temp < min){
				min=temp;
				min_cluster_index = i;
			} 

		}
		return min_cluster_index;
	}
	
	double calculateEucidean(vector<float> &data_desc1,vector<float> centroid_desc2)
	{
		double diff = 0;
		for(int i = 0;i<centroid_desc2.size();i++)
		{
			diff+= (data_desc1[i] - centroid_desc2[i])*(data_desc1[i] - centroid_desc2[i]);
		}

		diff = sqrt (diff);
		
		return diff;
	}
	void read_vocabulary(vector< vector<float> > &vocabulary_read){
		cout << "Reading Vocabulary:" << endl;
		ifstream file("vocabulary.txt");
		string line ;

		while(getline( file, line ) ){
			
			std::istringstream buf(line);
			std::istream_iterator<std::string> beg(buf), end;
			
			std::vector<std::string> tokens(beg, end);
			vector<float> seq;
			for(int m=0;m<tokens.size();m++){
				float t = strtof(tokens[m].c_str(),0);
				seq.push_back( t );
				
			}
			
			vocabulary_read.push_back( seq);

		} 
		
	}
	Data_Histograms get_bov(const Dataset &file_names){
		vector< vector<float> > all_patches;
		
		
		
		for(Dataset::const_iterator it=file_names.begin();it!=file_names.end();++it){
			vector< vector<SiftDescriptor> > data_1;
			cout << "Processing image class:" << it->first << endl;
			for(int i=0;i<it->second.size();i++){
				data_1.push_back(extract_features(it->second[i].c_str(), all_patches));
			}
			train_data[it->first] = data_1;
		}
		
		//construct vocabulary
		clusters = construct_vocabulary(all_patches, vocabulary);
		cout << "Vocabulary construction completed..." << endl;
		//Construct Histogram
		
		for(Data::const_iterator it=train_data.begin();it!=train_data.end();++it){
			vector< vector<int> > class_img_hist;
			cout << "Constructing Histogram for image class:"<< it->first << endl;
			for(int i=0;i < it->second.size();i++){
				class_img_hist.push_back(get_vocabulary_match_index(it->second[i],clusters));
			}
			data_histograms[it->first] = class_img_hist;
		}
		cout << endl;
		cout << "Histograms construction completed!" << endl;
		return data_histograms;
	}
	void print_vector_i(vector<int> temp){
		for(int i=0;i<temp.size();i++){
			cout << "vector print : i:" << i << temp[i] << endl;
		}
	}
	vector<int> get_vocabulary_match_index(vector<SiftDescriptor> image_sift_desc, vector<Cluster> &clusters){
		vector<int> image_hist_bov(clusters.size(), 0);
		//image_hist_bov.reserve(clusters.size());
		for(int j=0; j<image_sift_desc.size(); j++){
			
			for(int i=0; i<clusters.size();i++){
				vector< vector<float> > cluster_elem_temp = clusters[i].cluster_elements;
				
				if( std::find(cluster_elem_temp.begin(), cluster_elem_temp.end(), image_sift_desc[j].descriptor ) != cluster_elem_temp.end()){
					image_hist_bov[i] = image_hist_bov[i]+1;
				}
				
			}
		}
		//print_vector_i(image_hist_bov);
		return image_hist_bov;
	}
	
	vector<Cluster> construct_vocabulary(vector< vector<float> > &all_patches, vector< vector<float> > &vocabulary){
		cout << "Constructing Vocabulary ... kmeans started.." << endl;
		KMeans km(all_patches,k);
		clusters = km.k_means_alg(vocabulary);
		return clusters;
	}
	vector<SiftDescriptor> extract_features(const string &image_file_path, vector< vector<float> > &all_patches){
		CImg<double> input_image(image_file_path.c_str());
		CImg<double> gray = input_image.get_RGBtoHSI().get_channel(2);
		
		vector<SiftDescriptor> descriptors = Sift::compute_sift(gray);
		descriptors.resize(50);
		vector< vector<float> > descriptors_temp;
		for(int i=0; i<descriptors.size(); i++){
			descriptors_temp.push_back(descriptors[i].descriptor);
		}
		all_patches.insert(all_patches.end(), descriptors_temp.begin(), descriptors_temp.end());
		return descriptors;
	}
};
