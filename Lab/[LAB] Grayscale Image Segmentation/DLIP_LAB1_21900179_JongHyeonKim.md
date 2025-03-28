# LAB: Grayscale Image Segmentation

**Date:**  2025-March-28

**Author:**  JongHyeon Kim  21900179

**Github:** [repository link](https://github.com/Dandelion-Kim/DLIP_2025/tree/main)

---



# Introduction
## 1. Objective
**Goal**: Image analysis of the gear determines whether the gear is defective or not.

There is a gear with a defective gear tooth part. We can detect whether it is defective by calculating the area it occupies.



## 2. Preparation

### Software Installation

- OpenCV 4.9.0,  Visual Studio 2022

  

### Dataset

**Dataset link:** [Download the test image](https://github.com/Dandelion-Kim/DLIP_2025/tree/main/Image/Lab1_TestIMG)






# Algorithm

## 1. Overview

| ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1_TestIMG/Gear1.jpg?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1_TestIMG/Gear2.jpg?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1_TestIMG/Gear3.jpg?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1_TestIMG/Gear4.jpg?raw=true) |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: | ------------------------------------------------------------ |
|                            Gear 1                            |                            Gear 2                            |                            Gear 3                            | Gear 4                                                       |



<img src="https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/DLIP_LAB1_Flowchart.png?raw=true"  >

We will load Gear Images first. after processing, we can find out which Gear teeth are defective by the Plot results.  



## 2. Procedure

### Binarization 

To find Contours, we have to convert an image to Binary data. So, we have to use the Threshold function in openCV. 

|                     Before Binarization                      |                      After Binarization                      |
| :----------------------------------------------------------: | :----------------------------------------------------------: |
| ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Binarization_Before.png?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Binarization_After.png?raw=true) |

As we can see image above, after binarization, we can only see the intensities 0 and 255. I gave threshold value as 128.

```c++
threshold(Gears[i], GearBinaries[i], g_thresh, 255, THRESH_BINARY);	// Convert Raw Image to Binary Image.
```



### Find Contours

To make contours teeth only, we have to find contours twice, and this is the first processing to find outer contours.  

<image>

```c++
findContours(GearBinaries[i], contours1[i], RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));	
drawContours(drawings[i], contours1[i], -1, Scalar(0, 255, 0), 2, 4, noArray());			
```



### Find the center of Gear using Contours

To find center of Gear, we have to find a circle that surrounding Gear and find the center. So, we can use minEnclosingCircle function in openCV to in order to find the center point. Find outer circle is not important for this lab. We need Gear inside of circle.

```
minEnclosingCircle(contours1[i][0], centers[i], mECradii[i]);	
```



### Draw a circle inside of the Gear

After earn the center point, we have to make an inner circle line to make contours that surround teeth only. By using RadiusMeasure function that personaly make, we can find the inner circle line. 



```c++
void RadiusMeasure(Mat &target, Point2f &center, float &Radius) {
	float value = 0.0;		// Space for checking distance.
	Radius = 1000.0;		// Initial Value. 

	for (int i = 0; i < target.rows; i++) {											
		for (int j = 0; j < target.cols; j++) {										
			Vec3b pixel = target.at<Vec3b>(i, j);									
			if (pixel[0] == 0 && pixel[1] == 255 && pixel[2] == 0) {				
				value = sqrt(pow((center.x - j), 2) + pow((center.y - i), 2));		
				if (Radius > value)									
					Radius = value;													
			}
		}
	}
}
```



### Remain teeth only process and Draw Contours

| ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Circle_Mod.png?raw=true) | <img src="https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/dst1.png?raw=true"  /> | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/dst2.png?raw=true) |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
|                          Mod Image                           |                             dst1                             |                   dst2 = Mod Image - dst1                    |

After making the inner circle, we use Mod Image source to make dst1 using morphology to erase line between each teeth. So that we can gain an image like dst2. After this, we have to make contours to gain area, but the dst2 images' line is disconnected. 



| ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/dst2_Morph.png?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/dst2_gray.png?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Final_Contour.png?raw=true) |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
|             Final Image After Morphology process             |            Convert to Gray scale and Thresholding            |                           Contours                           |

By the morphology process in the dst2 image, we can gain an image like the first figure above, and to make this image as contours, convert image as gray scale, and binarization. Then, make contours like the third figure above.



```c++
// Morphology 
morphologyEx(drawings[i], drawing1Mod, MORPH_CLOSE, KernelELL3x3, Point(-1, -1), 1);				
morphologyEx(drawing1Mod, drawing1Mod, MORPH_CLOSE, KernelELL3x3, Point(-1, -1), 1);				
erode(drawing1Mod, dst1, KernelELL5x5, Point(-1, -1), 1);											
morphologyEx(dst1, dst1, MORPH_CLOSE, KernelELL5x5, Point(-1, -1), 4);								
dilate(dst1, dst1, Kernelrect5x5, Point(-1, -1), 2);												
morphologyEx(dst1, dst1, MORPH_CLOSE, KernelELL5x5, Point(-1, -1), 2);								

dst2 = drawing1Mod - dst1;																			

// Morphology 
erode(dst2, dst2, KernelELL3x3, Point(-1, -1), 1);													
dilate(dst2, dst2, Kernelrect5x5, Point(-1, -1), 2);												
morphologyEx(dst2, dst2, MORPH_CLOSE, Kernelrect3x3, Point(-1, -1), 1);								
erode(dst2, dst2, Kernelrect3x3, Point(-1, -1),1);		
```



### Examine defective Gear teeth

To check defective Gear teeth, we have to know the area of each contours. After, I decided to use the median method to find the Examine value.  I am going to use the sort() function then, pick the value which locate in half of array size index. 



```
for (int j = 0; j < contours2[i].size(); j++)	{						// Number of Conturs : 20 
	arr[j] = contourArea(contours2[i][j]);
}
```

To use the sort() function, we have to store the area value in vector<double> space. 



```c++
sort(arr.begin(), arr.end());							// Soring by upward.
medianVal = arr[(int)(contours2[i].size() / 2)];		// Pick the median value.
detectValMin = medianVal - tuningVal;					// Add tuning value to check defective tooth (MaxValue).
detectValMax = medianVal + tuningVal;					// Add tuning value to check defective tooth (MinValue).
```

After gaining the median Value, we have to tune the value because it is a median value. I chose a tuning Value of 200. So, if the area value is upper than detectValuMax or lower than detectValMin then, mark as defective tooth. 



### Plot Result

Text will be inserted using the putText() function, and putText will be used to obtain each context coordinate. The coordinates were manipulated and changed to output in the middle. In addition, in order to display the gear teeth in which the problem was found, the outline of the defect was inserted into the raw gear image to be displayed.



# Result and Discussion

## 1. Final Result

|                  |                         **Gear #1**                          |                         **Gear #2**                          |                         **Gear #3**                          |                         **Gear #4**                          |
| :--------------: | :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
| Output Images 1  | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Gear1_Output.png?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Gear2_Output.png?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Gear3_Output.png?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Gear4_Output.png?raw=true) |
| Output Images 2  | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Gear1_Contour_Output.png?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Gear2_Contour_Output.png?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Gear3_Contour_Output.png?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Gear4_Contour_Output.png?raw=true) |
|  Teeth Numbers   |                              20                              |                              20                              |                              20                              |                              20                              |
| Avg. Teeth Area  |                            1647.6                            |                           1734.72                            |                           2176.18                            |                             2267                             |
| Defective Teeth  |                              3                               |                              0                               |                              5                               |                              3                               |
| Diameter of Gear |                           328.132                            |                           327.207                            |                           364.957                            |                           366.203                            |
|     Quality      |                             Fail                             |                             Pass                             |                             Fail                             |                             Fail                             |
|  Output window   | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Gear1_Output_Result.png?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Gear2_Output_Result.png?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Gear3_Output_Result.png?raw=true) | ![](https://github.com/Dandelion-Kim/DLIP_2025/blob/main/Image/Lab1/Gear4_Output_Result.png?raw=true) |



## 2. Discussion

After image processing, it can be seen that the gear teeth are slightly dented. This may cause a problem in obtaining the exact area of the object, but it is judged that there is no big problem because it is now the discrimination of defective products. In addition, it took a lot of time because the best algorithm had to be selected through a lot of morphology work to obtain the final outline. However, it seems highly likely to be applied in automation because no matter what image comes in through the overall algorithm, it automatically obtains the inscribed circle, obtains the area, and determines the defect.





# Conclusion

Through image processing, the program was able to detect and display defective parts. If deep learning image processing is applied, it is expected that more accurate results can be produced.





---

# Appendix

```
- openCV Documentation (https://docs.opencv.org/4.9.0/)
- Gary R. Bradski(2017). Learning Opencv 3: Computer Vision in C++ with the Opencv Library 
- https://velog.io/@hyesoup/%EB%91%90-%EC%A0%90-%EC%82%AC%EC%9D%B4%EC%9D%98-%EA%B1%B0%EB%A6%AC-%EA%B3%B5%EC%8B%9D
- https://hwan-shell.tistory.com/119
- https://developer-cat.tistory.com/19
- https://blockdmask.tistory.com/178
- https://ansohxxn.github.io/cpp/chapter7-2/
```

```c++
double meanCal(vector<double>& arr,int size) {
	double result = 0;
	for (int i = 0; i < size; i++) {
		result += arr[i];
	}
	return result / size;
}
```

```c++
void RadiusMeasure(Mat &target, Point2f &center, float &Radius) {
	float value = 0.0;		// Space for checking distance.
	Radius = 1000.0;		// Initial Value. 

	for (int i = 0; i < target.rows; i++) {											
		for (int j = 0; j < target.cols; j++) {										
			Vec3b pixel = target.at<Vec3b>(i, j);									
			if (pixel[0] == 0 && pixel[1] == 255 && pixel[2] == 0) {				
				value = sqrt(pow((center.x - j), 2) + pow((center.y - i), 2));		
				if (Radius > value)									
					Radius = value;													
			}
		}
	}
}
```

Discription in CPP file as comment. 
