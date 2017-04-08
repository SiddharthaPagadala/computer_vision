#include <limits>
class Cluster{
public:
	vector<float> centroid;
	vector< vector<float> > cluster_elements;
	
	Cluster(vector<float> centroid_inp):centroid(centroid_inp){}
	Cluster(vector< vector<float> > cluster_elements_inp):cluster_elements(cluster_elements_inp){}
	
	void add_cluster_element(vector<float> element){
		cluster_elements.push_back(element);
	}
	vector<float> get_centroid(){
		return centroid;
	}
	void set_centroid(vector<float> centroid_inp){
		centroid = centroid_inp;
	}
	void clear_cluster_elements(){
		cluster_elements.clear();
	}
	vector<float> get_sum(vector<float> a, vector<float> b){
		std::transform (a.begin(), a.end(), b.begin(), a.begin(), std::plus<int>());
		return a;
	}
	
	void print_vector(vector<float> temp){
		for(int i=0;i<temp.size();i++){
			cout << "vector print : i:" << i << temp[i] << endl;
		}
	}
	vector<float> get_mean(){
		
		std::vector<float> temp(cluster_elements[0].size(), 0.0);
		for(int i=0;i<cluster_elements.size();i++){
			temp = get_sum(temp,cluster_elements[i]);
		}
		cout << "cluster_elements.size():" << cluster_elements.size() << endl;
		float div;
		if(cluster_elements.size() == 0){
			div = std::numeric_limits<float>::min();
		}else{
			div = (1/(float)cluster_elements.size());
		}
		
		std::transform(temp.begin(), temp.end(), temp.begin(),std::bind2nd(std::multiplies<float>(),div ));
		
		vector<float> temp1;
		temp1 = temp;
		return temp1;
	}
};