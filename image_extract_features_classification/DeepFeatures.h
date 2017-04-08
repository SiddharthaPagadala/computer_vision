#define OVERFEAT_PATH "overfeat/bin/linux_64/overfeat"
class DeepFeatures{
public:
	Data_Deep data_deep;
	DeepFeatures(){}
	
	Data_Deep get_deep_features(const Dataset &file_names){
		vector< string > class_img_haar;
		for(Dataset::const_iterator it =file_names.begin();it!=file_names.end();++it){
			class_img_haar.clear();
			cout << "Processing image class:" << it->first << endl;
			vector< vector<SiftDescriptor> > data_1;
			for(int i=0; i<it->second.size(); i++){
				int d = it->second.size();
				
				class_img_haar.push_back(extract_features(it->second[i].c_str()));
				//extract_features(it->second[i].c_str());
			}
			data_deep[it->first] = class_img_haar;	
		}
		return data_deep;
	}
	
	string exec(const char* cmd) {
		FILE* pipe = popen(cmd, "r");
		if (!pipe) return "ERROR";
		char buffer[128];
		std::string result = "";
		while(!feof(pipe)) {
			if(fgets(buffer, 128, pipe) != NULL)
				result += buffer;
		}
		pclose(pipe);
		return result;
	}
	
	string trim(const string& str)
	{
		size_t first = str.find_first_not_of('\n');
		if (string::npos == first)
		{
			return str;
		}
		size_t last = str.find_last_not_of('\n');
		return str.substr(first, (last - first + 1));
	}
	string extract_features(const string &image_file_path){
		int size = 231;
		CImg<double> input_image(image_file_path.c_str());
		
		CImg<double> gray = input_image.resize(size,size);
		
		gray.get_normalize(0,255).save("temp.png");
		string output = exec((string(OVERFEAT_PATH)+string(" -f ")+string("temp.png")).c_str());
		
		std::string delimiter = "\n";
		std::string token = trim(output.substr(output.find(delimiter)+1,output.length()-1)); 
		
		return token;
	}
};