/**
* @file Utils.h
* @brief 汎用関数用ヘッダファイル
* @author h.wakimoto
* @date 2017/05/01
*/

#pragma once

#include <string>
#include <sstream>

#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

//JUCE依存は微妙な気がする。
string time8601toJP(string time8601);

string getCurrentTime8601();

string getCurrentTimeJP();

vector<string> split(const string &s, char delim);

bool showMessageDialog(string msg);

string Replace(string srcStr, string findStr, string replaceStr);

string Float2String(float f, int digits);
