#include <iostream>
#include <opencv2/opencv.hpp>

void colorToMono(cv::Mat &image)
{
    for(int i=0; i<image.cols; i++)
        for(int j=0; j<image.rows; j++)
        {
            cv::Vec3b color=image.at<cv::Vec3b>(cv::Point(i,j));
            
            color[0]=color[1]=color[2]=int(color[0]+color[1]+color[2]/3/(255-1))*(255-1);
            
            image.at<cv::Vec3b>(cv::Point(i,j))=color;
        }
}

int neighbourCount(cv::Point p, cv::Mat image)
{
    int answer=0;
    
    if(p.x>0)
        if(image.at<cv::Vec3b>(p)==image.at<cv::Vec3b>(cv::Point(p.x-1, p.y))) answer++;
    
    if(p.x<image.cols-1)
        if(image.at<cv::Vec3b>(p)==image.at<cv::Vec3b>(cv::Point(p.x+1, p.y))) answer++;
    
    if(p.y>0)
        if(image.at<cv::Vec3b>(p)==image.at<cv::Vec3b>(cv::Point(p.x, p.y-1))) answer++;
    
    if(p.y<image.rows-1)
        if(image.at<cv::Vec3b>(p)==image.at<cv::Vec3b>(cv::Point(p.x, p.y+1))) answer++;
    
    if(p.x>0 && p.y>0)
        if(image.at<cv::Vec3b>(p)==image.at<cv::Vec3b>(cv::Point(p.x-1, p.y-1))) answer++;
    
    if(p.x>0 && p.y<image.rows-1)
        if(image.at<cv::Vec3b>(p)==image.at<cv::Vec3b>(cv::Point(p.x-1, p.y+1))) answer++;
    
    if(p.x<image.cols-1 && p.y>0)
        if(image.at<cv::Vec3b>(p)==image.at<cv::Vec3b>(cv::Point(p.x+1, p.y-1))) answer++;
    
    if(p.x<image.cols-1 && p.y<image.rows-1)
        if(image.at<cv::Vec3b>(p)==image.at<cv::Vec3b>(cv::Point(p.x+1, p.y+1))) answer++;
    
    return answer;
}

void transform(cv::Mat &image)
{
    for(int i=0; i<image.cols; i++)
        for(int j=0; j<image.rows; j++)
        {
            int count=neighbourCount(cv::Point(i,j), image);
            
            if(count>=2) image.at<cv::Vec3b>(cv::Point(i,j))=cv::Vec3b(255,255,255);
            else image.at<cv::Vec3b>(cv::Point(i,j))=cv::Vec3b(0,0,0);
        }
}

void clearImage(cv::Mat &image, int n)
{
    for(int k=0; k<n; k++)
        for(int i=0; i<image.cols; i++)
            for(int j=0; j<image.rows; j++)
                if(image.at<cv::Vec3b>(cv::Point(i,j))==cv::Vec3b(0,0,0))
                {
                    int count=neighbourCount(cv::Point(i,j), image);
                    
                    if(count<=3) image.at<cv::Vec3b>(cv::Point(i,j))=cv::Vec3b(255,255,255);
                }
}

void sketchImage(cv::Mat &image)
{
    for(int i=0; i<image.cols; i++)
        for(int j=0; j<image.rows; j++)
            if(image.at<cv::Vec3b>(cv::Point(i,j))==cv::Vec3b(0,0,0))
            {
                int count=neighbourCount(cv::Point(i,j), image);
                
                if(count<=1) image.at<cv::Vec3b>(cv::Point(i,j))=cv::Vec3b(150,150,150);
                else
                    if(count==2) image.at<cv::Vec3b>(cv::Point(i,j))=cv::Vec3b(100,100,100);
                else
                    if(count==3) image.at<cv::Vec3b>(cv::Point(i,j))=cv::Vec3b(50,50,50);
                else
                    if(count>=4) image.at<cv::Vec3b>(cv::Point(i,j))=cv::Vec3b(0,0,0);
            }
}

int main(int argc, char ** argv)
{
    cv::VideoCapture video(argv[1]);
    cv::VideoWriter move;
    bool created=false;
    
    int cadr_count=int(video.get(cv::CAP_PROP_FRAME_COUNT));
    
    for(int i=0; i<cadr_count; i++)
    {
        cv::Mat image;
        video>>image;
        
        colorToMono(image);
        transform(image);
        clearImage(image, 1);
        sketchImage(image);
        
        if(!created)
        {
            move.open(argv[2], video.get(CV_CAP_PROP_FOURCC), video.get(CV_CAP_PROP_FPS), image.size());
            created=true;
        }
        
        move<<image;
        
        std::cout<<i+1<<"/"<<cadr_count<<"\n";
    }
    
    return 0;
}
