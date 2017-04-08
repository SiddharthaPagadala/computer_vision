class FeatureFilter{
	int rows;
	int cols;
	int sep;
	
	public:
	FeatureFilter(int a, int b, int c):rows(a), cols(b),sep(c){};
	int get_rows(){
		return rows;
	}
	
	int get_cols(){
		return cols;
	}
	
	int get_sep(){
		return sep;
	}
};
class HaarFeatures
{
public:
	int x;
	Data_Haar data_haar;
	HaarFeatures(int a):x(a){}
	
	Data_Haar get_haar_features(const Dataset &file_names){
		
		cout << "Extracting Haar Features Started..." << endl;
		cout << endl;
		for(Dataset::const_iterator it =file_names.begin();it!=file_names.end();++it){
			vector< vector<double> > class_img_haar;
			cout << "Processing image class:" << it->first << endl;
			vector< vector<SiftDescriptor> > data_1;
			for(int i=0; i<it->second.size(); i++){
				cout << "Image :" << i << endl;
				class_img_haar.push_back(extract_features(it->second[i].c_str()));
			}
			data_haar[it->first] = class_img_haar;	
		}
		cout << "Extracting Haar Features Completed!" << endl;
		cout << endl;
		return data_haar;	
	}
		
	
	double compute_integral_image_table(int x, int y, double intensity, CImg<double> &integral_image){
		if(x<1 && y<1) return intensity;
		else if(x<1) return intensity + integral_image(y-1,x);
		else if(y<1) return intensity + integral_image(y,x-1);
		else return intensity + integral_image(y,x-1) + integral_image(y-1,x) - integral_image(y-1,x-1);
	}
	
	vector<double> compute_features(CImg<double> &integral_image,FeatureFilter &filter){
		vector<double> features;
		int rows = filter.get_rows();
		int cols = filter.get_cols();
		int sep = filter.get_sep();
		
		for(int i=0; i<integral_image.height()-rows+1; i++){
			for(int j=0;j<integral_image.width()-cols;j++){
				double d_1 = ((i + rows - 1)>-1 && (j + cols - 1)>-1) ? integral_image(j + cols - 1, i + rows - 1) : 0.0;
				double a_1 = ((i - 1)>-1 && (j - 1)>-1) ? integral_image(j - 1, i - 1) : 0.0;
				double b_1 = ((j + cols - 1)>-1 && (i - 1)>-1) ? integral_image(j + cols - 1, i - 1) : 0.0;
				double c_1 = ((i + rows - 1)>-1 && (j - 1)>-1) ? integral_image(j - 1, i + rows - 1) : 0.0;
				//cout << "a_1:"<<a_1<<",b_1:"<<b_1<<",c_1:"<<c_1<<",d_1:"<<d_1<<endl;
				double d_2 = ((i + rows- 1)>-1 && (j + cols +sep- 1)>-1) ? integral_image(j + cols+sep - 1, i + rows- 1) : 0.0;
				double a_2 = ((i  - 1)>-1 && (j +sep- 1)>-1) ? integral_image(j+sep - 1, i - 1) : 0.0;
				double b_2 = ((j + cols+sep - 1)>-1 && (i - 1)>-1) ? integral_image(j + cols+sep - 1, i  - 1) : 0.0;
				double c_2 = ((i + rows - 1)>-1 && (j+sep - 1)>-1) ? integral_image(j +sep- 1, i + rows - 1) : 0.0;
				double temp_diff = d_2 + a_2 - b_2 - c_2 - (d_1 + a_1 - b_1 - c_1);
				
				features.push_back(temp_diff);
			}
		}
		return features;
	}
	
	CImg<double> get_integral_imge(CImg<double> &input_gray_image){
		CImg<double> integral_image(input_gray_image.width(),input_gray_image.height());
		//cout << "Width:" << input_gray_image.width() << ",Height:" << input_gray_image.height() << endl;
		for(int i=0;i<input_gray_image.height();i++){
			for(int j=0;j<input_gray_image.width();j++){
				integral_image(j,i) = compute_integral_image_table(i,j,input_gray_image(j,i),integral_image);
			}
		}
		return integral_image;
		
	}
	vector<double> extract_features(const string &image_file_path){
		CImg<double> input_image(image_file_path.c_str());
		CImg<double> gray = input_image.get_RGBtoHSI().get_channel(2);
		
		CImg<double> integral_image = get_integral_imge(gray);
		FeatureFilter filter1(1,1,1);
		vector<double> features = compute_features(integral_image,filter1);
		FeatureFilter filter2(1,4,2);
		vector<double> features1 = compute_features(integral_image,filter2);
		features.insert(features.end(), features1.begin(), features1.end());
		return features;
	}
};
