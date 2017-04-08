//
//  SVM.h
//  

#include <fstream>
#include <vector>
#include <string>
#include<sstream>
#include <iterator>
#include <typeinfo>

class SVM : public Classifier
{
public:
	class_encode class_encoding1 ;
    SVM(const vector<string> &_class_list) : Classifier(_class_list) {
		
		//creates map of classes with index from class_list
		for(int i=0;i<class_list.size();i++){
			class_encoding1[class_list[i]] = i+1;
		}
		
	}
	
	
    
    // SVM training.
    virtual void train(const Dataset &filenames, string algo)
    {
		if(algo == "baseline"){
			ofstream outputFile;
			outputFile.open("train_data.dat");
			cout<<"output file opened";
			int target = 1;
			
			for(Dataset::const_iterator c_iter=filenames.begin(); c_iter != filenames.end(); ++c_iter)
			{
				
				cout << "Processing " << c_iter->first << endl;
				CImg<double> class_vectors(size*size*3, c_iter->second.size(), 1);
				
				// convert each image to be a row of this "model" image
				
				for(int i=0; i<c_iter->second.size(); i++)
				{
					CImg<double> features = extract_features(c_iter->second[i].c_str());
					
					outputFile<< (target)<<" ";
					
					for(int k = 0; k < features.size(); k++)
					{
						outputFile<<(k+1)<<":"<<features[k]<<".0 ";
					}
					
					outputFile<<"\n";
				}
				target +=1;
				
			}
			outputFile.close();
			cout<<"OUTPUT CLOSE--------------------\nn\n";
			system("./svm_light1/svm_multiclass/svm_multiclass_learn -c 5000 train_data.dat model_create");
		}else if(algo == "haar"){
			system("./svm_light1/svm_multiclass/svm_multiclass_learn -c 50 data_haar.dat model_create");
		}else if(algo == "bow"){
			system("./svm_light1/svm_multiclass/svm_multiclass_learn -c 50 data_histogram.dat model_create");
		}else if(algo == "deep"){
			cout << "training started" << endl;
			system("./svm_light1/svm_multiclass/svm_multiclass_learn -c 10 deep_features.dat model_create");
			cout << "training completed" << endl;
		}
        
    }
    
    void test(const Dataset &filenames, string algo)
    {
		if(algo == "baseline"){
			cerr << "Loading model " << endl;
			//load_model();
			ofstream outputFile;
			outputFile.open("test_data.dat");
			cout<<"\ntest file opened\n";
			map< string, double > names;
			
			int target = 1;
			
			for(Dataset::const_iterator c_iter=filenames.begin(); c_iter != filenames.end(); ++c_iter)
			{
				
				cout << "Processing " << c_iter->first << endl;
				
				for(int i=0; i<c_iter->second.size(); i++)
				{
					CImg<double> features = extract_features(c_iter->second[i].c_str());
					
					outputFile<< (target)<<" ";
					
					for(int k = 0; k < features.size(); k++)
					{
						outputFile<<(k+1)<<":"<<features[k]<<".0 ";
					}
					outputFile<<"\n";
				}
				target +=1;
				
				
			}
			outputFile.close();
			cout<<"OUTPUT CLOSE--------------------\nn\n";
		}else if(algo== "eigen"){
			CImg<double> eigen_mat("eigen_face.txt");
			eigen_mat.transpose();
			
			
			CImg<double>avg_image("avg_image.txt");
			
			cerr << "Loading model " << endl;
			
			// load_model();
			
			ofstream outputFile;
			outputFile.open("test_data.dat");
			cout<<"\ntest file opened\n";
			map< string, double > names;
			
			int target = 1;
			
			for(Dataset::const_iterator c_iter=filenames.begin(); c_iter != filenames.end(); ++c_iter)
			{
				
				cout << "Processing " << c_iter->first << endl;
				
				for(int i=0; i<c_iter->second.size(); i++)
				{
					CImg<double> features = extract_features(c_iter->second[i].c_str());
					
					features -=avg_image;
					
					CImg<double> updated_features = features*eigen_mat;
					
					outputFile<< (target)<<" ";
					
					for(int k = 0; k < updated_features.size(); k++)
					{
						outputFile<<(k+1)<<":"<<updated_features[k]<<" ";
					}
					outputFile<<"\n";
				}
				target +=1;
				
				
			}
			outputFile.close();
			cout<<"OUTPUT CLOSE--------------------\nn\n";
		}else if(algo == "haar"){
			cout << "Loading model " << endl;
			
		}else if(algo == "bow"){
			cerr << "Loading model " << endl;
			//load_model();
			
			BagOfVisualWords bovw(10);
			Data_Histograms data_hist = bovw.get_test_image_features(filenames);
			
			ofstream out("test_data.dat");
			for(Data_Histograms::const_iterator it=data_hist.begin();it!=data_hist.end();++it){
				for(int i=0; i<it->second.size();i++){
					out << class_encoding1[it->first] << " ";
					for(int j = 0; j < it->second[i].size(); j++)
					{
						out<<j+1<<":"<<it->second[i][j]<< " ";
					}
					//std::copy(it->second[i].begin(), it->second[i].end(), ostream_iterator<int>(out, " "));
					out << endl;
				}
				
			}
			out.close();
			
		}else if(algo == "deep"){
			cerr << "Loading model " << endl;
			
		}
        
        
        
        system("./svm_light1/svm_multiclass/svm_multiclass_classify test_data.dat model_create prediction");
        
        ifstream testFile ("test_data.dat");
        ifstream predictedFile ("prediction");
        string line;
        string line1;
        int confusionMatrix[class_list.size()][class_list.size()];
        for(int x = 0; x< class_list.size(); x++)
            for(int y = 0; y< class_list.size(); y++)
            {
                confusionMatrix[x][y] = 0;
            }
        
        int correct=0, total=0;
        
        if (predictedFile.is_open())
        {
            
            while ( getline (predictedFile,line) && getline(testFile, line1))
            {
                std::istringstream buf(line);
                std::istream_iterator<std::string> beg(buf), end;
                
                std::vector<std::string> tokens(beg, end);
                
                
                std::istringstream buf1(line1);
                std::istream_iterator<std::string> beg1(buf1), end1;
                
                std::vector<std::string> tokens1(beg1, end1);
                
                
                string st = tokens[0];
                string st1 = tokens1[0];
                
                int t = std::atoi(tokens[0].c_str());
                int t1 = std::atoi(tokens1[0].c_str());
                confusionMatrix[t1-1][t-1]++;
                
                if(tokens[0] == tokens1[0])
                {
                    correct++;
                }
                total++;
                
            }
            
            testFile.close();
        }
        
        cout<<correct<<"\t correct\n";
        cout<<total<<"\t total\n";
        
        cout << "Confusion matrix:" << endl << setw(20) << " " << " ";
        
        for(int j=0; j<class_list.size(); j++)
            cout << setw(2) << class_list[j].substr(0, 2) << " ";
        
        for(int i=0; i<class_list.size(); i++)
        {
            cout << endl << setw(20) << class_list[i] << " ";
            for(int j=0; j<class_list.size(); j++)
                cout << setw(2) << confusionMatrix[i][j] << (j==i?".":" ");
        }
        
        cout << endl << "Classifier accuracy: " << correct << " of " << total << " = " << setw(5) << setprecision(2) << correct/double(total)*100 << "%";
        cout << "  (versus random guessing accuracy of " << setw(5) << setprecision(2) << 1.0/class_list.size()*100 << "%)" << endl;
        
    }
    
    virtual string classify(const string &filename)
    {
        CImg<double> test_image = extract_features(filename);
        
        // figure nearest neighbor
        pair<string, double> best("", 10e100);
        double this_cost;
        for(int c=0; c<class_list.size(); c++)
            for(int row=0; row<models[ class_list[c] ].height(); row++)
                if((this_cost = (test_image - models[ class_list[c] ].get_row(row)).magnitude()) < best.second)
                    best = make_pair(class_list[c], this_cost);
        
        return best.first;
    }
    
    virtual void load_model()
    {
        for(int c=0; c < class_list.size(); c++)
            models[class_list[c] ] = (CImg<double>(("nn_model." + class_list[c] + ".png").c_str()));
    }
protected:
	static const int size=30;  // subsampled image resolution
    map<string, CImg<double> > models; // trained models
    // extract features from an image, which in this case just involves resampling and
    // rearranging into a vector of pixel data.
    CImg<double> extract_features(const string &filename)
    {
        return (CImg<double>(filename.c_str())).resize(size,size,1,3).unroll('x');
    }
   
	void CIMgWriteFile(CImg<double> &img,string path,string cstr){
		string fullpath= path+cstr;
		img.resize(size,size,1,3).get_normalize(0,255).save(fullpath.c_str());
		// ofstream file(path);
		// file<<img.get_normalize(0,255).save(cstr);
	}
    
};
