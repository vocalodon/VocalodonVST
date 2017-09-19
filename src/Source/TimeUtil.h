/**
* @file TimeUtil.h
* @brief 日付関係の関数（Utilsに書いてもいいかもしれない）
* @author h.wakimoto
* @date 2017/05/01
*/#pragma once

#include <time.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

string current_time_str()
{
	time_t     current;
	struct tm  *local;

	time(&current);                     /* 現在の時刻を取得 */
	local = localtime(&current);        /* 地方時の構造体に変換 */

	ostringstream sout;
	sout << std::setfill('0') << setw(4) << local->tm_year + 1900;
	sout << std::setfill('0') << setw(2) << local->tm_mon + 1;
	sout << std::setfill('0') << setw(2) << local->tm_mday;
	sout << std::setfill('0') << setw(2) << local->tm_hour;
	sout << std::setfill('0') << setw(2) << local->tm_min;
	sout << std::setfill('0') << setw(2) << local->tm_sec;

	return sout.str();
}

