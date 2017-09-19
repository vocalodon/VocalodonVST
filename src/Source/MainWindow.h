/**
 * @file MainWindow.h
 * @brief DonMainWindowクラス用のヘッダファイル。
 * @author h.wakimoto
 * @date 2017/05/01
 */

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ApiAccount.h"
#include "TootModel.h"
#include "DummyToots.h"
#include "VersionDialog.h"
#include "DonWebBrowser.h"

#include <string>
#include <thread>


using namespace std;

class DummyTootTimer;

/***
DonMainWindowクラス（メインウインドウ）
・VSTから呼び出されるカスタムエディタ本体。
・最終的にはメインウインドウはシンプルに設定用ボタンなどを置き、タイムラインは
　別ウインドウにする予定。だが、デバッグ簡易化のために今のところメインウインドウで
 　すべての処理を行っている。
・JUCE依存
*/
class DonMainWindow : public AudioProcessorEditor, public ButtonListener,private Timer
{
public:
	DonMainWindow(DonAudioProcessor&);
	~DonMainWindow();

	void paint(Graphics&) override;
	void resized() override;

	void timerCallback() override;
	bool initialSetting();
	bool loadAccessInfo();
	void saveAccessInfo();
	void showUserInformations();
	void setIsPlaying(bool value);
	bool getIsPlaying();
	void reloadTimeline();

private:
	DonAudioProcessor& processor;
	DonAudioProcessorEditor* mainEditor;
	
	bool setupCheck();//!@todo 未実装

	bool showLoginDialog(LoginInfo* logininfo);
	bool showCodeEntryDialog(LoginInfo* logininfo,string url);
	bool showAboutDialog();

	bool showPreferenceDialog();
	bool loadSettings();
	void saveSettings();

	void clearTimeline();
	void renderHTML();

	bool isPlaying = false;

	void getCurrentTimeline();
	bool threadactive = false;

//	ScopedPointer<Label> titleLabel;
	ScopedPointer<Label> infoLabel;


	ScopedPointer<Label> activeTitleLabel;
	ScopedPointer<TextButton> activeTLButton;
	ScopedPointer<TextButton> activeLMButton;
	ScopedPointer<TextButton> activeDTButton;
	ScopedPointer<TextButton> activeSoundButton;

	ScopedPointer<TextButton> accountSettingButton;
	ScopedPointer<TextButton> preferenceButton;
	ScopedPointer<TextButton> aboutButton;

	ScopedPointer<Label> userTitleLabel;
	ScopedPointer<Label> instanceLabel;
	ScopedPointer<Label> userLabel;

	ScopedPointer<TextButton> selectTLButton_Home;
	ScopedPointer<TextButton> selectTLButton_Local;
	ScopedPointer<TextButton> selectTLButton_Super_Local;
	ScopedPointer<TextButton> selectTLButton_Public;

	ScopedPointer<DonWebBrowser> webBrowser;

	ScopedPointer<TextEditor> textEditor;
	ScopedPointer<TextButton> testButton;
	
	ScopedPointer<Label> lmTitleLabel;
	ScopedPointer<Label> momentaryLoudnessLabel;
	ScopedPointer<Label> shortTermLoudnessLabel;
	ScopedPointer<Label> integratedLoudnessLabel;
	ScopedPointer<Label> momentaryLoudnessValue;
	ScopedPointer<Label> shortTermLoudnessValue;
	ScopedPointer<Label> integratedLoudnessValue;
	void updateLoudness();
	void resetLoudness();
	void setLabelColor(Label* lbl, float val);

	SafePointer<VersionDialog> versionDialog;

	Image bgImage;


	//ScopedPointer<ListBox> listBox;

	void buttonClicked(Button* button) override;

	AccessInfo* accessinfo;
	DonInstanceModel* instanceinfo;

	DonAccountListModel* accounts;
	DonTimeLineModel* timeline;
	DonTimeLineMode tl_mode = tl_mode_super_local;

	ScopedPointer<DummyTootTimer> dummyTootTimer;

	bool stop_timer = false;
	thread makeListThread;

	ScopedPointer<ApplicationProperties> applicaitonProperties;
	ScopedPointer<PropertiesFile::Options> prop_options;


	//ここはApplicaitonPropertiesが利用する場所と結果的には同じなので、うまく統合できないか？
	string TEMPLATE_DIR  = (File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName() + File::separator + "VocalodonVST_files" + File::separator + "html" + File::separator).toStdString();
	string IMAGE_DIR     = (File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName() + File::separator + "VocalodonVST_files" + File::separator + "images" + File::separator).toStdString();
	string USER_FILE_DIR = (File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getFullPathName() + File::separator + "VocalodonVST"+ File::separator).toStdString();
	string TEMP_DIR         = juce::String(USER_FILE_DIR + "temp" + File::separator).toStdString();
	string CONFIG_FILE_PATH = juce::String(USER_FILE_DIR + "temp" + File::separator + "config").toStdString();
	string LOCAL_URL        = "file://"+TEMP_DIR;

	string lastLoadedAt;

	int TIMER_INTERVAL = 200;
	int INTERVAL_TL_RELOAD = 1000;
	int INTERVAL_GET_TOOTS = 3000;

	int counter_tl_reload = INTERVAL_TL_RELOAD;
	int counter_get_toots = 500;//初回は早く読む

	string dummyTootLang = "ja";

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DonMainWindow)
};


class DummyTootTimer : Timer {
public:
	DummyTootTimer(DonTimeLineModel* pTimeline, DonAccountListModel* pAccounts, DonAccountModel* pSelfAccount, DonMainWindow* pWindow, string pLanguage) {
		this->timeline = pTimeline;
		this->accounts = pAccounts;
		this->self_account = pSelfAccount;
		this->window = pWindow;
		this->language = pLanguage;
	}
	~DummyTootTimer() {
		//
	}
	void timerCallback() override {
		try {
			Random random = Random();
			if (random.nextBool()) {
				DummyTootFactory* dummy = new DummyTootFactory(this->timeline, this->accounts, this->self_account, language);
				dummy->makeDummyToot();
				//window->reloadTimeline();//ホントはここで再表示したいがエラーになる。
			}
		}
		catch (const exception& e) {
			showMessageDialog("エラー：MW001");
		}
	}
	void setLanguage(string value) {
		this->language = value;
	}
	void start(int intervalInMilliseconds) {
		startTimer(intervalInMilliseconds);
	}
	void stop() {
		stopTimer();
	}

	//std::function<void(DonTimeLineModel*, DonAccountListModel*, string, )> onTimer = nullptr;
private:
	DonTimeLineModel* timeline;
	DonAccountListModel* accounts;
	DonAccountModel* self_account ;
	DonMainWindow* window;
	string language;
};

