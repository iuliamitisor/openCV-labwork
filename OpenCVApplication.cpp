// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>

wchar_t* projectPath;

void testOpenImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("image",src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName)==0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName,"bmp");
	while(fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(),src);
		if (waitKey()==27) //ESC pressed
			break;
	}
}

void testImageOpenAndSave()
{
	_wchdir(projectPath);

	Mat src, dst;

	src = imread("Images/Lena_24bits.bmp", IMREAD_COLOR);	// Read the image

	if (!src.data)	// Check for invalid input
	{
		printf("Could not open or find the image\n");
		return;
	}

	// Get the image resolution
	Size src_size = Size(src.cols, src.rows);

	// Display window
	const char* WIN_SRC = "Src"; //window for the source image
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Dst"; //window for the destination (processed) image
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, src_size.width + 10, 0);

	cvtColor(src, dst, COLOR_BGR2GRAY); //converts the source image to a grayscale one

	imwrite("Images/Lena_24bits_gray.bmp", dst); //writes the destination to file

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	waitKey(0);
}

void testNegativeImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		double t = (double)getTickCount(); // Get the current time [s]
		
		Mat src = imread(fname,IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = Mat(height,width,CV_8UC1);
		// Accessing individual pixels in an 8 bits/pixel image
		// Inefficient way -> slow
		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				uchar val = src.at<uchar>(i,j);
				uchar neg = 255 - val;
				dst.at<uchar>(i,j) = neg;
			}
		}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image",src);
		imshow("negative image",dst);
		waitKey();
	}
}

void testNegativeImageFast()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = src.clone();

		double t = (double)getTickCount(); // Get the current time [s]

		// The fastest approach of accessing the pixels -> using pointers
		uchar *lpSrc = src.data;
		uchar *lpDst = dst.data;
		int w = (int) src.step; // no dword alignment is done !!!
		for (int i = 0; i<height; i++)
			for (int j = 0; j < width; j++) {
				uchar val = lpSrc[i*w + j];
				lpDst[i*w + j] = 255 - val;
			}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image",src);
		imshow("negative image",dst);
		waitKey();
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src = imread(fname);

		int height = src.rows;
		int width = src.cols;

		Mat dst = Mat(height,width,CV_8UC1);

		// Accessing individual pixels in a RGB 24 bits/pixel image
		// Inefficient way -> slow
		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				Vec3b v3 = src.at<Vec3b>(i,j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst.at<uchar>(i,j) = (r+g+b)/3;
			}
		}
		
		imshow("input image",src);
		imshow("gray image",dst);
		waitKey();
	}
}

void testBGR2HSV()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		int height = src.rows;
		int width = src.cols;

		// HSV components
		Mat H = Mat(height, width, CV_8UC1);
		Mat S = Mat(height, width, CV_8UC1);
		Mat V = Mat(height, width, CV_8UC1);

		// Defining pointers to each matrix (8 bits/pixels) of the individual components H, S, V 
		uchar* lpH = H.data;
		uchar* lpS = S.data;
		uchar* lpV = V.data;

		Mat hsvImg;
		cvtColor(src, hsvImg, COLOR_BGR2HSV);

		// Defining the pointer to the HSV image matrix (24 bits/pixel)
		uchar* hsvDataPtr = hsvImg.data;

		for (int i = 0; i<height; i++)
		{
			for (int j = 0; j<width; j++)
			{
				int hi = i*width * 3 + j * 3;
				int gi = i*width + j;

				lpH[gi] = hsvDataPtr[hi] * 510 / 360;	// lpH = 0 .. 255
				lpS[gi] = hsvDataPtr[hi + 1];			// lpS = 0 .. 255
				lpV[gi] = hsvDataPtr[hi + 2];			// lpV = 0 .. 255
			}
		}

		imshow("input image", src);
		imshow("H", H);
		imshow("S", S);
		imshow("V", V);

		waitKey();
	}
}

void testResize()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat dst1,dst2;
		//without interpolation
		resizeImg(src,dst1,320,false);
		//with interpolation
		resizeImg(src,dst2,320,true);
		imshow("input image",src);
		imshow("resized image (without interpolation)",dst1);
		imshow("resized image (with interpolation)",dst2);
		waitKey();
	}
}

void testCanny()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src,dst,gauss;
		src = imread(fname,IMREAD_GRAYSCALE);
		double k = 0.4;
		int pH = 50;
		int pL = (int) k*pH;
		GaussianBlur(src, gauss, Size(5, 5), 0.8, 0.8);
		Canny(gauss,dst,pL,pH,3);
		imshow("input image",src);
		imshow("canny",dst);
		waitKey();
	}
}

void testVideoSequence()
{
	_wchdir(projectPath);

	VideoCapture cap("Videos/rubic.avi"); // off-line video from file
	//VideoCapture cap(0);	// live video from web cam
	if (!cap.isOpened()) {
		printf("Cannot open video capture device.\n");
		waitKey(0);
		return;
	}
		
	Mat edges;
	Mat frame;
	char c;

	while (cap.read(frame))
	{
		Mat grayFrame;
		cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
		Canny(grayFrame,edges,40,100,3);
		imshow("source", frame);
		imshow("gray", grayFrame);
		imshow("edges", edges);
		c = waitKey(100);  // waits 100ms and advances to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished\n"); 
			break;  //ESC pressed
		};
	}
}


void testSnap()
{
	_wchdir(projectPath);

	VideoCapture cap(0); // open the deafult camera (i.e. the built in web cam)
	if (!cap.isOpened()) // openenig the video device failed
	{
		printf("Cannot open video capture device.\n");
		return;
	}

	Mat frame;
	char numberStr[256];
	char fileName[256];
	
	// video resolution
	Size capS = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),
		(int)cap.get(CAP_PROP_FRAME_HEIGHT));

	// Display window
	const char* WIN_SRC = "Src"; //window for the source frame
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Snapped"; //window for showing the snapped frame
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, capS.width + 10, 0);

	char c;
	int frameNum = -1;
	int frameCount = 0;

	for (;;)
	{
		cap >> frame; // get a new frame from camera
		if (frame.empty())
		{
			printf("End of the video file\n");
			break;
		}

		++frameNum;
		
		imshow(WIN_SRC, frame);

		c = waitKey(10);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished");
			break;  //ESC pressed
		}
		if (c == 115){ //'s' pressed - snap the image to a file
			frameCount++;
			fileName[0] = NULL;
			sprintf(numberStr, "%d", frameCount);
			strcat(fileName, "Images/A");
			strcat(fileName, numberStr);
			strcat(fileName, ".bmp");
			bool bSuccess = imwrite(fileName, frame);
			if (!bSuccess) 
			{
				printf("Error writing the snapped image\n");
			}
			else
				imshow(WIN_DST, frame);
		}
	}

}

void MyCallBackFunc(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == EVENT_LBUTTONDOWN)
		{
			printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
				x, y,
				(int)(*src).at<Vec3b>(y, x)[2],
				(int)(*src).at<Vec3b>(y, x)[1],
				(int)(*src).at<Vec3b>(y, x)[0]);
		}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}

/* Histogram display function - display a histogram using bars (simlilar to L3 / Image Processing)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/
void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i<hist_cols; i++)
	if (hist[i] > max_hist)
		max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}

void changeGrayLevels() {
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = src.clone();

		double t = (double)getTickCount(); // Get the current time [s]

		// The fastest approach of accessing the pixels -> using pointers
		uchar* lpSrc = src.data;
		uchar* lpDst = dst.data;
		int w = (int)src.step; // no dword alignment is done !!!
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				uchar val = lpSrc[i * w + j];
				int sum = val + 50;
				if (sum > 255)
					sum = 255;
				lpDst[i * w + j] = (uchar)sum;
			}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("negative image", dst);
		waitKey();
	}
}

void fourColoredQuarters() {
	Mat_<Vec3b> img(200, 400);
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (i < img.rows / 2 && j < img.cols / 2) {
				img(i, j) = Vec3b(0, 0, 255);
			}
			else if (i > img.rows / 2 && j < img.cols / 2) {
				img(i, j) = Vec3b(255, 255, 255);
			}
			else if (i < img.rows / 2 && j > img.cols / 2) {
				img(i, j) = Vec3b(0, 255, 255);
			}
			else if (i > img.rows / 2 && j > img.cols / 2) {
				img(i, j) = Vec3b(150, 150, 150);
			}
		}
	}

	imshow("Four colored quarters", img);
	waitKey();
}

void inverseMatrix() {
	float vals[16] = { 1111, 21, 31, 14, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	Mat M(4, 4, CV_32FC1, vals);
	Mat M_inv;
	M_inv = M.inv();
	std::cout << M_inv;
	getchar();
	getchar();
}

void RGBSplitChannels() {
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_COLOR);

		int height = src.rows;
		int width = src.cols;

		Mat RChannel = Mat(height, width, CV_8UC3);
		Mat GChannel = Mat(height, width, CV_8UC3);
		Mat BChannel = Mat(height, width, CV_8UC3);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3b v3 = src.at<Vec3b>(i, j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				RChannel.at<Vec3b>(i, j) = Vec3b(0, 0, r);
				GChannel.at<Vec3b>(i, j) = Vec3b(0, g, 0);
				BChannel.at<Vec3b>(i, j) = Vec3b(b, 0, 0);
			}
		}

		imshow("input image", src);
		imshow("RChannel", RChannel);
		imshow("GChannel", GChannel);
		imshow("BChannel", BChannel);
		waitKey();
	}
}

void RGBtoGrayscale() {
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);

		int height = src.rows;
		int width = src.cols;

		Mat dst = Mat(height, width, CV_8UC1);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3b v3 = src.at<Vec3b>(i, j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst.at<uchar>(i, j) = (0.5*r + 2*g + 0.5*b) / 3;
			}
		}

		imshow("input image", src);
		imshow("gray image", dst);
		waitKey();
	}
}

void grayscaletToBlackAndWhite() {
	char fname[MAX_PATH];
	int threshold = 128;
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);

		int height = src.rows;
		int width = src.cols;

		Mat dst = Mat(height, width, CV_8UC1);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (src.at<uchar>(i, j) > threshold) {
					dst.at<uchar>(i, j) = 255;
				}
				else {
					dst.at<uchar>(i, j) = 0;
				}
			}
		}

		imshow("input image", src);
		imshow("black and white image", dst);
		waitKey();
	}
}

void RGBtoHSV() {
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_COLOR);

		int height = src.rows;
		int width = src.cols;

		Mat Hnorm = Mat(height, width, CV_8UC1);
		Mat Snorm = Mat(height, width, CV_8UC1);
		Mat Vnorm = Mat(height, width, CV_8UC1);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3b v3 = src.at<Vec3b>(i, j);
				uchar b = v3[0] / 255.0f;
				uchar g = v3[1] / 255.0f;
				uchar r = v3[2] / 255.0f;

				Vnorm.at<uchar>(i, j) = max_(r, max_(g, b));

				uchar M = Vnorm.at<uchar>(i, j);
				uchar m = min_(r, min_(g, b));

				if (Vnorm.at<uchar>(i, j) == 0) {
					Snorm.at<uchar>(i, j) = 0;
				}
				else {
					Snorm.at<uchar>(i, j) = (M - m) / Vnorm.at<uchar>(i, j);
				}

				if ((M - m) != 0) {
					if (M == r) {
						Hnorm.at<uchar>(i, j) = 60.0f * (g - b) / (M - m);
					}
					if (M == g) {
						Hnorm.at<uchar>(i, j) = 120.0f + 60.0f * (b - r) / (M - m);
					}
					if (M == b) {
						Hnorm.at<uchar>(i, j) = 240.0f + 60.0f * (r - g) / (M - m);
					}
				}
				else {
					Hnorm.at<uchar>(i, j) = 0;
				} 
				if (Hnorm.at<uchar>(i, j) < 0) {
					Hnorm.at<uchar>(i, j) += 360;
				}

			}
		}

		imshow("input image", src);
		imshow("Hnorm", Hnorm);
		imshow("Snorm", Snorm);
		imshow("Vnorm", Vnorm);
		waitKey();
	}
}

void showHistogramGscale() {
	char fname[MAX_PATH];
	int h[256] = { 0 };
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);

		int height = src.rows;
		int width = src.cols;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				h[src.at<uchar>(i, j)]++;
			}
		}

		imshow("input image", src);
		showHistogram("Histogram", h, 256, 200);
	}
}

void multilevelThresholding() {
	char fname[MAX_PATH];
	int WH = 5;
	float TH = 0.0003f;

	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		Mat dst = src.clone();
		int height = src.rows;
		int width = src.cols;
		int M = width * height;

		int h[256] = { 0 };
		float p[256] = { 0.0f };
		int localMax[256] = { 0 };
		localMax[0] = localMax[255] = 1;

		// build histogram counts
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				h[src.at<uchar>(i, j)]++;

		// normalize histogram to get probabilities
		for (int i = 0; i < 256; i++)
			p[i] = (float)h[i] / (float)M;

		for (int k = WH; k <= 255 - WH; k++) {
			// use float accumulator for probabilities
			float localSum = 0.0f;
			for (int i = k - WH; i <= k + WH; i++) {
				localSum += p[i];
			}
			float windowAvg = localSum / (2 * WH + 1);

			if (p[k] > p[k - WH] && p[k] > p[k + WH] &&
				p[k] > p[k - 1] && p[k] > p[k + 1] &&
				p[k] > windowAvg + TH) {
				localMax[k] = 1;
			}
			else {
				localMax[k] = 0;
			}
		}

		for (int i = 0; i < height; i++){
			for (int j = 0; j < width; j++) {
				uchar val = src.at<uchar>(i, j);
				// find closest local maximum to the pixel value
				int closestLocalMax = 0;
				for (int k = 0; k < 256; k++) {
					if (localMax[k] == 1) {
						if (abs(k - val) < abs(closestLocalMax - val)) {
							closestLocalMax = k;
						}
					}
				}
				dst.at<uchar>(i, j) = closestLocalMax;
			}
		}
		imshow("input image", src);
		imshow("multilevel thresholded image", dst);
	}
}

void floydSteinbergDithering() {

	char fname[MAX_PATH];
	int WH = 5;
	float TH = 0.0003f;

	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		Mat dst = src.clone();
		int height = src.rows;
		int width = src.cols;
		int M = width * height;
		int epsilon = 0;

		int h[256] = { 0 };
		float p[256] = { 0.0f };
		int localMax[256] = { 0 };
		localMax[0] = localMax[255] = 1;

		// build histogram counts
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				h[src.at<uchar>(i, j)]++;

		// normalize histogram to get probabilities
		for (int i = 0; i < 256; i++)
			p[i] = (float)h[i] / (float)M;

		for (int k = WH; k <= 255 - WH; k++) {
			// use float accumulator for probabilities
			float localSum = 0.0f;
			for (int i = k - WH; i <= k + WH; i++) {
				localSum += p[i];
			}
			float windowAvg = localSum / (2 * WH + 1);

			if (p[k] > p[k - WH] && p[k] > p[k + WH] &&
				p[k] > p[k - 1] && p[k] > p[k + 1] &&
				p[k] > windowAvg + TH) {
				localMax[k] = 1;
			}
			else {
				localMax[k] = 0;
			}
		}

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				uchar val = dst.at<uchar>(i, j);
				// find closest local maximum to the pixel value
				int closestLocalMax = 0;
				for (int k = 0; k < 256; k++) {
					if (localMax[k] == 1) {
						if (abs(k - val) < abs(closestLocalMax - val)) {
							closestLocalMax = k;
						}
					}
				}
				dst.at<uchar>(i, j) = closestLocalMax;
				epsilon = val - closestLocalMax;
				if (j + 1 < width)
					dst.at<uchar>(i, j + 1) = min(max(0, dst.at<uchar>(i, j + 1) + epsilon * 7 / 16), 255);
				if (i + 1 < height && j - 1 >= 0)
					dst.at<uchar>(i + 1, j - 1) = min(max(0, dst.at<uchar>(i + 1, j - 1) + epsilon * 3 / 16), 255);
				if (i + 1 < height)
					dst.at<uchar>(i + 1, j) = min(max(0, dst.at<uchar>(i + 1, j) + epsilon * 5 / 16), 255);
				if (i + 1 < height && j + 1 < width)
					dst.at<uchar>(i + 1, j + 1) = min(max(0, dst.at<uchar>(i + 1, j + 1) + epsilon / 16), 255);
			}
		}

		imshow("input image", src);
		imshow("dithered image", dst);
	}
}

void labelingBFS() {
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		Mat dst(src.rows, src.cols, CV_8UC3, Scalar(255, 255, 255));
		Mat labels(src.rows, src.cols, CV_32SC1, Scalar(0));

		std::queue<Point> q;
		int di[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
		int dj[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
		int label = 1;
		srand(time(NULL));

		for (int i = 0; i < src.rows; i++) {
			for (int j = 0; j < src.cols; j++) {

				if (src.at<uchar>(i, j) != 0)
					continue;

				if (labels.at<int>(i, j) != 0)
					continue;

				q = std::queue<Point>();
				q.push(Point(j, i));
				labels.at<int>(i, j) = label;
				Vec3b color(rand() % 256, rand() % 256, rand() % 256);

				while (!q.empty()) {
					Point p = q.front();
					q.pop();
					dst.at<Vec3b>(p.y, p.x) = color;

					for (int k = 0; k < 8; k++) {
						int ni = p.y + di[k];
						int nj = p.x + dj[k];
						if (ni < 0 || ni >= src.rows || nj < 0 || nj >= src.cols)
							continue;

						if (src.at<uchar>(ni, nj) != 0)
							continue;
						if (labels.at<int>(ni, nj) != 0)
							continue;

						labels.at<int>(ni, nj) = label;
						q.push(Point(nj, ni));
					}
				}

				label++;
			}
		}


		imshow("input image", src);
		imshow("BFS labeling", dst);
	}
}

void labelingTwoPass() {

	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		if (src.empty())
			continue;

		int rows = src.rows;
		int cols = src.cols;

		Mat dst(rows, cols, CV_8UC3, Scalar(255, 255, 255));
		Mat labels(rows, cols, CV_32SC1, Scalar(0));

		std::vector<std::vector<int>> edges;
		edges.resize(1);

		int label = 0;
		int di[4] = { 0, -1, -1, -1 };
		int dj[4] = { -1, 0, -1, 1 };
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				if (src.at<uchar>(i, j) != 0)
					continue;

				if (labels.at<int>(i, j) != 0)
					continue;
				std::vector<int> L;
				for (int k = 0; k < 4; k++) {
					int ni = i + di[k];
					int nj = j + dj[k];
					if (ni < 0 || ni >= rows || nj < 0 || nj >= cols)
						continue;
					int nl = labels.at<int>(ni, nj);
					if (nl > 0) {
						if (std::find(L.begin(), L.end(), nl) == L.end())
							L.push_back(nl);
					}
				}

				if (L.empty()) {
					label++;
					labels.at<int>(i, j) = label;
					if ((int)edges.size() <= label)
						edges.resize(label + 1);
				}
				else {
					int x = *std::min_element(L.begin(), L.end());
					labels.at<int>(i, j) = x;
					
					for (int y : L) {
						if (y == x) continue;
						
						int maxIdx;
						if (x > y) {
							maxIdx = x;
						}
						else maxIdx = y;

						if ((int)edges.size() <= maxIdx)
							edges.resize(maxIdx + 1);
						if (std::find(edges[x].begin(), edges[x].end(), y) == edges[x].end())
							edges[x].push_back(y);
						if (std::find(edges[y].begin(), edges[y].end(), x) == edges[y].end())
							edges[y].push_back(x);
					}
				}
			}
		}

		if (label == 0) {
			imshow("input image", src);
			imshow("two-pass labeling", dst);
			continue;
		}

		std::vector<int> newlabels(label + 1, 0);
		int newlabel = 0;
		std::queue<int> Q;

		for (int i = 1; i <= label; i++) {
			if (newlabels[i] != 0)
				continue;
			newlabel++;
			newlabels[i] = newlabel;
			while (!Q.empty()) Q.pop();
			Q.push(i);
			while (!Q.empty()) {
				int x = Q.front(); Q.pop();
				if (x < (int)edges.size()) {
					for (int y : edges[x]) {
						if (newlabels[y] == 0) {
							newlabels[y] = newlabel;
							Q.push(y);
						}
					}
				}
			}
		}
		RNG rng(12345);
		std::vector<Vec3b> colors(newlabel + 1);
		for (int i = 1; i <= newlabel; i++) {
			colors[i] = Vec3b((uchar)rng.uniform(0, 256), (uchar)rng.uniform(0, 256), (uchar)rng.uniform(0, 256));
		}
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				if (src.at<uchar>(i, j) == 0) {
					int old = labels.at<int>(i, j);
					int nl = 0;
					if (old > 0 && old < (int)newlabels.size())
						nl = newlabels[old];
					if (nl > 0)
						dst.at<Vec3b>(i, j) = colors[nl];
				}
			}
		}

		imshow("input image", src);
		imshow("two-pass labeling", dst);
		waitKey();
	}
}

int main() 
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
    projectPath = _wgetcwd(0, 0);

	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Open image\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Image negative\n");
		printf(" 4 - Image negative (fast)\n");
		printf(" 5 - BGR->Gray\n");
		printf(" 6 - BGR->Gray (fast, save result to disk) \n");
		printf(" 7 - BGR->HSV\n");
		printf(" 8 - Resize image\n");
		printf(" 9 - Canny edge detection\n");
		printf(" 10 - Edges in a video sequence\n");
		printf(" 11 - Snap frame from live video\n");
		printf(" 12 - Mouse callback demo\n");
		printf(" 13 - Change gray levels (L01)\n");
		printf(" 14 - Four colored quarters (L01)\n");
		printf(" 15 - Inverse matrix (L01)\n");
		printf(" 16 - RGB24 - split channels (L02)\n");
		printf(" 17 - RGB24 to grayscale (L02)\n");
		printf(" 18 - Grayscale to black and white (L02)\n");
		printf(" 19 - RGB24 to HSV (L02)\n");
		printf(" 20 - Histogram (grayscale image) (L03)\n");
		printf(" 21 - Multilevel thresholding (L03)\n");
		printf(" 22 - Floyd-Steinberg dithering (L03)\n");
		printf(" 30 - Labeling: BFS (L05)\n");
		printf(" 31 - Labeling: Two-pass traversal (L05)\n");

		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d",&op);
		switch (op)
		{
			case 1:
				testOpenImage();
				break;
			case 2:
				testOpenImagesFld();
				break;
			case 3:
				testNegativeImage();
				break;
			case 4:
				testNegativeImageFast();
				break;
			case 5:
				testColor2Gray();
				break;
			case 6:
				testImageOpenAndSave();
				break;
			case 7:
				testBGR2HSV();
				break;
			case 8:
				testResize();
				break;
			case 9:
				testCanny();
				break;
			case 10:
				testVideoSequence();
				break;
			case 11:
				testSnap();
				break;
			case 12:
				testMouseClick();
				break;
			case 13:
				changeGrayLevels();
				break;
			case 14:
				fourColoredQuarters();
				break;
			case 15:
				inverseMatrix();
				break;
			case 16:
				RGBSplitChannels();
				break;
			case 17:
				RGBtoGrayscale();
				break;
			case 18:
				grayscaletToBlackAndWhite();
				break;
			case 19:
				RGBtoHSV();
				break;
			case 20:
				showHistogramGscale();
				break;
			case 21:
				multilevelThresholding();
				break;
			case 22:
				floydSteinbergDithering();
				break;
			case 30:
				labelingBFS();
				break;
			case 31:
				labelingTwoPass();
				break;
		}

	}
	while (op!=0);
	return 0;
}