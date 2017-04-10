# Image Matching using RANSAC and Quantized Projection Function of SIFT vector
Estimates Homography between images using RANSAC with SIFT features. It detects inliers and outliers of correspondence points between the images.
Determines matching correspondences between images and generates visualization of matches.

Generates summary vectors of 128 dimensional SIFT vectors using Quantized Projection Function to improve running time and compare performances.

## Running the tests
./a2 part image_1 image_2 ... image_n  
part - ransac 
