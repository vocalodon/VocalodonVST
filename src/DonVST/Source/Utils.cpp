/**
* @file Utils.cpp
* @brief 汎用関数など
* @author h.wakimoto
* @date 2017/05/01
*/

#include "Utils.h"
#include <iomanip>

string time8601toJP(string time8601) {
	StringRef sr = StringRef(time8601.c_str());
	Time tm = Time::fromISO8601(sr);
	return tm.formatted("%Y/%m/%d %H:%M:%S").toStdString();
}

string getCurrentTime8601() {
	Time tm = Time::getCurrentTime();
	return tm.toISO8601(false).toStdString();
}

string getCurrentTimeJP() {
	Time tm = Time::getCurrentTime();
	return tm.formatted("%Y/%m/%d %H:%M:%S").toStdString();
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		if (!item.empty()) {
			elems.push_back(item);
		}
	}
	return elems;
}

bool showMessageDialog(string msg) {
	AlertWindow w("Message", "", AlertWindow::QuestionIcon);

	w.addTextBlock(msg);
	w.setSize(800, w.getHeight());
	w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));

	if (w.runModalLoop() != 0) //OK
	{
		return true;
	}
	return false;
}

string Replace(string srcStr, string findStr, string replaceStr)
{
	string::size_type  Pos(srcStr.find(findStr));

	while (Pos != string::npos)
	{
		srcStr.replace(Pos, findStr.length(), replaceStr);
		Pos = srcStr.find(findStr, Pos + replaceStr.length());
	}

	return srcStr;
}

string Float2String(float f, int digits)
{

	ostringstream oss;

	oss << setprecision(digits) << setiosflags(ios::fixed) << f;

	return oss.str();

}