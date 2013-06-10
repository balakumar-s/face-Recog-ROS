#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <ros/ros.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sys/stat.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/contrib/contrib.hpp>

using namespace std;
using namespace cv;
string face_cascade_name="/opt/ros/groovy/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
int count_image=1,count_person=0;
string person_name,count_name;

string  faceRecog(Mat inputFace)
{
	vector<cv::Mat> images;
	vector<int> labels;
	vector<string> names;
	string line;
	ifstream fileToOpen;
	fileToOpen.open("/home/jarvis/ROS_jarvis/eigen_faces/imageSheet.txt");
	while(fileToOpen.good())
	{
		string location;
		string name;
		Mat image;
		int lbl;
		fileToOpen>> lbl>> name>> location;
		image=imread(location,0);
		images.push_back(image);
		labels.push_back(lbl);
		names.push_back(name);
	}
	fileToOpen.close();		
	images.pop_back();
	labels.pop_back();
	names.pop_back();
	int i=0;
	Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
        model->train(images, labels);
	int predictedLabel = model->predict(inputFace);
	int count=0;	
	i=0;
	string predictedPerson;
	while(i<labels.size()&&!count)
	{
		if(predictedLabel==labels[i])
		{	
			predictedPerson=names[i];
			count++;
		}
	i++;
	}
	//cout<<predictedPerson<<"\n";
	return(predictedPerson);
	
}

Mat faceGrabber(Mat frame,string name)
{
	Mat frame_mono,face;
        std::vector<Rect> faces;
	cvtColor(frame,frame_mono,CV_BGR2GRAY);
	equalizeHist(frame_mono,frame_mono);
	face_cascade.detectMultiScale(frame_mono,faces,1.1,2,0|CV_HAAR_SCALE_IMAGE,Size(50,50));
	for (int i=0;i<faces.size();i++)
	{	
	if(faces[i].height>0)
	{
	
	Point center(faces[i].x+faces[i].width*.5,faces[i].y+faces[i].height*.5);
	Point pt1(faces[i].x,faces[i].y);		
	Point pt2(faces[i].x+faces[i].width,faces[i].y+faces[i].height);
	rectangle(frame,pt1,pt2,Scalar(255,0,0),2,8,0);
	Mat face = frame_mono( faces[i] );
	Mat image_resized(100,100, DataType<float>::type);
	resize(face,image_resized,image_resized.size(),0,0,CV_INTER_LINEAR);
	string namePerson=faceRecog(image_resized);
	putText(frame,namePerson,pt1,FONT_HERSHEY_COMPLEX_SMALL,1,Scalar(0,0,255));
	}
	}
	imshow("Output",frame);	
	return(face);	
}


void callback(const sensor_msgs::Image& input)
{
	cv_bridge::CvImagePtr input_cv;
	input_cv=cv_bridge::toCvCopy(input,sensor_msgs::image_encodings::BGR8);
	Mat image_cv;
	image_cv=input_cv->image;
	Mat faceS=faceGrabber(image_cv,"test");
	waitKey(1);
}

int initiateFaceRecog(int argC,char** argV)
{
	ros::init(argC,argV,"opencvFaceDatabase");
	ros::NodeHandle nh;
	ros::Subscriber sub=nh.subscribe("/kinectSplitter/image", 1,callback);
	ros::spin();
	return(0);
}
	
int main(int argc,char** argv)
{
	namedWindow("Output");
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
	initiateFaceRecog(argc,argv);
	return 0;
                 
}
                 
