
//Link to the header file
#include "CImg.h"
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <Sift.h>
#include<cmath>
#include<algorithm>
#include<time.h>
#include<climits>
//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;

#define DIST_THRES 40
const unsigned char color[] = { 64 };
class InputImage{

public: int matchDesc;
 int argNumber;
 string inputFile;
  CImg<double> gray;
    string name;
 vector<SiftDescriptor> descriptors;
};
int n;

static vector < int >*base_arr;
 
bool compar (int a, int b)
{
  return ((*base_arr)[a] < (*base_arr)[b]);
}

void sort_index_arr(int n ,vector<int> arr, vector<int> &idx){
	 /* initialize initial index permutation of unmodified `arr'
   */
  for (int i = 0; i < n; i++)
    {
      idx[i] = i;
    }
 
  base_arr = &arr;
  sort (idx.begin (), idx.end (), compar);
 
  cout << "\nOriginal list: ";
  for (int i = 0; i < n; i++)
    {
      cout << (*base_arr)[i] << " ";
    }
 
}

class Point{
  
    float x, y;
  public:
    Point(float a,float b):x(a),y(b){};
    float get_x(){
      return x;
    }
    float get_y(){
      return y;
    }
};

//functions for RANSAC
CImg<float> estimate_homography_dlt(int n, vector<Point> image1, vector<Point> image2){
  
  CImg<float> A_mat(9, 2*n);
  A_mat.fill(0);
  
  CImg<float> H_mat(3,3);
 
  for(int i=0;i<n;i++){
    A_mat(0,2*i) = -image1[i].get_x();
    A_mat(1,2*i) = -image1[i].get_y();
    A_mat(2,2*i) = -1;
    A_mat(6,2*i) = image1[i].get_x()*image2[i].get_x();
    A_mat(7,2*i) = image1[i].get_y()*image2[i].get_x();
    A_mat(8,2*i) = image2[i].get_x();
    
    A_mat(3,2*i+1) = -image1[i].get_x();
    A_mat(4,2*i+1) = -image1[i].get_y();
    A_mat(5,2*i+1) = -1;
    A_mat(6,2*i+1) = image1[i].get_x()*image2[i].get_y();
    A_mat(7,2*i+1) = image1[i].get_y()*image2[i].get_y();
    A_mat(8,2*i+1) = image2[i].get_y();
  }
  
  CImgList<float> svd_mat = A_mat.get_SVD();
   
  H_mat(0,0) = svd_mat[2](8,0);
  H_mat(1,0) =  svd_mat[2](8,1);
  H_mat(2,0) = svd_mat[2](8,2);
  H_mat(0,1) = svd_mat[2](8,3);
  H_mat(1,1) = svd_mat[2](8,4);
  H_mat(2,1) = svd_mat[2](8,5);
  H_mat(0,2) = svd_mat[2](8,6);
  H_mat(1,2) = svd_mat[2](8,7);
  H_mat(2,2) =  svd_mat[2](8,8);

 
  return H_mat;
  
}

//functions for RANSAC
vector<Point> normalization(int n, vector<Point> points, CImg<float> &T_mat){
  //CImg<float> T_mat(3, 3);
  //T_mat.fill(0);
  
  CImg <float> temp_mat(1,3);
  temp_mat.fill(1);

  vector<Point> new_points;
  new_points.reserve(n);

  float mean_x, mean_y, scale,temp;
  mean_x = 0;
  mean_y = 0;
  temp = 0;
  //Mean Calculation
  for(int i=0; i<n; i++){
    mean_x += points[i].get_x();
    mean_y += points[i].get_y();
  }
  
  mean_x = mean_x/(float) n;
  mean_y = mean_y/(float) n;
  
  for(int i=0; i<n; i++){
    temp += sqrt(pow(points[i].get_x()-mean_x,2))+sqrt(pow(points[i].get_y()-mean_y,2));
  }
  temp = temp/(float)n;
  scale = sqrt(2)/temp;
  //Transformation matrix
  T_mat(0,0) = scale;
  T_mat(2,0) = -scale*mean_x;
  T_mat(1,1) = scale;
  T_mat(2,1) = -scale*mean_y;
  T_mat(2,2) = 1;
  
  //Transforming all points
  for(int i=0; i<n; i++){
    temp_mat(0,0) = points[i].get_x();
    temp_mat(0,1) = points[i].get_y();
    temp_mat = T_mat * temp_mat;
    Point point(temp_mat(0,0)/(float) temp_mat(0,2),temp_mat(0,1)/(float) temp_mat(0,2));
    new_points.push_back(point); 
  }
  return new_points;
}

//functions for RANSAC
int inliers_count(int n, vector<Point> image1, vector<Point> image2, CImg<float> h_mat, int *outliers){
  //int outliers[n] ;
  float dist = 0;
  CImg <float> temp_mat(1,3);
  CImg<float> inv_h_mat = h_mat.get_invert();
  int count = 0;
  for(int i=0; i<n; i++){
    temp_mat.fill(1);
    temp_mat(0,0) = image1[i].get_x();
    temp_mat(0,1) = image1[i].get_y();
    temp_mat =  h_mat * temp_mat;
    Point point1(temp_mat(0,0)/(float) temp_mat(0,2),temp_mat(0,1)/(float) temp_mat(0,2));
    dist = pow(image2[i].get_x()-point1.get_x(),2)+pow(image2[i].get_y()-point1.get_y(),2);
    
    temp_mat.fill(1);
    temp_mat(0,0) = image2[i].get_x();
    temp_mat(0,1) = image2[i].get_y();
    temp_mat = inv_h_mat * temp_mat;
    Point point2(temp_mat(0,0)/(float) temp_mat(0,2),temp_mat(0,1)/(float) temp_mat(0,2));
    dist += pow(image1[i].get_x()-point2.get_x(),2)+pow(image1[i].get_y()-point2.get_y(),2);
    
    if(dist < DIST_THRES){
      count++;
	  outliers[i] = 0;
	  
    }
    
  }
  
  return count;
}

bool contains(int a[], int x){
  size_t myArraySize = sizeof(a) / sizeof(int);
  int *end = a + myArraySize;
  bool exists =  std::find(a, end, x) != end;
  return exists;
}
void test_inliers(int outliers[],int n){
	
	for(int i=0;i<n;i++){
		if(outliers[i] == 0){
			//cout << "test inliers:i:" << i << endl;
		}
	}
}
/*RANSAC
 */
int * Ransac(int n, vector<Point> image1, vector<Point> image2, CImg<float> &best_h_mat,int &num_max){
  CImg<float> T_mat_image1(3, 3), T_mat_image2(3,3);
  T_mat_image1.fill(0);
  T_mat_image2.fill(0);
  int outliers_temp[n] ;
  std::fill_n(outliers_temp, n, 1);
  int *outliers = new int[n];
  //int outliers[n];
  //int num_max = -1;
  //Parameters
  //thres - threshold used to identify a point that fits well
  //k - number of iterations
  float p = 0.9;
  float thres;
  int k=1000, iter=0;
  int sample_index[4];
  int num_inliers = 0;
  float e;
  vector<Point> sample_image1;
  vector<Point> sample_image2;
  sample_image1.reserve(4);
  sample_image2.reserve(4);

  srand(time(NULL));
  while(k>iter){
	  sample_image1.clear();
	  sample_image2.clear();
    //randomly sample 4 points
    for(int j=0; j<4; j++){
      int r;
      do{
        r = rand() % n;
      }while(contains(sample_index,r));
      sample_index[j] = r;
      Point point_image1(image1[sample_index[j]].get_x(),image1[sample_index[j]].get_y());
      Point point_image2(image2[sample_index[j]].get_x(),image2[sample_index[j]].get_y());
      sample_image1.push_back(point_image1);
      sample_image2.push_back(point_image2);
    }
    
    //Normalization
    sample_image1 = normalization(4,sample_image1,T_mat_image1);
    sample_image2 = normalization(4,sample_image2,T_mat_image2);
    //DLT to estimate Homography
    CImg<float> h_mat = estimate_homography_dlt(4,sample_image1,sample_image2);
    T_mat_image2.invert();
    h_mat = T_mat_image2 * h_mat;
    h_mat = h_mat * T_mat_image1;
    num_inliers = inliers_count(n, image1, image2, h_mat,outliers_temp);
	
	//updating homography matrix
	if(num_inliers > num_max){
		num_max = num_inliers;
		best_h_mat = h_mat;
		outliers = outliers_temp;
		
		//std::fill_n(outliers_temp, n, 1);
	}
	
	e = 1-(num_inliers/(float)n);
	//cout << "e:" << e << endl;
	//k = (int)(log(1-p)/log(1-pow(1-e,4)));
	//cout << "k:" << k << endl;
	//cout << "iter:" << iter << endl;
	iter ++;
  }
  return outliers;
}




//functions for RANSAC
double calculateEucidean(SiftDescriptor desc1,SiftDescriptor desc2)
{
    double diff = 0;
    for(int i = 0;i<128;i++)
    {
        diff+= (desc1.descriptor[i] - desc2.descriptor[i])*(desc1.descriptor[i] - desc2.descriptor[i]);
    }

    diff = sqrt (diff);
    
    return diff;
}


bool checkSmaller (InputImage obj1 , InputImage obj2) { return (obj1.matchDesc > obj2.matchDesc); }

//functions for RANSAC
double * matchDescriptors( vector<SiftDescriptor> descriptor1, vector<SiftDescriptor> descriptor2, SiftDescriptor closeDes[][2], double desDist[][2])
{
    double *ratio = new double[descriptor1.size()];
    int count = 0;
    
    int M = descriptor1.size();
    int N = descriptor2.size();
    SiftDescriptor clos1;
    SiftDescriptor close2;
    double diff;
    double max = INT_MIN;
    double min = INT_MAX;
    
    for(int i=0; i<M; i++)
    {
        for(int j=0; j<N; j++)
        {
            diff = calculateEucidean(descriptor1[i], descriptor2[j]);
            if(diff < desDist[i][0])
            {
                desDist[i][1] = desDist[i][0];
                desDist[i][0] = diff;
                closeDes[i][1] = closeDes[i][0];
                closeDes[i][0] = descriptor2[j];
            }
            else if(diff >= desDist[i][0] && diff < desDist[i][1])
            {
                desDist[i][1] = diff;
                closeDes[i][1] = descriptor2[j];
            }
        }
    }
    
    
    for(int i = 0; i<M; i++)
    {
        ratio[i] = (desDist[i][0]/desDist[i][1]);
        
        if(max < ratio[i])
            max = ratio[i];
        if(min > ratio[i])
            min = ratio[i];
    }
    
    
    

    return ratio;
}

//functions for RANSAC
void get_sift_matches(CImg<double> input_image,CImg<double> input_image1,vector<SiftDescriptor> descriptors , vector<SiftDescriptor> descriptors1, vector<Point> &image1_match, vector<Point> &image2_match){
  double *p;
  double closest[descriptors.size()][2];
  SiftDescriptor closeDesc[descriptors.size()][2];
  
  for(int i = 0; i< descriptors.size();i++)
  {
	  closest[i][0] =  INT_MAX;
	  closest[i][1] =  INT_MAX;
	  closeDesc[i][0] = descriptors[i];
	  closeDesc[i][1] = descriptors[i];
  }
        
          
  double diff1 = calculateEucidean(descriptors[3], descriptors1[3]);
  
  
  p = matchDescriptors(descriptors, descriptors1, closeDesc, closest);
  
  double wid = input_image.width();
  input_image.append(input_image1,'x', 0);
  const unsigned char color[] = { 64 };

  for(int i=0; i<descriptors.size(); i++)
  {
	 
	  if(p[i] < 0.8)
	  {
		  for(int j=0; j<5; j++)
			  for(int k=0; k<5; k++)
				  if(j==2 || k==2)
					  for(int p=0; p<3; p++)
					  {
						  input_image(descriptors[i].col+k-1, descriptors[i].row+j-1, 0, p)=0;
						  input_image(closeDesc[i][0].col+wid+k-1, closeDesc[i][0].row+j-1, 0, p)=0;
					  }
		  Point point1(descriptors[i].col-1,descriptors[i].row-1);
		  Point point2(closeDesc[i][0].col-1,closeDesc[i][0].row-1);
		  image1_match.push_back(point1);
		  image2_match.push_back(point2);
		  input_image.draw_line(descriptors[i].col-1, descriptors[i].row-1,closeDesc[i][0].col+wid-1, closeDesc[i][0].row-1, color);
	  }
	
  }
  input_image.get_normalize(0,255).save("sift.png");
}

//functions for RANSAC
void sift_matches_part1(CImg<double> input_image,CImg<double> input_image1, vector<Point> &image1_match, vector<Point> &image2_match ){
	
    
    
    // convert image to grayscale
	CImg<double> gray = input_image.get_RGBtoHSI().get_channel(2);
    CImg<double> gray1 = input_image1.get_RGBtoHSI().get_channel(2);
	
    vector<SiftDescriptor> descriptors = Sift::compute_sift(gray);
    vector<SiftDescriptor> descriptors1 = Sift::compute_sift(gray1);
        
    get_sift_matches(input_image, input_image1,descriptors, descriptors1,image1_match,image2_match);
    
}

//functions for RANSAC
int ransac_image_pair(CImg<double> input_image, CImg<double> input_image1, CImg<float> &best_h_mat){
	vector<Point> image1_match;
	vector<Point> image2_match; 
	sift_matches_part1(input_image,input_image1,image1_match, image2_match );
	
	n=image1_match.size();
	
	//CImg<float> h_mat = estimate_homography_dlt(n,image1_match,image2_match);
	//cout << "1,0:" <<h_mat(1,0) << endl;
	int *outliers1 ;
	
	int count=-1;
	outliers1 = Ransac( n, image1_match,image2_match, best_h_mat,count);
	
	float wid = input_image.width();
	input_image.append(input_image1,'x', 0);
	const unsigned char color[] = {0,238,0};
	for(int i=0; i<n; i++){
	  
	  if(outliers1[i] == 0){
	   // green lines --> Inliers
		
		input_image.draw_line(image1_match[i].get_x()-1, image1_match[i].get_y()-1,image2_match[i].get_x()+wid-1, image2_match[i].get_y()-1, color);
	  }
	}
	input_image.get_normalize(0,255).save("ransac.png");
	return count;
}

double uniRand()
{
    return rand() / double(RAND_MAX);
}

double uniRand(double a,double b){
	return (b-a)*uniRand() + a;
}

//functions for RANSAC
vector< vector<double> >  get_vector_x(){
	vector< vector<double> > x;
	int k=2;
	srand(time(0));
	for(int i =0; i< k;i++){
		vector<double> x_temp;
		
		for(int j=0;j<128;j++){
			
			x_temp.push_back(uniRand(0,1));
			
		}
		
		x.push_back(x_temp);
	}
	return x;
}

double get_product_val(vector<double> &x ,vector<float> &v){
	double sum =1.0;
	int w = 100;
	for(int i=0;i<128;i++){
		sum = sum+ (v[i] *x[i]) ;
		
	}
	
	int y = floor(sum/w);
	
	return y;
}

//functions for RANSAC
vector<double> reduce_dimension(vector<float> &v,vector< vector<double> > x){
	
	vector<double> summary_vector;
	for(int i=0;i< x.size();i++){
		double pval = get_product_val(x[i],v);
		summary_vector.push_back(pval);
	}
	
	return summary_vector;
}

//functions for RANSAC
bool compare_vector_identity(vector<double> v1, vector<double> v2){
	for(int i=0; i<v1.size();i++){
		if(v1[i] != v2[i]){
			return false;
		}
		
	}
	return true;
}

void get_match_quantized_proj_func(CImg<double> input_image,CImg<double> input_image1){
	//CImg<double> input_image(inputFile.c_str());
	
	vector< vector<double> > k_d_summ_vectors_image1;
	vector< vector<double> > k_d_summ_vectors_image2;
	
	// convert image to grayscale
	CImg<double> gray = input_image.get_RGBtoHSI().get_channel(2);
    CImg<double> gray1 = input_image1.get_RGBtoHSI().get_channel(2);
	
	vector<SiftDescriptor> descriptors = Sift::compute_sift(gray);
	vector<SiftDescriptor> descriptors1 = Sift::compute_sift(gray1);
	
	int iter = 2;
	vector< vector<int> > image1_match(descriptors.size());
	for(int l=0;l<iter;l++){
		vector< vector<double> > x = get_vector_x();
		k_d_summ_vectors_image1.clear();
		k_d_summ_vectors_image2.clear();
		//image1
				
		for(int i=0;i<descriptors.size();i++){
		
			vector<double> summary_vec = reduce_dimension(descriptors[i].descriptor,x);
			k_d_summ_vectors_image1.push_back(summary_vec);
			
		}
		
		for(int i=0;i<descriptors1.size();i++){
		
			vector<double> summary_vec = reduce_dimension(descriptors1[i].descriptor,x);
			k_d_summ_vectors_image2.push_back(summary_vec);
			
		}
			
		
		for(int i=0;i<k_d_summ_vectors_image1.size();i++){
			
			for(int j=0;j<k_d_summ_vectors_image2.size();j++){
				
				if(compare_vector_identity(k_d_summ_vectors_image1[i],k_d_summ_vectors_image2[j])){
					if(std::find(image1_match[i].begin(), image1_match[i].end(), j) == image1_match[i].end()){
						image1_match[i].push_back(j);
					}
					
				}
				
			}
		}
		
	}
	double desDist[descriptors.size()][2];
    SiftDescriptor closeDes[descriptors.size()][2];
	
	for(int i = 0; i< descriptors.size();i++)
          {
              desDist[i][0] =  INT_MAX;
              desDist[i][1] =  INT_MAX;
              closeDes[i][0] = descriptors[i];
              closeDes[i][1] = descriptors[i];
          }
	
	
	double diff;
    
	for(int i=0; i<image1_match.size();i++){
		vector<int> temp = image1_match[i];
		for(int j=0; j<temp.size();j++){
			diff = calculateEucidean(descriptors[i], descriptors1[temp[j]]);
            if(desDist[i][1] ==0)
            {
                desDist[i][1] = 1;
            }
            if(diff < desDist[i][0])
            {
                desDist[i][1] = desDist[i][0];
                desDist[i][0] = diff;
                closeDes[i][1] = closeDes[i][0];
                closeDes[i][0] = descriptors1[j];
            }
            else if(diff >= desDist[i][0] && diff < desDist[i][1])
            {
                desDist[i][1] = diff;
                closeDes[i][1] = descriptors1[j];
            }
		}
	}
	
	double ratio[descriptors.size()];
	double max = INT_MIN;
    double min = INT_MAX;
	for(int i = 0; i<descriptors.size(); i++)
    {
        ratio[i] = (desDist[i][0]/desDist[i][1]);
        
        if(max < ratio[i])
            max = ratio[i];
        if(min > ratio[i])
            min = ratio[i];
    }
	
	
          double wid = input_image.width();
          input_image.append(input_image1,'x', 0);
          
    const unsigned char color[] = { 64 };
	for(int i=0; i<descriptors.size(); i++)
	  {
          if(ratio[i] < 0.8)
          {
              for(int j=0; j<5; j++)
                  for(int k=0; k<5; k++)
                      if(j==2 || k==2)
                          for(int p=0; p<3; p++)
                          {
                              input_image(descriptors[i].col+k-1, descriptors[i].row+j-1, 0, p)=0;
                              input_image(closeDes[i][0].col+wid+k-1, closeDes[i][0].row+j-1, 0, p)=0;
                          }
              input_image.draw_line(descriptors[i].col-1, descriptors[i].row-1,closeDes[i][0].col+wid-1, closeDes[i][0].row-1, color);
              
          }
        
	  }
	input_image.get_normalize(0,255).save("sift_quantized.png");
}

int main(int argc, char **argv)
{
	clock_t tStart = clock();
  try {

    if(argc < 2)
      {
	cout << "Insufficent number of arguments; correct usage:" << endl;
	cout << "    a2 part_id ..." << endl;
	return -1;
      }

    string part = argv[1];
    string inputFile = argv[2];
    
   
    if(part == "ransac")
      {
		  string ip1 = argv[3];
	
	    CImg<double> input_image(inputFile.c_str());
		int count = 0;
		vector<int> correspondence_points_count, idx;
		correspondence_points_count.resize(argc-3);
		idx.resize(argc-3);
		
		for(int i=0; i<argc-3;i++){
		  string ip1 = argv[i+3];
		  CImg<double> input_image1(ip1.c_str());
		
		  CImg<float> best_h_mat(3,3);
		  count = ransac_image_pair( input_image, input_image1, best_h_mat);
		  
		  correspondence_points_count[i]=count;
		  clock_t ransac_end = clock();
		  cout << "Time taken for Ransac:" <<(double)(ransac_end - tStart)/CLOCKS_PER_SEC << endl;
		  get_match_quantized_proj_func(input_image,input_image1);
		  clock_t quantized_end = clock();
		  cout << "Time taken for quantized sift:" << (double)(quantized_end - ransac_end)/CLOCKS_PER_SEC << endl;
		}
        //std::sort (correspondence_points_count.begin(), correspondence_points_count.end());
		sort_index_arr(argc-3 ,correspondence_points_count, idx); 
		cout<<"\nMatched images in descending order of inliers match: " << endl;
		for(int j=argc-4;j>=0;j--){
			cout << argv[idx[j]+3] << endl ;
		}

		
      }
    
    else
      throw std::string("unknown part!");

    // feel free to add more conditions for other parts (e.g. more specific)
    //  parts, for debugging, etc.
  }
  catch(const string &err) {
    cerr << "Error: " << err << endl;
  }
}








