#include "aedat.hpp"
#include "dvs_gesture.hpp"

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include <vector>
#include <opencv2/opencv.hpp>

#include <fstream>

using std::string;
using std::cout;
using std::endl;

int folderHelper(const string &path);

#ifdef linux
#include <sys/stat.h>
#include <unistd.h>
int folderHelper(const string &path)
{
    if(access(path.c_str(), F_OK))
    {
        int ret = ::mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
        if(!ret)
        {
            cout << __func__ << ":\t" << "error in mkdir" << path << endl;
            return -1;
        }
    }
    return 0;
}
#elif _WIN32
#include <io.h>
int folderHelper(const string &path)
{
    if(!_access(path.c_str(), 0))
    {
        //创建文件夹
    }
}
#endif





int saveImg(const short &label_count, const uint32_t &frame_count, cv::Mat* const &pImg)
{
    string path = "/media/hdd0/guanzhou/data/DvsGesture/img/user1_natural_";
    path += std::to_string(label_count);
    path += "_";
    path += std::to_string(frame_count);
    path += ".jpg";
    // cout << __func__ << "\t" << path << std::endl;

    cv::Mat temp = pImg->clone();
    cv::imwrite(path, temp);
    
    return 0;
}

int aedat2jpg(const string &aedat_filename, const string &labels_filename, uint32_t ticksPerFrame = 5000)
{
    dvs_gesture::DataSet dataset(aedat_filename, labels_filename);

    cv::Mat* pImg = new cv::Mat(128, 128, CV_8UC3, cv::Scalar(0, 0, 0));
    // cv::Mat* pImg = new cv::Mat(128, 128, CV_8UC1, cv::Scalar::all(0));

    //循环不同labels
    for (auto data : dataset.datapoints) {
cout <<"========================================"<<endl;
        auto label_count = data.label;
        auto events = data.events;
cout << label_count << endl;

        uint32_t frame_count = 0;
        

        //循环events
        for(auto event : events)
        {
            // cout << event.x << " " << event.y << " " << event.polarity << " " << event.timestamp << endl;
            if(event.timestamp / ticksPerFrame > frame_count)
            {
                saveImg(label_count, frame_count, pImg);
                ++frame_count;
                if(pImg)
                {
                    delete pImg;
                    pImg = nullptr;
                }
                pImg = new cv::Mat(128, 128, CV_8UC3, cv::Scalar(0, 0, 0));
                // pImg = new cv::Mat(128, 128, CV_8UC1, cv::Scalar::all(0));
            }
            if(event.polarity == 0)
            {
                // pImg->at<uchar>(event.x, event.y) = 255;
                pImg->at<cv::Vec3b>(event.x, event.y)[0] = 0;
                pImg->at<cv::Vec3b>(event.x, event.y)[1] = 0;
                pImg->at<cv::Vec3b>(event.x, event.y)[2] = 255;
            }
            else
            {
                // pImg->at<uchar>(event.x, event.y) = 100;
                pImg->at<cv::Vec3b>(event.x, event.y)[0] = 255;
                pImg->at<cv::Vec3b>(event.x, event.y)[1] = 0;
                pImg->at<cv::Vec3b>(event.x, event.y)[2] = 0;
            }
        }

cout << "compeletd\t" << label_count << endl;
        saveImg(label_count, frame_count, pImg);
        if(pImg)
        {
            delete pImg;
            pImg = nullptr;
        }
        pImg = new cv::Mat(128, 128, CV_8UC3, cv::Scalar(0, 0, 0));
        // pImg = new cv::Mat(128, 128, CV_8UC1, cv::Scalar::all(0));
        
        ++label_count;

    }
    return 0;
}

int aedat2txt(const string &aedat_filename, const string &labels_filename, const string &path, std::vector<int> &labels_count)
{
    dvs_gesture::DataSet dataset(aedat_filename, labels_filename);

    //循环不同labels
    for (auto data : dataset.datapoints)
    {
        auto label = data.label;
        auto events = data.events;

        string folder = path + std::to_string(label) + "/";
        if(folderHelper(folder))
        {
            continue;
        }
        int count = labels_count[label] + 1;
        string txt_filename = folder + std::to_string(count) + ".txt";
        std::ofstream fout(txt_filename, std::ios::out);
        size_t line_count = 0;
        if(!fout.is_open())
        {
            cout << "error in open the file:\t" << txt_filename << endl;
        }
        //循环事件
        for(auto event : events)
        {
            fout << event.timestamp << " " << event.y << " " << event.x << " " << event.polarity << endl;
            ++line_count;
        }
        if(line_count)
        {
            ++labels_count[label];
        }
        fout.close();
    }

    return 0;
}

int allAedat2txt(const string &txt_path)
{
    int user = 1;
    std::vector<string> environments = {"fluorescent", "fluorescent_led", "lab", "led", "natural"};
    std::vector<int> labels_count(15, 0);

    for(;user <= 29; user++)
    {
        string path = "/media/hdd0/guanzhou/data/DvsGesture/user";
        if(user < 10)
        {
            path += "0";
        }
        path += std::to_string(user);
        path += "_";
        for(auto environment : environments)
        {
            string aedat_filename = path + environment + ".aedat";
            string labels_filename = path + environment + "_" + "labels.csv";
            cout << "======================================================" << endl;
            cout << "begin:\tuser:\t" << user << "environment:\t" << environment << endl;
            int ret = aedat2txt(aedat_filename, labels_filename, txt_path, labels_count);
            if(ret != 0)
            {
                cout << "error at user:" << user << ", at environment:" << environment << endl;
            }
            cout << "completed:\tuser:\t" << user << "environment:\t" << environment << endl;
        }
    }

    return 0;
}

int main()
{
    string path = "/media/hdd0/guanzhou/data/DvsGesture/";
    const string aedat_filename = path + "user02_fluorescent_led.aedat";
    const string labels_filename = path + "user02_fluorescent_led_labels.csv";
    // aedat2jpg(aedat_filename, labels_filename, 50000);


    allAedat2txt("/media/hdd0/guanzhou/data/DvsGesture/txt/");
}