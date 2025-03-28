/**
******************************************************************************
* @course   		: Image Processing with Deep Learning - HGU
* @author			: Jong-Hyeon Kim
* @Created			: 2025-03-20
* @mod				: 2025-03-28
* @brief			: [LAB] Grayscale Image Segmentation
******************************************************************************
*/

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

// Function declaration
void RadiusMeasure(Mat& target, Point2f& center, float& Radius);			// Measuring the radius. (Check the nearest pixel poing from the center)
double meanCal(vector<double>& arr, int size);								// Calulate mean value of vector array.

int main()
{
	// Raw Image file load by gray scale mode
	Mat Gear1 = imread("Gear1.jpg",0);
	Mat Gear2 = imread("Gear2.jpg", 0);
	Mat Gear3 = imread("Gear3.jpg", 0);
	Mat Gear4 = imread("Gear4.jpg", 0);

	// Raw Image file load by color mode
	Mat Gear1COL = imread("Gear1.jpg", IMREAD_COLOR);
	Mat Gear2COL = imread("Gear2.jpg", IMREAD_COLOR);
	Mat Gear3COL = imread("Gear3.jpg", IMREAD_COLOR);
	Mat Gear4COL = imread("Gear4.jpg", IMREAD_COLOR);

	// Background emtpy Image file using  raw image size 
	Mat drawing1(Gear1.size(), CV_8UC3, Scalar(0));
	Mat drawing2(Gear2.size(), CV_8UC3, Scalar(0));
	Mat drawing3(Gear3.size(), CV_8UC3, Scalar(0));
	Mat drawing4(Gear4.size(), CV_8UC3, Scalar(0));

	// Kernel for morphology.
	Mat KernelELL3x3 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3), Point(-1, -1));
	Mat KernelELL5x5 = getStructuringElement(MORPH_ELLIPSE, Size(5, 5), Point(-1, -1));
	Mat Kernelrect5x5 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
	Mat Kernelrect3x3 = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	Mat Kernelrect2x2 = getStructuringElement(MORPH_RECT, Size(2, 2), Point(-1, -1));

	// Space for Final contours Images.
	Mat FinalIMG1(Gear1.size(), CV_8UC3, Scalar(0));
	Mat FinalIMG2(Gear2.size(), CV_8UC3, Scalar(0));
	Mat FinalIMG3(Gear3.size(), CV_8UC3, Scalar(0));
	Mat FinalIMG4(Gear4.size(), CV_8UC3, Scalar(0));

	// Space for Gear image for Binarization.
	Mat Gear1Binary(Gear1.size(), CV_8U, Scalar(0));
	Mat Gear2Binary(Gear2.size(), CV_8U, Scalar(0));
	Mat Gear3Binary(Gear3.size(), CV_8U, Scalar(0));
	Mat Gear4Binary(Gear4.size(), CV_8U, Scalar(0));

	// Detecting function
	double tuningVal = 200.0;											// Tuning value for detecting.
	double detectValMin = 0.0;											// Value for detect.
	double detectValMax = 0.0;											// Value for detect.
	double medianVal = 0.0;												// Median Value of Area in Contours group.
	vector<double> arr(20);												// Array for sorting
	double TeethAreaMeanVal = 0.0;										// Mean value of Area in Contours group.

	// Contours
	vector<vector<Point>> contour1, contour2, contour3, contour4;		// Point for Contour processing(first)
	vector<vector<Point>> contour1P, contour2P, contour3P, contour4P;	// Point for Contour processing(second)
	
	int count = 0;																// Counting defective teeth.
	int thresholdVal = 128;														// Threshold Value
	Point2f centermin;															// To check each contours center and radius by using minEnclosing 
	float radiusmin;															// To check each contours center and radius by using minEnclosing 
	Mat drawing1Mod;															// Space for using modify drawing Images.
	Mat dst1;																	// Space for morphologying
	Mat dst2;																	// Space for morphologying
	Mat dst2Gray;																// Space for converting dst2 to GrayScale		
	Point2f center1, center2, center3, center4;									// Center for minEnclosing to get center of raw Gear Image.
	float mECradius1 = 0, mECradius2 = 0, mECradius3 = 0, mECradius4 = 0;		// Radius for minEnclosing. (Not important)
	float radius1 = 0, radius2 = 0, radius3 = 0, radius4 = 0;					// Diameter of the gear (We will use this for contour making)

	// Vectorization
	vector<Mat> Gears = { Gear1,Gear2,Gear3,Gear4 };											
	vector<Mat> GearsCOL = { Gear1COL,Gear2COL,Gear3COL,Gear4COL };								
	vector<Mat> GearBinaries = { Gear1Binary, Gear2Binary, Gear3Binary, Gear4Binary };
	vector<Mat> FinalIMGS = { FinalIMG1,FinalIMG2,FinalIMG3,FinalIMG4 };
	vector<Mat> drawings = { drawing1,drawing2,drawing3,drawing4 };
	vector<vector<vector<Point>>> contours1 = { contour1, contour2, contour3, contour4 };
	vector<vector<vector<Point>>> contours2 = { contour1P, contour2P, contour3P, contour4P };
	vector<Point2f> centers = {center1, center2, center3, center4 };
	vector<float> mECradii = { mECradius1, mECradius2, mECradius3, mECradius4 };
	vector<float> radii = { radius1, radius2, radius3, radius4 };
	
	// Process of Images
	for (int i = 0; i < Gears.size(); i++) {
		// ThreshHolding(Binaryization)
		threshold(Gears[i], GearBinaries[i], thresholdVal, 255, THRESH_BINARY);									// Convert Raw Image to Binary Image.
		
		// Find Contours
		findContours(GearBinaries[i], contours1[i], RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));		// Search Contours.
		drawContours(drawings[i], contours1[i], -1, Scalar(0, 255, 0), 2, 4, noArray());					// Drawing found Contours (linewidth = 2, linetype=4, Green Color). 
		
		// Find a circle surrounding the outline
		minEnclosingCircle(contours1[i][0], centers[i], mECradii[i]);										// Find minimun circle in Contour shape and return center and radius
		
		// Find a circle of ther Gear 
		RadiusMeasure(drawings[i], centers[i], radii[i]);													// Measure the Diameter of the gear(radii*2)
		circle(drawings[i], centers[i], radii[i]+1.5, Scalar(0, 255, 0), 2);								// Draw the circle using Diameter of the gear(radii*2) and tuning +1.5 value in radius.
		
		// Morphology 
		morphologyEx(drawings[i], drawing1Mod, MORPH_CLOSE, KernelELL3x3, Point(-1, -1), 1);				// Closing Morphology (iteration: 1)
		morphologyEx(drawing1Mod, drawing1Mod, MORPH_CLOSE, KernelELL3x3, Point(-1, -1), 1);				// Closing Morphology (iteration: 1)
		erode(drawing1Mod, dst1, KernelELL5x5, Point(-1, -1), 1);											// Erosing			  (iteration: 1)
		morphologyEx(dst1, dst1, MORPH_CLOSE, KernelELL5x5, Point(-1, -1), 4);								// Closing Morphology (iteration: 4)
		dilate(dst1, dst1, Kernelrect5x5, Point(-1, -1), 2);												// Dilating           (iteration: 2)
		morphologyEx(dst1, dst1, MORPH_CLOSE, KernelELL5x5, Point(-1, -1), 2);								// Closing Morphology (iteration: 2)
		
		dst2 = drawing1Mod - dst1;																			// Delete line between tooth and tooth 
		
		// Morphology 
		erode(dst2, dst2, KernelELL3x3, Point(-1, -1), 1);													// Erosing			  (iteration: 1)
		dilate(dst2, dst2, Kernelrect5x5, Point(-1, -1), 2);												// Dilating           (iteration: 2)
		morphologyEx(dst2, dst2, MORPH_CLOSE, Kernelrect3x3, Point(-1, -1), 1);								// Closing Morphology (iteration: 1)
		erode(dst2, dst2, Kernelrect3x3, Point(-1, -1),1);													// Erosing			  (iteration: 1)
		
		cvtColor(dst2, dst2Gray, COLOR_BGR2GRAY);															// Convert dst to Gray Scale.
		
		// Find Contours that only including teeth
		findContours(dst2Gray, contours2[i], RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));				// Find Contours Image that has only teeth parts.
		drawContours(FinalIMGS[i], contours2[i], -1, Scalar(0, 255, 0), 1, 4, noArray());					// Draw Contours Image that has only teeth parts on FinalIMGS.

		// Converting contours data in Array
		for (int j = 0; j < contours2[i].size(); j++)	{						// Number of Conturs : 20 
			arr[j] = contourArea(contours2[i][j]);
		}

		// Calculate Mean value
		TeethAreaMeanVal = meanCal(arr, contours2[i].size());					// Store the mean value by calling function.

		// Calculate Median value
		sort(arr.begin(), arr.end());							// Soring by upward.
		medianVal = arr[(int)(contours2[i].size() / 2)];		// Pick the median value.
		detectValMin = medianVal - tuningVal;					// Add tuning value to check defective tooth (MaxValue).
		detectValMax = medianVal + tuningVal;					// Add tuning value to check defective tooth (MinValue).
		
		/*Mark Out Defective teeth information*/
		count = 0;																								// Reset count value
		for (int t = 0; t < contours2[i].size(); t++) 
		{
			// Processing for put text
			string text = to_string((int)contourArea(contours2[i][t]));											// Convert Area value into text type.
			RotatedRect r = fitEllipse(contours2[i][t]);														// Find Circle that fit ellipse
			r.center -= Point2f(15, -10);																		// Tuning the ellipse center.
			
			// Checking defective teeth
			if (contourArea(contours2[i][t]) < detectValMin || contourArea(contours2[i][t]) > detectValMax) {	
				drawContours(FinalIMGS[i], contours2[i], t, Scalar(0, 0, 255), 1, 4, noArray());				// Make defective tooth Color to Red 
				minEnclosingCircle(contours2[i][t], centermin, radiusmin);										// Find circle information that surround defective teeth. 
				circle(GearsCOL[i], centermin, radiusmin, Scalar(0, 0, 255), 2);								// Draw circle to mark defective teeth by red color. 

				putText(FinalIMGS[i], text, r.center, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1.5);		// Mark defective tooth as red text using fit ellipse center (fontScale:0.5, LineWidth: 1.5)
				
				count++;																						// Count Number of defective teeth.
			}
			else putText(FinalIMGS[i], text, r.center, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1.5);	// Mark non-defective tooth ad white text using fit ellipse center (fontScale:0.5, LineWidth: 1.5)
		}

		// Print 
		cout << "============ Gear" << i+1 << " ============" << endl;
		cout << "Teeth number: "<<contours2[i].size() << endl;
		cout << "AVG.Teeth Area: " << TeethAreaMeanVal << endl;
		cout << "Diameter of the gear: " << radii[i]*2 << endl;
		cout << "Defective Teeth: " << count << endl;
		if (count == 0) cout << "Quality: Pass" << endl;
		else cout << "Quality: Fail" << endl;
	}
	// Plot
	imshow("Gear1Teeth", FinalIMGS[0]);
	imshow("Gear2Teeth", FinalIMGS[1]);
	imshow("Gear3Teeth", FinalIMGS[2]);
	imshow("Gear4Teeth", FinalIMGS[3]);
	imshow("Gear1", Gear1COL);
	imshow("Gear2", Gear2COL);
	imshow("Gear3", Gear3COL);
	imshow("Gear4", Gear4COL);

	waitKey(0);	// Wait until press any key.
}

// Calulate mean Value.
/*
arr		: Vector containing values to calculate 
size	: Vector size
*/
double meanCal(vector<double>& arr,int size) {
	double result = 0;
	for (int i = 0; i < size; i++) {
		result += arr[i];
	}
	return result / size;
}

// Measure radius betwwen center and nereast pixel that include data.
/*
target	: Image data
center	: center point
Radius  : return value(radius)
*/
void RadiusMeasure(Mat &target, Point2f &center, float &Radius) {
	float value = 0.0;		// Space for checking distance.
	Radius = 1000.0;		// Initial Value. 

	for (int i = 0; i < target.rows; i++) {											// Examine all rows.
		for (int j = 0; j < target.cols; j++) {										// Examine all cols.
			Vec3b pixel = target.at<Vec3b>(i, j);									// Store each pixel information into the pixel variable.
			if (pixel[0] == 0 && pixel[1] == 255 && pixel[2] == 0) {				// If data is detected!
				value = sqrt(pow((center.x - j), 2) + pow((center.y - i), 2));		// Check out the distance between center and current value
				if (Radius > value)													// If current distance is smaller than before, 
					Radius = value;													// Update current radius value(distance will be radius value)
			}
		}
	}
}
