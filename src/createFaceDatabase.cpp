#include <ros/ros.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cv_bridge/cv_bridge.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <sensor_msgs/image_encodings.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;
using namespace cv;
string face_cascade_name="/opt/ros/groovy/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
int count_image=1,count_person=0;
string person_name,count_name;
//fstream text_file;
void faceDetectStore(Mat frame,string name)
{
	Mat frame_mono;
        std::vector<Rect> faces;
	cvtColor(frame,frame_mono,CV_BGR2GRAY);
	equalizeHist(frame_mono,frame_mono);
	face_cascade.detectMultiScale(frame_mono,faces,1.1,2,0|CV_HAAR_SCALE_IMAGE,Size(50,50));
	for (int i=0;i<faces.size();i++)
	{	
	string windowName="face";
	std::stringstream ss;
	ss << i;
	std::string s(ss.str());
	windowName+=s;
	cv::namedWindow(windowName);
	Point center(faces[i].x+faces[i].width*.5,faces[i].y+faces[i].height*.5);
	Point pt1(faces[i].x,faces[i].y);		
	Point pt2(faces[i].x+faces[i].width,faces[i].y+faces[i].height);
	rectangle(frame,pt1,pt2,Scalar(255,0,0),2,8,0);
	Mat face = frame_mono( faces[i] );
	imshow(windowName,face);

	if(i==0)
	   {	
		Mat image_resized(100,100, DataType<float>::type);
		resize(face,image_resized,image_resized.size(),0,0,CV_INTER_LINEAR);
		string location;
		location="/home/jarvis/ROS_jarvis/eigen_faces/"+name;	
		mkdir(location.c_str(),0755);
		std::stringstream cnt;
		cnt << count_image;
		std::string COUNT(cnt.str());
		location+="/"+name+COUNT+".jpeg";
		imwrite(location,image_resized);
		printf("out\n");
		ofstream text_file;		
		text_file.open("/home/jarvis/ROS_jarvis/eigen_faces/imageSheet.txt", std::ios_base::app);
		if(text_file.is_open())
		{
		printf("\nrunning");
		cout <<count_person<<" "<<name<<" "<<location<<endl;
		text_file <<count_person<<" "<<name<<" "<<location<<endl;
		text_file.close();
		}
		count_image++;
	   }
	}	
	imshow("output",frame);
}
void callback(const sensor_msgs::Image& input)
{
	cv_bridge::CvImagePtr input_cv;
	input_cv=cv_bridge::toCvCopy(input,sensor_msgs::image_encodings::BGR8);
	Mat image_cv;
	image_cv=input_cv->image;
	faceDetectStore(image_cv,person_name);	
	waitKey(0);
}


int initiateDatabase(int argC,char** argV)
{
	ros::init(argC,argV,"opencvFaceDatabase");
	ros::NodeHandle nh;
	ros::Subscriber sub=nh.subscribe("/kinectSplitter/image", 1,callback);
	ros::spin();
	return(0);
}

int main(int argc,char** argv)
{
	cv::namedWindow("output");
	
	printf("\nWelcome to Eigen Face Database\nEnter your name:\n");		
	cin>>person_name;
	string line;
	ifstream text_file_old ("/home/jarvis/ROS_jarvis/eigen_faces/imageSheet.txt");
	if(text_file_old.is_open())
	    {	
		printf("if works\n");
		ifstream text_file;		
		text_file.open("/home/jarvis/ROS_jarvis/eigen_faces/imageSheet.txt");
		while(text_file.good())
		{
			text_file>>count_person>> count_name>> line;
			// Ignore first character or everything up to the next newline,
			// whichever comes first
			text_file.ignore(0,'\n'); 
		}
		//stringstream convert(line);
		//convert>>count_person;
    		count_person++;
		cout<<count_person<<"\n";
		text_file.close();	
	
	    }
	
	else 
	    {	
		//fstream text_file;
		//text_file.open("/home/jarvis/ROS_jarvis/eigen_faces/imageSheet.txt",ios::out);
		count_person=1;
		//text_file<<"imageSheet"<<endl;
		//text_file.close();	
	    }
	
	text_file_old.close();
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
	initiateDatabase(argc,argv);
	//text_file.close();
	//text_file_old.close();
	return(0);
}
	
