//删除没用的代码
#include <opencv/cv.h>    
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <unistd.h>  
using std::cout;
using std::vector;
using std::string;
#include<pthread.h>
#include<string.h>
#include<fstream>
#include<math.h>
#include<dirent.h>
#include <sys/time.h>

#include "../SiftGPU/SiftGPU.h"

#define SIFTGPU_REPEAT 30

// you should specify the input image and the sift parameter to speed.exe
// for example: speed.exe -i 600.pgm  -fo -1
// to test CUDA, you first need to recompile SiftGPU with CUDA capability

//新定义的数据结构
typedef struct {
	float x;
	float y;

}Keypoints;
typedef struct{
	Keypoints kp;
	float des[128];
}exDescriptor;

typedef struct{
	int num;
	exDescriptor *exDesArray;
}desSet;

//该结构用来保存特征点信息
typedef struct{
	string originalImgId;
	string roiId;
	//string roipath;
	desSet ds;
}imageFeatureInfo;

//该结构用来保存图像的信息以IplImage的形式保存
typedef struct{
	string originalImgId;
	string roiId;
	IplImage image;
	//desSet ds;
}imgSets;
pthread_mutex_t mutex;
void* addToList(void *str);
//void addToList();
//void detectDecpector(imgSets picture);
void *thread2(void *str);
void *thread1(void *str);


//void detectDescriptor();
void* detectDescriptor(void *str);
int GetMilliSecond(); 
void getFiles( char* dirs, vector<string>& files);
std::list<imgSets> imgset;
std::list<imageFeatureInfo> imgFeature;
int main(int argc, char * argv[])
{

	pthread_t pth1,pth2;
	//	int k;
	//	addToList();
//		detectDescriptor(NULL);
 	int ret = pthread_create(&pth1, NULL,addToList, NULL);
int ret1 = pthread_create(&pth2, NULL ,detectDescriptor, NULL);
	pthread_join(pth1, NULL);
	pthread_join(pth2, NULL);
	/*
	   if (ret != 0) {
	   printf("error");
	   exit(1);
	   }
	   if (ret1 != 0) {
	   printf("error");
	   exit(1);
	   }

	   pthread_join(pth2, NULL);
	   if(pthread_join(pth1, NULL)!=0)
	   std::cout<<"pthread_join error"<<std::endl;
	   if(pthread_join(pth2, NULL)!=0)
	   std::cout<<"pthread_join error"<<std::endl;
	 */		
	//    sleep(1);
	return 0;
}

//void addToList(){
void* addToList(void *str){
	IplImage *image1=cvLoadImage("/home/wwd/SiftGPU/bin/target.jpg",1);
	IplImage* img1 = cvCreateImage(cvGetSize(image1), IPL_DEPTH_8U, 1);
	//IplImage* img1 = cvCreateImage(cvGetSize(&(image1)), IPL_DEPTH_8U, 1);
	cvCvtColor(image1,img1,CV_BGR2GRAY);
	imgSets imgset123;
	imgset123.image= *img1;//这里存入的是灰度图像
	imgset123.originalImgId="test123";
	imgset123.roiId= "test456";
	pthread_mutex_lock(&mutex);
	imgset.push_back(imgset123);
	imgset.push_back(imgset123);
	imgset.push_back(imgset123);
	imgset.push_back(imgset123);
	pthread_mutex_unlock(&mutex);
	std::cout<<"addtolist"<<std::endl;
	//return NULL;
}

//void detectDescriptor(){
void* detectDescriptor(void *str){
	SiftGPU sift;
char * argv[] = {"-cuda"};
    int argc = sizeof(argv)/sizeof(char*);
    sift.ParseParam(argc, argv);

	sift.SetVerbose(0); 

	std::cout<<"Initialize and warm up...\n";

	/*IplImage *image1=cvLoadImage("/home/wwd/SiftGPU/bin/target.jpg",1);
	//IplImage* img1 = cvCreateImage(cvGetSize(image1), IPL_DEPTH_8U, 1);
	IplImage* img1 = cvCreateImage(cvGetSize((image1)), IPL_DEPTH_8U, 1);
	cvCvtColor(image1,img1,CV_BGR2GRAY);
	imgSets imgset123;
	imgset123.image= *img1;//这里存入的是灰度图像
	imgset123.originalImgId="test123";
	imgset123.roiId= "test456";

	imgset.push_back(imgset123);
	imgset.push_back(imgset123);
	//if(sift.CreateContextGL() ==0) return 0;
*/
	std::vector<float> descriptor(1);
	std::vector<SiftGPU::SiftKeypoint> keys(1);
//	std::cout<<"before chu list2-1"<<std::endl;
	//	sleep(6);
	// imgSets picture=imgset.front();
	//imgset.pop_front();

sleep(2);
	pthread_mutex_lock(&mutex);
	imgSets picture=imgset.front();//得到list中的第一个元素并删除该元素
	imgset.pop_front();
	pthread_mutex_unlock(&mutex);
//	std::cout<<"before open fstream "<<picture.originalImgId<<std::endl;
	int num=0;
	//这里测试写入文件的代码
	//	std::fstream f2;
	//	f2.open("/home/wwd/SiftGPU/test-150330-3.txt",std::ios::out);
	//	if(!f2){
	//		std::cout<<"can't open the file"<<std::endl;
	//
	//	}
	//
	std::cout<<"before chu list2-2"<<std::endl;
	//调用sift算法
	sift.RunSIFT(&(picture.image));
	std::cout<<"before chu list2-3"<<std::endl;

		num=sift.GetFeatureNum();//得到特征点的数量

		std::cout<<"before chu list2-3"<<std::endl;
		std::cout<<"sift num---"<<num<<std::endl;
		keys.resize(num);
		descriptor.resize(128*num);
		sift.GetFeatureVector(&keys[0],&descriptor[0]);
		imageFeatureInfo imgfeatureinfo;//定义一个imageInfo的结构体临时变量，用来存

		std::cout<<"sift num--1-"<<num<<std::endl;
		imgfeatureinfo.ds.num=num;//得到特征点数目
	//这里还需要将imageInfo的图片信息给赋值。
	std::cout<<"sift num--2-"<<imgfeatureinfo.ds.num<<std::endl;
	imgfeatureinfo.originalImgId= picture.originalImgId;
	imgfeatureinfo.roiId= picture.roiId;
	//该段代码是讲得到的特征点信息保存到数据结构中，并存入list中。
	imgfeatureinfo.ds.exDesArray = new exDescriptor[num];
	int stepcount = 0;
	for (int i = 0; i < num; ++i)
	{
	imgfeatureinfo.ds.exDesArray[i].kp.x=keys[i].x;
	imgfeatureinfo.ds.exDesArray[i].kp.y=keys[i].y;
	for (int j = 0; j < 128; ++j)
	{
	imgfeatureinfo.ds.exDesArray[i].des[j]=(unsigned int)floor(0.5+512*descriptor[stepcount]);
	stepcount++;
	}
	}

	//这里测试写入文件的代码
	std::fstream f1;
	f1.open("/home/wwd/SiftGPU/test-150330-3.txt",std::ios::out);
	if(!f1){
	std::cout<<"can't open the file"<<std::endl;

	}
	int count1=0;
	f1<<num<<" "<<"128"<<"\n";
	for (int i = 0; i < num; ++i)
	{
	//imgfeatureinfo.ds.exDesArray[i].kp.x=keys[i].x;
	//imgfeatureinfo.ds.exDesArray[i].kp.y=keys[i].y;
	f1<< imgfeatureinfo.ds.exDesArray[i].kp.y<<" ";
	f1<< imgfeatureinfo.ds.exDesArray[i].kp.x<<" ";
	f1<<"\n";
	count1=0;
	for (int j = 0; j < 128; ++j)
	{
	//imgfeatureinfo.ds.exDesArray[i].des[j]=(unsigned int)floor(0.5+512*descriptor[stepcount]);
	if(count1>=20)  { f1<<"\n"; count1=0;}
	f1<<imgfeatureinfo.ds.exDesArray[i].des[j]<<" ";//(unsigned int)floor(0.5+512*descriptor[stepcount])<<" ";
	count1++;
	stepcount++;
	}f1<<"\n";
	}
	f1.close();





	imgFeature.push_back(imgfeatureinfo);
	std::cout<<"sift num--3-"<<num<<std::endl;
	for(int i=0;i<num;i++){
	std::cout<<keys[i].x<<" "<<keys[i].y<<std::endl;
	//for(int i=0;i<128;i++){
	//	std::cout<<descriptor[i]<<" ";
	}
	//*/
	//return NULL;
}




int GetMilliSecond()
{
#if defined(_WIN32)
#if defined(TIMING_BY_CLOCK)
	return clock() * 1000 / CLOCKS_PER_SEC;
#else
	static int  started = 0;
	static int	tstart;
	//the resolution of teimGetTime will be changed to 1ms inside SiftGPU
	if(started == 0)
	{
		tstart = timeGetTime();
		started = 1;
		return 0;
	}else
	{
		return timeGetTime() - tstart;
	}
#endif
#else
	static int    started = 0;
	static struct timeval tstart;
	if(started == 0) 
	{
		gettimeofday(&tstart, NULL);
		started = 1;
		return 0;
	}else
	{	
		struct timeval now;
		gettimeofday(&now, NULL) ;
		return (now.tv_usec - tstart.tv_usec + (now.tv_sec - tstart.tv_sec) * 1000000)/1000;
	}
#endif
}
//该函数从文件目录中得到该目录下的所有文件名，所有文件名均为绝对路径
void getFiles( char* dirs, vector<string>& files){

	DIR  *dir;
	struct dirent *file;
	dir = opendir(dirs);

	while ((file = readdir(dir)) != NULL)
	{
		if(strcmp(file->d_name,"..")!=0 && strcmp(file->d_name,".")!=0)
		{

			char *absolutePath = (char*)malloc(1024 *sizeof(char));
			strcpy(absolutePath,dirs);
			strcat(absolutePath,"/");
			strcat(absolutePath,file->d_name);
			std::cout<<"---------"<<absolutePath<<std::endl;
			files.push_back(absolutePath);

		}
	}
	closedir(dir);


}



void *thread1(void *str){
	int i;
	for (i = 0; i < 10; ++i)
	{
		//        sleep(2);

		pthread_mutex_lock(&mutex);
		//  s[i]=i*i;
		printf( "This in the thread1 : %d\n" , i );
		pthread_mutex_unlock(&mutex);
	}
	return NULL;

}
void *thread2(void *str){
	int i;
	for (i = 0; i < 10; ++i)
	{
		//      sleep(2);
		//     printf( "This in the thread2 : %d\n" , s[i] );
		pthread_mutex_lock(&mutex);
		printf( "This in the thread2 : %d\n" , i );
		pthread_mutex_unlock(&mutex);
	}
}

