#ifndef RESOURCE_UTILS_H
#define RESOURCE_UTILS_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "opengl_utils.h"
using namespace std;
VAO* loadSplineControlPoints(string path)
{
    //TODO: load spline control point data and return VAO
    //You can make use of getVAOFromAttribData in opengl_utils.h
    string line;
    ifstream  myfile(path);;
    int cnt = 0;
    int cnt_block = 0;
    int size  = 1;
    vector<float> attrib_data;
    vector<unsigned int> attrib_sizes;
    string space_delimiter = " ";

    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            cnt += 1;

            if (cnt == 1) {
                size = stoi(line);
                //cout << size << endl;
                attrib_data.assign(size * 3  , 0.0);
            }

            else {

                size_t pos = 0;
                vector<string> x{};
                cnt_block = 0;

                while ((pos = line.find(space_delimiter)) != string::npos) {
                    x.push_back(line.substr(0, pos));
                    line.erase(0, pos + space_delimiter.length());
                    cnt_block += 1;
                }
                x.push_back(line.substr(0, pos));
                cnt_block += 1;

                //for (auto i : x)
                    //std::cout << i << ' ';
                //cout << "" << endl;
                for (int i = 0; i < cnt_block; i++) {
                    //cout << i + cnt * 3 - 6 << endl;
                    attrib_data[i + cnt * 3 - 6] = stof(x[i]);
                }
                
            }

        }
        myfile.close();
        attrib_sizes = { 3 };
        VAO* ret = RESOURCE_UTILS_H::getVAOFromAttribData(attrib_data, attrib_sizes);
        
        //cout << (ret->ID) << endl;
        //cout << "test1" << endl;
        return ret;
        
    }
    

}

VAO* loadBezierSurfaceControlPoints(string path)
{
    //(Optional)TODO: load surface control point data and return VAO.
    //You can make use of getVAOFromAttribData in opengl_utils.h


    string line;
    ifstream  myfile(path);;
    int cnt = 0;
    int kl = 0;
    int cnt_block = 0;
    int size = 1;
    vector<float> attrib_data;
    vector<unsigned int> attrib_sizes;
    string space_delimiter = " ";

    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            cnt += 1;
            kl += 1;

            if (cnt == 1) {
                size = stoi(line);
                //cout << size << endl;
                attrib_data.assign(size * 3 * 16, 0.0);
            }

            else {
                //cout << line << endl;
                if (kl % 17 != 2) {
                    size_t pos = 0;
                    vector<string> x{};
                    cnt_block = 0;
                    cnt -= 1;
                    float data[3];
                    stringstream ss(line);
                    for (int i = 0; i < 3; i++) {
                        ss >> data[i];
                    }
                    x.push_back(line.substr(0, pos));
                    cnt_block += 1;


                    //cout << cnt << endl;
                   // if (cnt_block == 3) {
                        cnt += 1;
                        for (int i = 0; i < 3; i++) {
                            attrib_data[i + cnt * 3 - 6] = data[i];
                        }
                    //}
                }
                else {
                    cnt -= 1;
                }


            }

        }
        myfile.close();
        attrib_sizes = { 3 };
        //for (auto i : attrib_data)
               //std::cout << i << ' ';
       //cout << "" << endl;
        VAO* ret = RESOURCE_UTILS_H::getVAOFromAttribData(attrib_data, attrib_sizes);

        //cout << (ret->ID) << endl;
        //cout << "test1" << endl;
        return ret;

    }

}
#endif