/**
 * @file DonWebBrowser.h
 * @brief DonWebBrowserクラス用のヘッダファイル
 * @author h.wakimoto
 * @date 2017/05/01
 */


#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

/***
DonWebBrowser
WebBrowserでスクロールが発生したときにリロードを止める処理を書くために
試行錯誤したが今のところ成果なし。今のところWebBrowserComponentを直接使っても同じ
*/
class DonWebBrowser : public WebBrowserComponent {
public:

	/**
	* @brief イベントハンドラ
	* @todo 今のところ取れない（要調査）
	*/
	void mouseEnter(const MouseEvent &event) {
		if (mouseMoveEvent != nullptr) {
			mouseMoveEvent(false);
		}
	}

	/**
	* @brief イベントハンドラ
	* @todo 今のところ取れない（要調査）
	*/
	void mouseDown(const MouseEvent &event) { //取れない
		if (mouseMoveEvent != nullptr) {
			mouseMoveEvent(false);
		}
	}

	/**
	* @brief イベントハンドラ
	* @todo タイミングが不自然で利用しづらい
	*/
	void mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel) { 
		if (mouseMoveEvent != nullptr) {
			mouseMoveEvent(false);
		}
	}

	/**
	* @brief なんか試行錯誤してた途中のゴミ
	*/
	std::function<void(bool)> mouseMoveEvent = nullptr;
};