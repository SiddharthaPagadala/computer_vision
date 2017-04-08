//Kmeans to cluster the data
//Used to construct vocabulary 
#include<Cluster.h>
class KMeans{
public:
	vector< vector<float> > data;
	int k;
	KMeans(vector< vector<float> > data_inp,int k_inp):data(data_inp),k(k_inp){}
	
	
	void get_initial_centroids(vector< vector<float> > all_patches, vector<Cluster> &initial_random_centroids){
		
		get_k_random_points(k,all_patches.size(),all_patches,initial_random_centroids);
	}
	
	bool contains(int a[], int x){
	  size_t myArraySize = sizeof(a) / sizeof(int);
	  int *end = a + myArraySize;
	  bool exists =  std::find(a, end, x) != end;
	  return exists;
	}

	void get_k_random_points(int k, int n, vector< vector<float> > &data, vector<Cluster> &clusters){
		int sample_index[k];
		srand(time(NULL));
		for(int i=0;i<k;i++){
			int r;
			do{
				r= rand()%n;
			}while(contains(sample_index,r));
			sample_index[i] = r;
			Cluster cluster(data[r]);
			clusters.push_back(cluster);
		}
		
		
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
	
	void assign_data_to_clusters(vector< vector<float> > &data,vector<Cluster> &clusters){
		double min;
		int min_cluster_index;
		for(int i=0;i<data.size();i++){
			min=9999999999999;
			min_cluster_index = 0;
			for(int j=0;j<clusters.size();j++){
				double temp = calculateEucidean(data[i],clusters[j].get_centroid());
				if(temp < min){
					min=temp;
					min_cluster_index = j;
				} 
			}
			
			clusters[min_cluster_index].add_cluster_element(data[i]);
			
		}
	}
	void update_centroids(vector< vector<float> > &data,vector<Cluster> &clusters){
		int iter =0;
		do{
			cout << "iteration:" << iter << endl;
			for(int j=0;j<clusters.size();j++){
				clusters[j].clear_cluster_elements();
			}
			assign_data_to_clusters(data, clusters);
			for(int i=0; i<clusters.size();i++){
				clusters[i].set_centroid(clusters[i].get_mean());
			}
			iter++;
		}while(iter < 10);
		
	}
	
	vector<Cluster> k_means_alg(vector< vector<float> > &vocabulary){
		vector<Cluster> clusters;
		
		get_initial_centroids(data,clusters);
		update_centroids(data,clusters);
		
		for(int i=0;i<clusters.size();i++){
			vocabulary.push_back(clusters[i].get_centroid());
		}
		
		return clusters;
	}
};