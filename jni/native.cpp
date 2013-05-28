
#include <errno.h>
#include <jni.h>
#include <sys/time.h>
#include <time.h>
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>


using namespace std;
using namespace cv;
extern "C"{
static int DIRECTION=-1;

static int V_MARGIN=10;
static int H_MARGIN=10;

void printnoLines(jlong addrRGB,int no,int i,int j){
	stringstream s;
	s <<no;

		Mat& mRGB  = *(Mat*)addrRGB;
				int screenWidth=mRGB.cols;
				int screenHeight=mRGB.rows;
				int width=screenWidth/3;
				int height=screenHeight/3;

	putText(mRGB, s.str(), Point(j*width+width/2,i*height+height/2) , FONT_HERSHEY_PLAIN, 2, Scalar(0,255,0),2,3);

}
int returnIndex(int l,int width,int MARGIN){
	int indexx;
	if((int)(l/width)>0)
		{
		if((int)(l/(width*(MARGIN-1)))>0)
			indexx=2;
		else
			indexx=1;
		}
	else
		indexx=0;
	return indexx;
}
void decreaseMargin(vector<Vec4i> lines,int width,int top){
	int screenWidth=width*((top==1)?V_MARGIN:H_MARGIN);
	int avg;
	int count=0;
	for(int i=0;i<lines.size();i++)
	{

		Vec4i v = lines[i];
		if(v[top]<width)
			{avg+=v[top];
				count++;
			}
		if(v[top+2]<width)
			{avg+=v[top];
			count++;
			}
	}
	if(count!=0)
	avg/=count;
	if(width<screenWidth/2)
    width=avg+10;
	if(top==1)V_MARGIN=(int)(screenWidth/width);
	else H_MARGIN=(int)(screenWidth/width);


}
Point2f computeIntersect(Vec4i a, Vec4i b)
{
    int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];
    int x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];

    if (float d = ((float)(x1-x2) * (y3-y4)) - ((y1-y2) * (x3-x4)))
    {
       Point2f pt;
        pt.x = ((x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4)) / d;
        pt.y = ((x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4)) / d;
        return pt;
    }
    else
        return Point2f(-1, -1);
}

JNIEXPORT jint JNICALL Java_com_example_rbf_RBFActivity_processingImage
  (JNIEnv *env, jobject thisObj, jlong addrRGB,jlong addrGray){
//*
	Mat& mRGB  = *(Mat*)addrRGB;
	Mat& mGray  = *(Mat*)addrGray;

    cvtColor(mRGB, mGray, CV_BGR2GRAY);
    GaussianBlur(mGray, mGray, Size(5, 5), 2.0, 2.0);
    //pyrMeanShiftFiltering(mRGB, mRGB, 6, 4.5);
	Canny(mGray,mGray,  66.0, 133.0, 3);
	vector<Vec4i> lines;
	HoughLinesP(mGray, lines, 1, CV_PI/180, 50, 0, 0 );

	int j=0;

	int screenWidth=mGray.cols;
		int screenHeight=mGray.rows;
		int width=screenWidth/H_MARGIN;
		int height=screenHeight/V_MARGIN;
		int temp[3][3]={{0,0,0},{0,0,0},{0,0,0}};

	if(lines.size()<=50)
		for(int i=0;i<lines.size();i++){

		Vec4i l = lines[i];
		if(l[0]>=1||(l[1]>=1 &l[1]<screenHeight-1))
		temp[returnIndex(l[0],width,H_MARGIN)][returnIndex(l[1],height,V_MARGIN)]++;
		if(l[2]>=1||(l[3]>=1 &l[3]<screenHeight-1) )
		temp[returnIndex(l[2],width,H_MARGIN)][returnIndex(l[3],height,V_MARGIN)]++;


		if((l[0]>=1||(l[1]>=1 &l[1]<screenHeight-1))||(l[2]>=1||(l[3]>=1 &l[3]<screenHeight-1) ))
			/////////////////////Debug Only////////////////////////////
		line( mRGB, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);

	}




	for(int m=0;m<3;m++)
	for(int n=0;n<3;n++)
		{

			printnoLines(addrRGB,temp[n][m],m,n);

		}
	DIRECTION=111;/*
	if( temp[0][1]>0)
		&&	temp[1][0]>0 && temp[2][1]>0 && temp[1][2]>0)
				DIRECTION=0;

	else
	if(temp[1][1]==0)
			{
		 if(temp[0][1]>0)  //L
			 DIRECTION=1;
		else if(temp[1][0]>0) //T
			DIRECTION=10;
		else if(temp[2][1]>0) //R
			DIRECTION=21;
		else if(temp[1][2]>0) //B
			DIRECTION=12;
		else if(temp[0][1]>0 && temp[2][1]>0 ) /// LR
			DIRECTION=111;
		else if(temp[1][0]>0 && temp[1][2]>0 ) ///TB :D
			DIRECTION=111;

		}
	else
	{
		if(temp[1][0]>0)
			DIRECTION=01;
		else if(temp[1][2]>0)
			DIRECTION=21;
		else DIRECTION=111;

	}/

	int leftD=0,topD=0,bottomD=0,rightD=0;

	if(temp[0][1]>0)leftD=3;if(temp[1][0]>0)topD=4;if(temp[1][2]>1)bottomD=5;if(temp[2][1]>0)rightD=6;
	int sum=leftD+topD+bottomD+rightD;
	if(sum==18){

		DIRECTION=0;
	}
	else if(sum>11)
		 DIRECTION=sum-9;
	else if(sum==9)
		{

		if(temp[1][1]<2)
		if(leftD==3)
		decreaseMargin(lines,width,0);
		else
		decreaseMargin(lines,height,1);
		DIRECTION=111;
		}
	else if(sum>6)
		{
		if(sum>9) DIRECTION=6;
		else DIRECTION=3;
		}
	else if( sum>0)
		{if(temp[1][1]>0)
		DIRECTION=-111;
	else	DIRECTION=sum;
		}

	 width=screenWidth/H_MARGIN;
	 height=screenHeight/V_MARGIN;


	line( mRGB, Point(0, height), Point(screenWidth, height), Scalar(0,255,0), 2, CV_AA);
	line( mRGB, Point(0,(V_MARGIN-1)*height), Point(screenWidth, (V_MARGIN-1)*height), Scalar(0,255,0), 2, CV_AA);
	line( mRGB, Point(width, 0), Point(width, screenHeight), Scalar(0,255,0), 2, CV_AA);
	line( mRGB, Point(width*(H_MARGIN-1),0), Point(width*(H_MARGIN-1), screenHeight), Scalar(0,255,0),2, CV_AA);
	////////////////////////////Tp////////////////////////////////////////////////////////////
	stringstream t;		t<<DIRECTION;
	putText(mRGB, "+", Point(screenWidth/3, screenHeight/3) , FONT_HERSHEY_PLAIN, 2, Scalar(0,255,0),2,3);
	putText(mRGB, "+", Point((2*screenWidth)/3,screenHeight/3) , FONT_HERSHEY_PLAIN, 2, Scalar(0,255,0),2,3);
	putText(mRGB, "+", Point(screenWidth/3,(2*screenHeight)/3) , FONT_HERSHEY_PLAIN, 2, Scalar(0,255,0),2,3);
	putText(mRGB, "+", Point((2*screenWidth)/3,(2*screenHeight)/3) , FONT_HERSHEY_PLAIN, 2, Scalar(0,255,0),2,3);
	putText(mRGB, t.str(), Point(screenWidth/2,screenHeight/2) , FONT_HERSHEY_PLAIN, 2, Scalar(255,0,0),2,3);


	vector<Point> contours_poly(counter.size());
	approxPolyDP( Mat(counter), contours_poly, 3, true );
	if(counter.size()>=4){
	Rect boundRect = boundingRect( Mat(contours_poly) );
    rectangle(mGray, boundRect.tl(), boundRect.br(), Scalar(0,0,255), 2, 8, 0 );
	}
//	for (size_t i=1;i < counter.size();i++)
	//       line(mGray, counter[i],counter[i-1],Scalar(0,0,255),1);

	/*
	  /*if(lines.size()>=4 && lines.size()<=20){


	  for (int i = 0; i < lines.size(); i++)
	  {
	      Vec4i v = lines[i];
	    //  lines[i][0] = 0;
	   //   lines[i][1] = ((float)v[1] - v[3]) / (v[0] - v[2]) * -v[0] + v[1];

	   //   lines[i][2] = mGray.cols;
	   //   lines[i][3] = ((float)v[1] - v[3]) / (v[0] - v[2]) * (mGray.cols - v[2]) + v[3];

	      Vec4i l = lines[i];
	    // line( mRGB, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
	     	 for (int j = i+1; j < lines.size(); j++)
	         {
	             Point2f pt = computeIntersect(lines[i], lines[j]);
	             if(pt.x>=0 && pt.y>=0)
	             line( mRGB, pt, pt, Scalar(255,0,0), 3, CV_AA);
	         }

	  }
	  }
	  */
	///////////////////////////////////////////////////////////////////
	/*
		Mat& mRGB  = *(Mat*)addrRGB;
		 Mat& mGray  = *(Mat*)addrGray;

		Mat imgGray;
		Mat temp;

		cvtColor( mRGB, temp, CV_BGRA2BGR,3 );
		line( mGray, Point(100,100), Point(100,100), Scalar(0,0,255), 2, CV_AA);

			///posterized image...
		pyrMeanShiftFiltering(temp, temp, 5, 50);
	    cvtColor( temp, imgGray, CV_BGR2GRAY );
	   GaussianBlur(imgGray, imgGray, Size(5, 5), 2.0, 2.0);
	    ////////////////////

	   Canny(imgGray, imgGray,66.0,133.0, 3);

    ////////////////////
	    vector<Mat> contours;
	    Mat hierarchy;

	    findContours(imgGray, contours, CV_RETR_LIST,  CV_CHAIN_APPROX_SIMPLE);

	     /// Approximate contours to polygons + get bounding rects and circles
	  vector<vector<Point> > contours_poly( contours.size() );
	  vector<Rect> boundRect( contours.size() );
	  vector<Point2f>center( contours.size() );
	  vector<float>radius( contours.size() );

	  if(contours.size()<25)
	  { for( int i = 0; i < contours.size(); i++ )
	     { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
	       boundRect[i] = boundingRect( Mat(contours_poly[i]) );
	       Scalar color = Scalar(0,0,255);
	     // drawContours( mGray, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
	      // Mat inner=mGray(boundRect[i]);
	       rectangle(mGray, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );

           floodFill(inner,   Point((boundRect[i].tl().x+boundRect[i].br().x)/2,(boundRect[i].tl().y+boundRect[i].br().y)/2), 255, (Rect*)0, Scalar(), 200);

	     }

	  }
	  stringstream t;t<<boundRect.size();
	  putText(mGray, t.str(), Point(100,100) , FONT_HERSHEY_PLAIN, 2, Scalar(0,255,0),2,3);
////////////////////////////////////////////////////////////////////////////////////
	  // */

		return DIRECTION;
}
JNIEXPORT void JNICALL Java_com_example_rbf_RBFActivity_perspectiveImage
  (JNIEnv *env, jobject thiObj, jlong temp){
}

}
