// ConsoleJsonRW.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include <fstream>
//#include <iostream>
#include <string>
#include <vector>

//jsoncons header
#include "jsoncons/json.hpp"

//name space
using namespace std;
using std::string;
using jsoncons::json;
using jsoncons::output_format;
using jsoncons::pretty_print;

//JSONreader(): 示範讀取 voxdata32-earth.json
//JSONwriter(): 示範寫入檔案
void JSONreader();
void JSONwriter();

int _tmain(int argc, _TCHAR* argv[])
{
	JSONreader();
	//JSONwriter();

	system("PAUSE");
	return 0;
}

//Input:要讀取的檔案路徑(file path): string fpth;
//Output: 顯示檔案的各個屬性，如果有錯誤會提示並停止讀取
void JSONreader(){
	/*
		##try to load .json
		1.set the file path to ( string fpth ), notice that use "/" instead of "\"
		2.create json object (json obj) to accept the parse_file()
		3.if file not exist, parse_file() function will throw error
	*/
	string fpth;
	//這裡設定你的讀檔路徑，也可以使用 std::cin 做使用者讀取
	fpth = "input/voxdata32-earth.json";

	json obj;

	std::cout << "開始讀取檔案" << fpth << std::endl;
	try{
		obj = json::parse_file(fpth);
	}catch(const std::exception& e){
		/*if open fail*/
		std::cerr << e.what() << std::endl;
	}

	/*
		##Start load flie
		1.access the model's information from json object: name / modelscale / voxelscale / number of voxels
		2.printout those information
		3.for each voxel, detect if voxel's data legal.If not, break out the for loop
	*/
	string name = obj["Modelname"].as<std::string>();
	int mscale = ( obj["Modelscale"].as<std::vector<int>>() )[0];
	double vscale = ( obj["Voxelscale"].as<std::vector<double>>() )[0];
	int vnum = obj["Numofvoxel"].as<int>();

	std::cout<< "Model's information:" <<std::endl;
	std::cout<< "------------------------------" << std::endl;
	std::cout<< "Name:"<< name <<std::endl;
	std::cout<< "Resolution:" << mscale <<std::endl;
	std::cout<< "Voxel size:" << vscale <<std::endl;
	std::cout<< "Total" << vnum << " of voxels\n" <<std::endl;
	std::cout<< "------------------------------" << std::endl;

	/*for each voxel...*/
	for(int i = 0; i < vnum; i++){
		/*use voxel to get each object in obj["Model"]*/
		json& voxel = obj["Model"][i];

		int color[3] = {0};//color
		int opacity = 0;
		color[0] = ( voxel["RGBAcolor"].as<std::vector<int>>() )[0];
		color[1] = ( voxel["RGBAcolor"].as<std::vector<int>>() )[1];
		color[2] = ( voxel["RGBAcolor"].as<std::vector<int>>() )[2];
		opacity = ( voxel["RGBAcolor"].as<std::vector<int>>() )[3];

		int pos[3] = {0};
		pos[0] = ( voxel["Position"].as<std::vector<int>>() )[0];
		pos[1] = ( voxel["Position"].as<std::vector<int>>() )[1];
		pos[2] = ( voxel["Position"].as<std::vector<int>>() )[2];

		int tag = voxel["Tag"].as<int>();

		/*print out if voxel data wrong*/
		if ( (tag == -1) && ( (color[0] != 256) || (color[1] != 256) || (color[2] != 256) || (opacity != 256) ) ){
			std::cout<< "the " << i << " th voxel wrong message" << std::endl;
			std::cout<< "Position:" << pos[0] << "," << pos[1] << ","<< pos[2] << std::endl;
			std::cout<< "Tag:" << tag << std::endl;//tag
			std::cout<< "Color: ";
			std::cout<< "R:"<< color[0] << " , ";//R
			std::cout<< "G:"<< color[1] << " , ";//G
			std::cout<< "B:"<< color[2] << " , ";//B
			std::cout<< "Opacity:"<< opacity << std::endl;//A
			std::cout<< "------------------------------" << std::endl;

			break;
		}
	}

	std::cout<< "讀檔完成..." << std::endl;
};

//Input: 要寫入的json物件: json obj，以及檔案路徑: string fpth
//Output: 寫入檔案到指定路徑
void JSONwriter(){
	/*
		##set up each  attribute
		1.model's name: string name
		2.model's resolution: json Modelscale
		3.voxle's scale: json Voxelscale
		4.number of voxel: int vnum
		5.each voxel's data: json Model
	*/
	string name = "cubic_voxel";

	int mscale = 3;
	json Modelscale(json::an_array);
	Modelscale.add(mscale);
	Modelscale.add(mscale);
	Modelscale.add(mscale);

	double vscale = 4.92;
	json Voxelscale(json::an_array);
	Voxelscale.add(vscale);
	Voxelscale.add(vscale);
	Voxelscale.add(vscale);

	int vnum = 27;

	//set up voxel [1,1,1] to inner voxel
	json Model(json::an_array);
	for(int x = 0; x < mscale; x++){
		for(int y = 0; y < mscale; y++){
			for(int z = 0; z < mscale; z++){
				json voxel;
				json rgbacolor(json::an_array);
				json position(json::an_array);
				int tag = 0;

				if( (x == 1) && (y == 1) && ( z == 1) ){
					tag = -1;//which means inner voxel
					rgbacolor.add(256);//R
					rgbacolor.add(256);//G
					rgbacolor.add(256);//B
					rgbacolor.add(256);//A
				}else{
					tag = 0;//bordor voxel
					rgbacolor.add(22);//R
					rgbacolor.add(34);//G
					rgbacolor.add(146);//B
					rgbacolor.add(1);//A
				}

				position.add(x);
				position.add(y);
				position.add(z);

				voxel["RGBAcolor"] = rgbacolor;
				voxel["Position"] = position;
				voxel["Tag"] = tag;
				
				Model.add(voxel);
			}
		}
	}

	/*
		## create json object (json obj), then push attribute above in obj.
	*/
	json obj;
	obj["Modelname"] = name;
	obj["Modelscale"] = Modelscale;
	obj["Voxelscale"] = Voxelscale;
	obj["Numofvoxel"] = vnum;
	obj["Model"] = Model;

	/*
		## write file
	*/
	 fstream file;
	 string fpth = "output/cubic_voxel.json";
	 file.open(fpth , ios::out | ios::trunc);

	 std::cout << "open file:" << fpth << std::endl;
	 if( file.fail() ){
		 std::cout << "open fail..."<< std::endl;
	 }else{
		 std::cout << "write file..." << endl;
		 file << pretty_print(obj);
		 std::cout << "done." << endl;
	 }
	 file.close();   
};