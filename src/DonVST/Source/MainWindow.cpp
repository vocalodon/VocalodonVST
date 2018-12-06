/**
 * @file MainWindow.cpp
 * @brief DonMainWindowクラス用のソースファイル
 * @author h.wakimoto
 * @date 2017/05/01
 */

#include "PluginProcessor.h"
#include "MainWindow.h"
#include "WebAccess.h"
#include "TimeUtil.h"

#include "Utils.h"

/**
@brief APIアクセスし現在のタイムラインデータを取得(別スレッド用)
*/
void getData(DonTimeLineMode tl_mode, AccessInfo* accessinfo, DonTimeLineModel* timeline, DonAccountListModel* accounts) {

	WebAccess* wa = new WebAccess();

	//ホームタイムラインを読み込む
	if (tl_mode == tl_mode_home) {
		wa->getCurrentHomeTimeline(accessinfo, timeline, accounts);
		//ローカルタイムラインを読み込む
	}
	else if (tl_mode == tl_mode_local) {
		wa->getCurrentLocalTimeline(accessinfo, timeline, accounts);
		//スーパーLTL
	}
	else if (tl_mode == tl_mode_super_local) {
		wa->getCurrentSuperLocalTimeline(accessinfo, timeline, accounts);
		//連合タイムラインを読み込む
	}
	else if (tl_mode == tl_mode_public) {
		wa->getCurrentPublicTimeline(accessinfo, timeline, accounts);
	}
	timeline->sort();

	delete wa;
}


/**
@brief コンストラクタ
*/
DonMainWindow::DonMainWindow(DonAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p)
{

#if JUCE_MAC || JUCE_WINDOWS
	getLookAndFeel().setDefaultSansSerifTypefaceName("Arial Unicode MS");
#endif

	//日本語リテラルを入れる時のサンプル
	//String textToDisplay = CharPointer_UTF8("¥xe4¥xb8¥x80¥xe4¥xba¥x9b¥xe6¥x96¥x87¥xe5¥xad¥x97");

	/**
	@brief タイマーをスルーするかどうかを設定する。
	*/
	stop_timer = true;


	/**
	@brief ユーザ名/インスタンス名
	@detail 設定済みであれば起動時、未設定であれば設定時に読み込まれる
	*/

	addAndMakeVisible(userTitleLabel = new Label(String(), TRANS("-- Instance / User --")));
	userTitleLabel->setBounds(450, 42, 215, 24);
	userTitleLabel->setColour(Label::textColourId, Colours::grey);
	userTitleLabel->setJustificationType(Justification::centred);

	addAndMakeVisible(instanceLabel = new Label(String(), TRANS("------")));
	instanceLabel->setEditable(false, false, false);
	instanceLabel->setColour(Label::textColourId, Colours::silver);
	instanceLabel->setBounds(450, 66, 215, 24);
	instanceLabel->setJustificationType(Justification::centred);

	addAndMakeVisible(userLabel = new Label(String(), TRANS("------")));
	userLabel->setEditable(false, false, false);
    userLabel->setFont(Font(16.00f, 0)); // for MacOSX, for Windows Font("Meiryo UI", 16.00f, 0)
	userLabel->setColour(Label::textColourId, Colours::silver);
	userLabel->setBounds(450, 86, 215, 24);
	userLabel->setJustificationType(Justification::centred);
    

	/**
	@brief 現在のステータスを表示する
	@detail 今のところStopやPlayingなどを表示させているがほぼテスト用。
	*/
	//addAndMakeVisible(infoLabel = new Label(String(), TRANS("INFO")));
	//infoLabel->setJustificationType(Justification::right);
	//infoLabel->setEditable(false, false, false);
	//infoLabel->setColour(Label::textColourId, Colours::silver);


	/**
	@brief 設定ボタン
	@detail 今のところ言語設定のみ。
	*/
	addAndMakeVisible(preferenceButton = new TextButton(String()));
	preferenceButton->setButtonText(TRANS("Preference"));
	preferenceButton->setColour(TextButton::buttonColourId, juce::Colour(48, 48, 48));
	preferenceButton->setConnectedEdges(Button::ConnectedOnRight);
	preferenceButton->addListener(this);
	preferenceButton->setBounds(450, 8, 69, 24);


	/**
	@brief アカウント設定ボタン
	@detail インスタンスやアカウントを変える時に使う。
	*/
	addAndMakeVisible(accountSettingButton = new TextButton(String()));
	accountSettingButton->setButtonText(TRANS("Account"));
	accountSettingButton->setColour(TextButton::buttonColourId, juce::Colour(48, 48, 48));
	accountSettingButton->setConnectedEdges(Button::ConnectedOnRight | Button::ConnectedOnLeft);
	accountSettingButton->addListener(this);
	accountSettingButton->setBounds(524, 8, 68, 24);

	/**
	@brief バージョン情報など。GPLの表記などがあるので重要。
	@todo 本当は別のWindowsにしたいが暫定。
	*/
	addAndMakeVisible(aboutButton = new TextButton(String()));
	aboutButton->setButtonText(TRANS("About"));
	aboutButton->setColour(TextButton::buttonColourId, juce::Colour(48, 48, 48));
	aboutButton->setConnectedEdges(Button::ConnectedOnLeft);
	aboutButton->addListener(this);
	aboutButton->setBounds(597, 8, 68, 24);

	/**
	@brief テストボタン（テスト用）
	@detail 本番では使わない。新しい機能をつけたときにとりあえず
	このボタンのイベントに書いてテストする。
	*/
	addAndMakeVisible(testButton = new TextButton(String()));
	testButton->setButtonText(TRANS("Dummy Toot"));
	testButton->addListener(this);


	/**
	@brief テキストエディタ（テスト用）
	@detail 本番では使わない。新しい機能をつけてとりあえずデバッグ表示したい時などに。
	*/
	addAndMakeVisible(textEditor = new TextEditor(String()));
	textEditor->setFont(Font("Meiryo UI", 20.00f, 0));
	textEditor->setColour(TextEditor::highlightedTextColourId, Colours::red);
	textEditor->setColour(TextEditor::highlightColourId, Colours::darkgrey);
	textEditor->setColour(TextEditor::textColourId, Colours::greenyellow);
	textEditor->setColour(TextEditor::backgroundColourId, Colours::black);
	textEditor->setMultiLine(true);


	/**
	@brief タイムラインセレクタ
	*/
	addAndMakeVisible(selectTLButton_Home        = new TextButton("Home"     ));
	addAndMakeVisible(selectTLButton_Local       = new TextButton("Local"    ));
	addAndMakeVisible(selectTLButton_Super_Local = new TextButton("Super LTL"));
	addAndMakeVisible(selectTLButton_Public      = new TextButton("Public"   ));
	selectTLButton_Home       ->setClickingTogglesState(true);
	selectTLButton_Local      ->setClickingTogglesState(true);
	selectTLButton_Super_Local->setClickingTogglesState(true);
	selectTLButton_Public     ->setClickingTogglesState(true);
	selectTLButton_Home       ->setRadioGroupId(1);
	selectTLButton_Local      ->setRadioGroupId(1);
	selectTLButton_Super_Local->setRadioGroupId(1);
	selectTLButton_Public     ->setRadioGroupId(1);
	selectTLButton_Home       ->setColour(TextButton::textColourOffId, Colours::black);
	selectTLButton_Local      ->setColour(TextButton::textColourOffId, Colours::black);
	selectTLButton_Super_Local->setColour(TextButton::textColourOffId, Colours::black);
	selectTLButton_Public     ->setColour(TextButton::textColourOffId, Colours::black);
	selectTLButton_Home       ->setColour(TextButton::textColourOnId, Colours::black);
	selectTLButton_Local      ->setColour(TextButton::textColourOnId, Colours::black);
	selectTLButton_Super_Local->setColour(TextButton::textColourOnId, Colours::black);
	selectTLButton_Public     ->setColour(TextButton::textColourOnId, Colours::black);
	selectTLButton_Home       ->setColour(TextButton::buttonColourId, Colours::white);
	selectTLButton_Local      ->setColour(TextButton::buttonColourId, Colours::white);
	selectTLButton_Super_Local->setColour(TextButton::buttonColourId, Colours::white);
	selectTLButton_Public     ->setColour(TextButton::buttonColourId, Colours::white);
    selectTLButton_Home       ->setColour(TextButton::buttonOnColourId, juce::Colour(60,245,215));
	selectTLButton_Local      ->setColour(TextButton::buttonOnColourId, juce::Colour(60,245,215));
	selectTLButton_Super_Local->setColour(TextButton::buttonOnColourId, juce::Colour(60,245,215));
	selectTLButton_Public     ->setColour(TextButton::buttonOnColourId, juce::Colour(60,245,215));
	int TL_SELECT_X = 10;
	selectTLButton_Home       ->setBounds(TL_SELECT_X, 8, 100, 24);
	selectTLButton_Local      ->setBounds(TL_SELECT_X+100, 8, 100, 24);
	selectTLButton_Super_Local->setBounds(TL_SELECT_X+200, 8, 100, 24);
	selectTLButton_Public     ->setBounds(TL_SELECT_X+300, 8, 100, 24);
	selectTLButton_Home       ->setConnectedEdges(Button::ConnectedOnRight);
	selectTLButton_Local	  ->setConnectedEdges(Button::ConnectedOnLeft | Button::ConnectedOnRight);
	selectTLButton_Super_Local->setConnectedEdges(Button::ConnectedOnLeft | Button::ConnectedOnRight);
	selectTLButton_Public     ->setConnectedEdges(Button::ConnectedOnLeft);
	selectTLButton_Super_Local->setToggleState(true, dontSendNotification);
	selectTLButton_Home->addListener(this);
	selectTLButton_Local->addListener(this);
	selectTLButton_Super_Local->addListener(this);
	selectTLButton_Public->addListener(this);


	/**
	@brief アクティブボタン
	*/

	addAndMakeVisible(activeTitleLabel = new Label(String(), TRANS("-- Active status --")));
	activeTitleLabel->setEditable(false, false, false);
	activeTitleLabel->setColour(Label::textColourId, Colours::grey);

	addAndMakeVisible(activeTLButton = new TextButton("Timeline"));
	addAndMakeVisible(activeLMButton = new TextButton("Loudness Meter"));
	addAndMakeVisible(activeDTButton = new TextButton("Cheerup message"));
	addAndMakeVisible(activeSoundButton = new TextButton("Mute"));

	int ACTIVE_BTN_POS_X = 450;
	int ACTIVE_BTN_POS_Y = 120;

	activeTitleLabel->setBounds(450, ACTIVE_BTN_POS_Y, 215, 24);
	activeTitleLabel->setJustificationType(Justification::centred);

	activeTLButton->setBounds(ACTIVE_BTN_POS_X,       ACTIVE_BTN_POS_Y + 30     , 105, 24);
	activeLMButton->setBounds(ACTIVE_BTN_POS_X + 110, ACTIVE_BTN_POS_Y + 30     , 105, 24);
	activeDTButton->setBounds(ACTIVE_BTN_POS_X,       ACTIVE_BTN_POS_Y + 24 + 35, 105, 24);
	activeSoundButton->setBounds(ACTIVE_BTN_POS_X + 110,     ACTIVE_BTN_POS_Y + 24 + 35, 105, 24);


	activeTLButton->setClickingTogglesState(true);
	activeLMButton->setClickingTogglesState(true);
	activeDTButton->setClickingTogglesState(true);
	activeSoundButton    ->setClickingTogglesState(true); 

	activeTLButton->setToggleState(true, dontSendNotification);
	activeLMButton->setToggleState(true, dontSendNotification);
	activeDTButton->setToggleState(true, dontSendNotification);
	activeSoundButton    ->setToggleState(true, dontSendNotification); //! @todo スタンドアローンだったら初期状態でミュート

	activeTLButton->setColour(TextButton::textColourOffId, Colours::grey);
	activeLMButton->setColour(TextButton::textColourOffId, Colours::grey);
	activeDTButton->setColour(TextButton::textColourOffId, Colours::grey);
	activeSoundButton    ->setColour(TextButton::textColourOffId, Colours::grey);

	activeTLButton->setColour(TextButton::textColourOnId, Colours::black);
	activeLMButton->setColour(TextButton::textColourOnId, Colours::black);
	activeDTButton->setColour(TextButton::textColourOnId, Colours::black);
	activeSoundButton    ->setColour(TextButton::textColourOnId, Colours::black);
	activeSoundButton->setColour(TextButton::textColourOffId, Colours::black);

	activeTLButton->setColour(TextButton::buttonOnColourId, juce::Colour(102, 204, 255));
	activeLMButton->setColour(TextButton::buttonOnColourId, juce::Colour(102, 204, 255));
	activeDTButton->setColour(TextButton::buttonOnColourId, juce::Colour(102, 204, 255));
	activeSoundButton->setColour(TextButton::buttonOnColourId, juce::Colour(102, 204, 255));
	activeSoundButton->setColour(TextButton::buttonColourId, juce::Colour(250, 164, 128));

	activeTLButton->setConnectedEdges(Button::ConnectedOnRight);
	activeLMButton->setConnectedEdges(Button::ConnectedOnLeft);
	activeDTButton->setConnectedEdges(Button::ConnectedOnRight);
	activeSoundButton    ->setConnectedEdges(Button::ConnectedOnLeft);

	activeTLButton->addListener(this);
	activeLMButton->addListener(this);
	activeDTButton->addListener(this);
	activeSoundButton    ->addListener(this);


	/**
	@brief ラウドネスメーター
	*/
	addAndMakeVisible(lmTitleLabel = new Label(String(), TRANS("-- Loudness meter (LUFS) --")));
	lmTitleLabel->setEditable(false, false, false);
	lmTitleLabel->setColour(Label::textColourId, Colours::grey);

	addAndMakeVisible(momentaryLoudnessLabel = new Label(String(), TRANS("Momentary")));
	addAndMakeVisible(shortTermLoudnessLabel  = new Label(String(), TRANS("Short Term")));
	addAndMakeVisible(integratedLoudnessLabel = new Label(String(), TRANS("Integrated")));

	addAndMakeVisible(momentaryLoudnessValue = new Label(String(), TRANS("---")));
	momentaryLoudnessValue->setJustificationType(Justification::right);
	momentaryLoudnessValue->setFont(Font("Arial", 24.00f, Font::bold));

	addAndMakeVisible(shortTermLoudnessValue = new Label(String(), TRANS("---")));
	shortTermLoudnessValue->setJustificationType(Justification::right);
	shortTermLoudnessValue->setFont(Font("Arial", 24.00f, Font::bold));

	addAndMakeVisible(integratedLoudnessValue = new Label(String(), TRANS("---")));
	integratedLoudnessValue->setJustificationType(Justification::right);
	integratedLoudnessValue->setFont(Font("Arial", 24.00f, Font::bold));

	int LM_START_POS_X = 450;
	int LM_START_POS_Y = 220;

	lmTitleLabel->setBounds(450, LM_START_POS_Y, 215, 24);
	lmTitleLabel->setJustificationType(Justification::centred);

	momentaryLoudnessLabel->setBounds(450, LM_START_POS_Y + 30, 150, 26);
	shortTermLoudnessLabel->setBounds(450, LM_START_POS_Y + 60, 150, 26);
	integratedLoudnessLabel->setBounds(450, LM_START_POS_Y + 90, 150, 26);
	momentaryLoudnessValue->setBounds(400, LM_START_POS_Y + 30, 270, 26);
	shortTermLoudnessValue->setBounds(400, LM_START_POS_Y + 60, 270, 26);
	integratedLoudnessValue->setBounds(400, LM_START_POS_Y + 90, 270, 26);

	/**
	@brief ブラウザコンポーネント。
	@detail 今のところWebBrowserComponentを単に継承しているだけなのでWebBrowserComponentでも構わない。
	*/
	webBrowser = new DonWebBrowser();
	addAndMakeVisible(webBrowser);
	webBrowser->setVisible(false);



	//プラグインメインウィンドウのサイズ
	setSize(700, 600);

	//インストールが正常に完了しているかどうか
	if (setupCheck()) {

		//シングルトン生成
		this->accessinfo = new AccessInfo();
		this->instanceinfo = new DonInstanceModel();
		this->accounts = new DonAccountListModel();
		this->timeline  = new DonTimeLineModel();
		this->applicaitonProperties = new ApplicationProperties();

		/**
		@brief 各種設定
		*/
		prop_options = new PropertiesFile::Options();
		prop_options->applicationName = "Vocalodon VST";
		prop_options->commonToAllUsers = true;
		prop_options->filenameSuffix = "xml";
		prop_options->folderName = "VocalodonVST";
		prop_options->storageFormat = PropertiesFile::storeAsXML;
		prop_options->ignoreCaseOfKeyNames = true;
		applicaitonProperties->setStorageParameters(*prop_options);

		lastLoadedAt = "";

		//設定確認
		if (loadAccessInfo()) {
			showUserInformations();
		}
		else {
			//初期設定
			if (initialSetting()) {
				if (loadAccessInfo()) {
					showUserInformations();
				}
				else {
					showMessageDialog("Wrong Setting. Please try again. ");
				}
			}
		}

		loadSettings();

		//	if (this->accessinfo->getAccessToken() != "") {
		stop_timer = false;
		//		activeTLButton->setToggleState(true, dontSendNotification);
		startTimer(TIMER_INTERVAL);
		//	}

			//ダミーToot用タイマー
		dummyTootTimer = new DummyTootTimer(this->timeline, this->accounts, this->accessinfo->getAccount(), this, dummyTootLang);

		//DAW停止中
		setIsPlaying(false);
	}
	else {
		showMessageDialog("Install is not complete. ");
		this->setEnabled(false);
	}

	repaint();
}

/**
@brief インストールが正常完了しているか確認。
@todo 未実装。
@detail ファイルなどがすべて揃っているか確認。現状ファイルが足りないとDAWが落ちるので事前チェック。
*/
bool DonMainWindow::setupCheck() {
	return true;
}


/**
@brief ユーザ情報、インスタンス情報を表示する
@detail コンストラクタ、設定ボタンクリックから呼び出される
*/
void DonMainWindow::showUserInformations() {
	
	//ユーザ情報
	string username = this->accessinfo->getAccount()->getDisplayName(); //表示名のみ
	//string username = this->accessinfo->getAccount()->getDisplayName() + " (@" + this->accessinfo->getAccount()->getAcct() + ")";//フル
	userLabel->setText(username, NotificationType::dontSendNotification);

	//インスタンス情報
	string instancename = this->instanceinfo->getTitle(); //表示名のみ
	//string instancename = this->instanceinfo->getTitle() + " (" + this->instanceinfo->getUri() + ")"; //フル
	instanceLabel->setText(instancename, NotificationType::dontSendNotification);
}



/**
@brief デストラクタ
*/
DonMainWindow::~DonMainWindow()
{
	if (this->isEnabled()) {
		saveSettings();
		try {

			if (threadactive) {
				makeListThread.join();
				threadactive = false;
			}

			stop_timer = true;//これはこの位置最後じゃないとエラー
			delete this->timeline;
			delete this->accounts;
			delete this->instanceinfo;
			delete this->accessinfo;
		}
		catch (exception e) {
			//
		}
	}
}


/**
@brief タイマーコールバック
@detail
タイマーでタイムラインを再表示。
便宜上同じタイマーでトリガしているが、getCurrentTimeline()によるタイムラインデータ取得は別スレッドで行われており
reloadTimelineが呼ばれた時点ではまだ前行の処理は行われていない。
最終的には個別のタイマーを呼び出したほうが良いと思われるがその辺はまだ検討中。

タイマーは0.2秒ごとに発生。
・ラウドネスメーターは毎回再描画
・タイムラインリロードは1秒ごと
・APIデータ取得は3秒ごと
*/
void DonMainWindow::timerCallback() {

	File htmlfile(TEMP_DIR + "index.html");
	
	if (htmlfile.exists() && !webBrowser->isVisible()) {
		webBrowser->setVisible(true);
	}

	if (!stop_timer) {

		//TL取得
		if (activeTLButton->getToggleState() && counter_get_toots <= 0) {
			getCurrentTimeline();
			reloadTimeline();
			counter_get_toots = INTERVAL_GET_TOOTS;
		}

		if (counter_tl_reload <= 0) {
			counter_tl_reload = INTERVAL_TL_RELOAD;
		}

		//ラウドネス
		if (activeLMButton->getToggleState()) {
			updateLoudness();
		}
		else {
			resetLoudness();
		}

		counter_tl_reload -= TIMER_INTERVAL;
		counter_get_toots -= TIMER_INTERVAL;
	}
}


/**
@brief 再描画
*/
void DonMainWindow::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
	g.setColour(Colours::white);
	g.setFont(15.0f);
	g.drawImageAt(bgImage, 0, 0);
}

/**
@brief リサイズ
@detail ウインドウサイズから逆算する場合にはここでセット。
*/
void DonMainWindow::resized()
{
	//infoLabel->setBounds(400, 58, 295, 26);
	//textEditor->setBounds(450, 10, 300, 300);//デバッグ用

	//testButton->setBounds(getWidth() - 100, 108, 90, 26);//デバッグ用

	webBrowser->setBounds(10, 40, 400, 550);
	webBrowser->goToURL(LOCAL_URL + "index.html");

}


/**
@brief 設定ファイルから情報を取得。
@detail さらにAPIで追加情報（ログインユーザの情報,インスタンス情報）を取得。
*/
bool DonMainWindow::loadAccessInfo() {

	PropertiesFile* prop = applicaitonProperties->getUserSettings();
	string host_url     = prop->getValue("host_url", "").toStdString();
	string access_token = prop->getValue("access_token", "").toStdString();
	applicaitonProperties->closeFiles();
	if (access_token == "") {
		return false;
	}
	else {
		
		this->accessinfo->setInfo(host_url,access_token);
		WebAccess* wa = new WebAccess();

		//この時点ではhost_urlとaccess_tokenしか情報がないので、サーバからユーザ情報を取得する。
		wa->getAccessInfoAdditional(this->accessinfo);

		//インスタンス情報
		wa->getInstanceInfo(this->accessinfo, this->instanceinfo);

		delete wa;

		string bg_filename;
		if (host_url == "https://vocalodon.net") {
			bg_filename = "bg.png";
		}
		else {
			bg_filename = "bg.png";
		}
		File bgImageFile(IMAGE_DIR + bg_filename);
		bgImage = ImageFileFormat::loadFrom(bgImageFile);

		return true;
	}
}

/**
@brief 設定をファイルに保存。
*/
void DonMainWindow::saveAccessInfo() {

	PropertiesFile* prop = applicaitonProperties->getUserSettings();
	prop->setValue("host_url", this->accessinfo->getHostUrl().c_str());
	prop->setValue("access_token", this->accessinfo->getAccessToken().c_str());

	applicaitonProperties->setStorageParameters(*prop_options);
	applicaitonProperties->saveIfNeeded();
	applicaitonProperties->closeFiles();

}

/**
@brief 初期設定ルーチン
@detail 起動時にインスタンスやアカウントの情報がない場合や、アカウント設定を変更したい場合に呼び出される。
*/
bool DonMainWindow::initialSetting() {
	stop_timer = true;
	LoginInfo* logininfo = new LoginInfo();
	bool changed = false;

	//インスタンス選択画面を呼び出し
	if (showLoginDialog(logininfo))
	{
		WebAccess* wa = new WebAccess();
		string url = wa->getCodeURL(logininfo);
		textEditor->setText(url);
		delete wa;
		ChildProcess process;
		process.start("start " + url);
		//設定画面を呼び出し
		if (showCodeEntryDialog(logininfo, url))
		{
			wa = new WebAccess();
			string access_token = wa->getAccessToken(logininfo);
			if (access_token=="null") { //nullという文字列が入ってくる
				//設定に失敗
				showMessageDialog("Setting FAILED. Try again.");
			}
			else {
				//設定に成功
				this->accessinfo->setInfo(logininfo->getHostUrl(), access_token);
				textEditor->setText(access_token);
				saveAccessInfo();
				changed = true;
				showMessageDialog("Success!");
				reloadTimeline();
			}
			delete wa;
		}
	}
	delete logininfo;
	stop_timer = false;
	return changed;
}


/**
@brief イベントハンドラ
*/
void DonMainWindow::buttonClicked(Button* button) {
	//アカウント設定
	if (button == accountSettingButton) {
		if (initialSetting()) {
			if (loadAccessInfo()) {
				showUserInformations();
			}
			else {
				showMessageDialog("Wrong Setting. Please try again. ");
			}
		}
	}
	//設定
	else if (button == preferenceButton) {
		if (showPreferenceDialog()) {
			loadSettings();
		}
	}

	//ホームボタン
	else if (button == selectTLButton_Home) {
		clearTimeline();
		this->tl_mode = tl_mode_home;
		reloadTimeline();
	}
	//ローカルボタン
	else if (button == selectTLButton_Local) {
		clearTimeline();
		this->tl_mode = tl_mode_local;
		reloadTimeline();
	}
	//スーパーLTLボタン
	else if (button == selectTLButton_Super_Local) {
		clearTimeline();
		this->tl_mode = tl_mode_super_local;
		reloadTimeline();
	}
	//連合ボタン
	else if (button == selectTLButton_Public) {
		clearTimeline();
		this->tl_mode = tl_mode_public;
		reloadTimeline();
	}
	//TLアクティブボタン
	else if (button == activeTLButton){
		if (activeTLButton->getToggleState()) {
			getCurrentTimeline();
		}
	}
	//応援メッセージアクティブボタン
	else if (button == activeDTButton) {
		if (activeDTButton->getToggleState()) {
			dummyTootTimer->start(500);
		}
		else {
			dummyTootTimer->stop();
		}
	}
	//ラウドネスアクティブボタン
	else if (button == activeLMButton) {
		//onTimerの分岐でフラグとして利用。イベントは発生しない
	}
	//AudioOutputボタン
	else if (button == activeSoundButton) {
		this->processor.setIsMute(!activeSoundButton->getToggleState());
	}
	//Aboutボタン
	else if (button == aboutButton) {
		showAboutDialog();
	}
	//テストボタン
	else if (button == testButton) {
		DummyTootFactory* dummy = new DummyTootFactory(this->timeline,this->accounts,this->accessinfo->getAccount(),dummyTootLang);
		dummy->makeDummyToot();
	}
	
}


/**
@brief インスタンス入力ダイアログを表示
@return OKボタンが押されたか
*/
bool DonMainWindow::showLoginDialog(LoginInfo* logininfo) {
	LookAndFeel_V4 myLookandFeel;
	myLookandFeel.setColour(TextEditor::textColourId, Colours::yellow);
	myLookandFeel.setColour(AlertWindow::backgroundColourId, juce::Colour(20, 20, 20));
	myLookandFeel.setColour(TextButton::buttonColourId, juce::Colour(0, 128, 128));

	AlertWindow w("Accounts", "Enter your instance's URL.                                               ", AlertWindow::NoIcon);
	w.setLookAndFeel(&myLookandFeel);

	w.setSize(580, 300);
	w.addTextEditor("hosturl", "https://vocalodon.net", "Instance URL: (don't include last slash)");
	w.addTextBlock("");

	w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
	w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

	if (w.runModalLoop() != 0) //OK
	{
		logininfo->setInfo(
			w.getTextEditorContents("hosturl").toStdString()
		);
		return true;
	}
	return false;
}

/**
@brief コード入力ダイアログを表示
@return OKボタンが押されたか
*/
bool DonMainWindow::showCodeEntryDialog(LoginInfo* logininfo, string url) {
	LookAndFeel_V4 myLookandFeel;
	myLookandFeel.setColour(TextEditor::highlightColourId, Colours::yellow);
	myLookandFeel.setColour(TextEditor::highlightedTextColourId, Colours::black);
	myLookandFeel.setColour(AlertWindow::backgroundColourId, juce::Colour(20, 20, 20));
	myLookandFeel.setColour(TextButton::buttonColourId, juce::Colour(0, 128, 128));

	AlertWindow w("InputCode", "Copy and Paste this URL.                                                          ", AlertWindow::NoIcon);
	w.setLookAndFeel(&myLookandFeel);

	w.setColour(TextEditor::textColourId, Colours::yellow);
	w.addTextBlock("1. After open your browser and login, Please copy this URL all (triple click and right click and select `copy`) and paste to your browser's URL bar.");
	w.addTextBlock(url);
	w.addTextBlock("");
	w.addTextBlock("2. On your web browser, Click Green button and then you get Auth-Code. Please copy it and paste here.");
	w.addTextEditor("authcode", "", "Code: (paste from web browser)");
	w.addTextBlock("");
	w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
	w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

	if (w.runModalLoop() != 0) //OK
	{
		logininfo->setAuthCode(
			w.getTextEditorContents("authcode").toStdString()
		);

		return true;
	}
	return false;
}


/**
@brief バージョンダイアログを表示
@todo 別Windowでちゃんと作りたい
*/
bool DonMainWindow::showAboutDialog() {
	LookAndFeel_V4 myLookandFeel;
	myLookandFeel.setColour(AlertWindow::backgroundColourId, juce::Colour(20, 20, 20));
	myLookandFeel.setColour(TextButton::buttonColourId, juce::Colour(0, 128, 128));
	AlertWindow w("Vocalodon VST", "                                                                                                    ", AlertWindow::NoIcon);
	w.setLookAndFeel(&myLookandFeel);

	w.addTextBlock("ver 1.1.0");
	w.addTextBlock("Copyright (c) TOMOKI++ / vocalodon.net ");
	w.addTextBlock("");
	w.addTextBlock("License : GPLv3");
	w.addTextBlock("Using\n\n");
	w.addTextBlock("JUCE : https://www.juce.com/\nlibcurl : https://curl.haxx.se/libcurl/\nThe LUFS Meter : https://www.klangfreund.com/lufsmeter/\npicojson : https://github.com/kazuho/picojson");
	w.addTextBlock("Official site : https://vst.vocalodon.net/");
	w.addTextBlock("Technology supported by SGN.Inc https://sgnx.co.jp/");
	w.addTextBlock("VST is a plugin technology by Steinberg Media Technologies GmbH.\nVOCALOID and VOCALO are trademarks of Yamaha Corporation.");
	w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
	if (w.runModalLoop() != 0) //OK
	{
		return true;
	}
	return true;
}


/**
@brief 設定ダイアログ
@detail 今のところは応援メッセージの言語選択のみ
@return OKボタンが押されたか
*/
bool DonMainWindow::showPreferenceDialog() {
	LookAndFeel_V4 myLookandFeel;
	myLookandFeel.setColour(TextEditor::highlightColourId, Colours::yellow);
	myLookandFeel.setColour(TextEditor::highlightedTextColourId, Colours::black);
	myLookandFeel.setColour(AlertWindow::backgroundColourId, juce::Colour(20, 20, 20));
	myLookandFeel.setColour(TextButton::buttonColourId, juce::Colour(0, 128, 128));

	AlertWindow w("Preferences", "                                                          ", AlertWindow::NoIcon);
	w.setLookAndFeel(&myLookandFeel);

	StringArray* langArray = new StringArray();
	langArray->add("Japanese (default)");
	langArray->add("English");
	
	w.addComboBox("language", *langArray);
	w.addTextBlock("");
	w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
	w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

	if (dummyTootLang == "en") {
		w.getComboBoxComponent("language")->setSelectedItemIndex(1);
	}
	else {
		w.getComboBoxComponent("language")->setSelectedItemIndex(0);
	}

	if (w.runModalLoop() != 0) //OK
	{
		switch (w.getComboBoxComponent("language")->getSelectedItemIndex()) {
			case 0: 
				dummyTootLang = "ja";
				break;
			case 1: 
				dummyTootLang = "en";
				break;
			default:
				dummyTootLang = "ja";

		}
		saveSettings();
		return true;
	}
	return false;
}


/**
@brief 各種設定保存
@detail アカウント設定以外はすべてここで保存
*/
void DonMainWindow::saveSettings() {

	PropertiesFile* prop = applicaitonProperties->getUserSettings();
	
	//応援メッセージの言語
	prop->setValue("cheerup_lang", this->dummyTootLang.c_str());
	dummyTootTimer->setLanguage(this->dummyTootLang.c_str());

	//TLボタンの状態
	string active_timeline;

	if (selectTLButton_Home->getToggleState()) {
		active_timeline = "home";
	}
	else if (selectTLButton_Local->getToggleState()) {
		active_timeline = "local";
	}
	else if (selectTLButton_Super_Local->getToggleState()) {
		active_timeline = "super_ltl";
	}
	else if (selectTLButton_Public->getToggleState()) {
		active_timeline = "public";
	}
	prop->setValue("active_timeline", active_timeline.c_str());


	//アクティブボタンの状態
	if (activeTLButton->getToggleState()) {
		prop->setValue("active_button_timeline", "1");
	}
	else {
		prop->setValue("active_button_timeline", "0");
	}
	if (activeLMButton->getToggleState()) {
		prop->setValue("active_button_loudnessmeter", "1");
	}
	else {
		prop->setValue("active_button_loudnessmeter", "0");
	}
	if (activeDTButton->getToggleState()) {
		prop->setValue("active_button_cheerupmessage", "1");
	}
	else {
		prop->setValue("active_button_cheerupmessage", "0");
	}
	if (activeSoundButton->getToggleState()) {
		prop->setValue("active_button_sound", "1");
	}
	else {
		prop->setValue("active_button_sound", "0");
	}

	applicaitonProperties->setStorageParameters(*prop_options);
	applicaitonProperties->saveIfNeeded();
	applicaitonProperties->closeFiles();

}

/**
@brief 設定読み込み
@detail アカウント設定以外はすべてここで読み込む
*/
bool DonMainWindow::loadSettings() {

	PropertiesFile* prop = applicaitonProperties->getUserSettings();

	//応援メッセージの言語
	this->dummyTootLang = prop->getValue("cheerup_lang","ja").toStdString();

	//Timeline
	string active_timeline = prop->getValue("active_timeline", "super_ltl").toStdString();
	if (active_timeline=="home") {
		selectTLButton_Home->setToggleState(true, dontSendNotification);
	}
	else if (active_timeline == "local") {
		selectTLButton_Local->setToggleState(true, dontSendNotification);
	}
	else if (active_timeline == "super_ltl") {
		selectTLButton_Super_Local->setToggleState(true, dontSendNotification);
	}
	else if (active_timeline == "public") {
		selectTLButton_Public->setToggleState(true, dontSendNotification);
	}

	//アクティブボタン
	activeTLButton->setToggleState(prop->getValue("active_button_timeline", "1").toStdString()       == "1", dontSendNotification);
	activeLMButton->setToggleState(prop->getValue("active_button_loudnessmeter", "1").toStdString()  == "1", dontSendNotification);
	activeDTButton->setToggleState(prop->getValue("active_button_cheerupmessage", "1").toStdString() == "1", dontSendNotification);
	activeSoundButton->setToggleState(prop->getValue("active_button_sound", "1").toStdString()          == "1", dontSendNotification);

	applicaitonProperties->closeFiles();

	return true;
}


/**
@brief DAWの再生/停止のステータスが変わったときの各種処理
@todo DAWの再生/停止を確実に取得する方法が不明なため、今のところ
AudioPlayHead::CurrentPositionInfokから取得しているが、Premiereなどでは
停止の判定がされない模様。
*/
void DonMainWindow::setIsPlaying(bool value) {
	string infotxt;
	if (this->isPlaying != value) {
		this->isPlaying = value;
		if (value && activeDTButton->getToggleState()) {
			infotxt = "playing";
			dummyTootTimer->start(500);
			this->processor.ebu128LoudnessMeter.reset();
			momentaryLoudnessValue->setText("---",NotificationType::dontSendNotification);
			shortTermLoudnessValue->setText("---", NotificationType::dontSendNotification);
			integratedLoudnessValue->setText("---", NotificationType::dontSendNotification);

		} else {
			infotxt = "stopped";
			momentaryLoudnessValue->setColour(Label::textColourId, Colours::white);
			shortTermLoudnessValue->setColour(Label::textColourId, Colours::white);
			integratedLoudnessValue->setColour(Label::textColourId, Colours::white);
			dummyTootTimer->stop();
		}
	}
	//this->infoLabel->setText(infotxt,NotificationType::dontSendNotification);//デバッグ用
}

/**
@brief 再生/停止状態を返す。
@return 再生/停止状態
*/
bool DonMainWindow::getIsPlaying() {
	return this->isPlaying;
}

/**
@brief タイムラインを初期化
@todo これが呼ばれるタイミングなどは今のところよく考えてない。
*/
void DonMainWindow::clearTimeline() {
	timeline->clear();
	renderHTML();
}


/**
@brief HTMLを生成
@detail タイムラインをHTML化して保存。
@todo この辺は別クラスにしたい。
*/
void DonMainWindow::renderHTML() {

	vector<DonTootModel*> list = timeline->getList();
	vector<DonTootModel*>::reverse_iterator itr;
	File headerfile(TEMPLATE_DIR + "header.html");
	string header = headerfile.loadFileAsString().toStdString();

	File footerfile(TEMPLATE_DIR + "footer.html");
	string footer = footerfile.loadFileAsString().toStdString();

	string body;
	for (itr = list.rbegin(); itr != list.rend(); ++itr) {
		DonTootModel* toot = *itr;
		DonAccountModel* account = toot->getAccount();

		//アカウント情報
		body += "<div class=\"toot_outer\">\n";

		//通常toot
		if (!toot->isBoost()) {
			body += "<div class=\"avatar\">\n";
			body += "  <img src=\"" + account->getAvatar() + "\">\n";
			body += "</div>\n";

			body += "<div class=\"toot_inner\">\n";
			body += "  <div class=\"head\">\n";
			body += "    <div class=\"account_detail\">\n";
			body += "      <p><span class=\"account\">" + account->getDisplayName() + "</span><span class=\"acct\">(@" + account->getAcct() + ")</span></p>\n";
			body += "      <p><span class=\"timestamp\">" + toot->getCreatedAt() + "</span></p>\n";
			body += "    </div>\n";
			body += "  </div>\n";

			//コンテンツ
			body += "  <div class=\"content\">\n";
			body += toot->getContent() + "\n";
			body += "  </div>\n";

			//添付
			body += "<div class=\"media_attachments\">\n";
			vector<DonMediaAttachmentModel*> attachments = toot->getMediaAttachments();
			vector<DonMediaAttachmentModel*>::iterator itera = attachments.begin();
			while (itera != attachments.end()) {
				DonMediaAttachmentModel* attachment = *itera;
				if (attachment->getType() == "image") {
					body += "<img src=\"" + attachment->getPreviewURL() + "\">\n";
				}
				++itera;
			}
			body += "</div>\n";//media_attachment

			body += "</div>\n";//toot_inner
		}

		//ブースト
		else {
			body += "<div class=\"avatar\">\n";
			body += "  <img src=\"" + account->getAvatar() + "\" class=\"boost\"><span class=\"boost\">Bst!</span><br>\n";
			body += "  <img src=\"" + toot->getReblogAvatar() + "\">\n";
			body += "</div>\n";

			body += "<div class=\"toot_inner\">\n";
			body += "  <div class=\"head\">\n";
			body += "    <div class=\"account_detail\">\n";
			body += "      <p><span class=\"account\">" + toot->getReblogName() + "</span><span class=\"acct\">(@" + toot->getReblogAcct() + ")</span></p>\n";
			body += "      <p><span class=\"timestamp\">" + toot->getReblogCreatedAt() + "</span></p>\n";
			body += "    </div>\n";
			body += "  </div>\n";

			//コンテンツ
			body += "  <div class=\"content\">\n";
			body += toot->getReblogContent() + "\n";
			body += "  </div>\n";

			//添付
			body += "<div class=\"media_attachments\">\n";
			vector<DonMediaAttachmentModel*> attachments = toot->getReblogMediaAttachments();
			vector<DonMediaAttachmentModel*>::iterator itera = attachments.begin();
			while (itera != attachments.end()) {
				DonMediaAttachmentModel* attachment = *itera;
				if (attachment->getType() == "image") {
					body += "<img src=\"" + attachment->getPreviewURL() + "\">\n";
				}
				++itera;
			}
			body += "</div>\n";//media_attachment

			body += "</div>\n";//toot_inner
		}

		body += "</div>\n";//toot_outer
	}

	File htmlfile(TEMP_DIR + "index.html");
	htmlfile.create();
	htmlfile.replaceWithText(header, false, false);
	htmlfile.appendText(body, false, false);
	htmlfile.appendText(footer, false, false);

}

/**
@brief 現在のタイムライン情報をAPIで取得（別スレッド）
*/
void DonMainWindow::getCurrentTimeline() {

	if (threadactive) {
		makeListThread.join();
	}

	//データ取得（別スレッド）
	DonTimeLineMode      _tl_mode = this->tl_mode;
	AccessInfo*          _accessinfo = this->accessinfo;
	DonTimeLineModel*    _timeline = this->timeline;
	DonAccountListModel* _accounts = this->accounts;

	makeListThread = thread([_tl_mode, _accessinfo, _timeline, _accounts] { getData(_tl_mode, _accessinfo, _timeline, _accounts); });

	threadactive = true;

}


/**
@brief 現在のタイムライン情報を描画
@detail ここではデータ取得は行わず、キャッシュ上のタイムラインを表示する。
*/
void DonMainWindow::reloadTimeline() {
	//タイムラインの最終更新日が変わっている場合のみ再描画
	if (lastLoadedAt != timeline->getLastAt()) {
		renderHTML();
		this->webBrowser->goToURL(LOCAL_URL + "index.html");
		lastLoadedAt = timeline->getLastAt();
	}
}


/**
@brief ラウドネス情報の色設定
*/
void DonMainWindow::setLabelColor(Label* lbl, float val) {
	if (val > -8) {
		lbl->setColour(Label::textColourId, Colours::red);
	}
	else if (val > -13) {
		lbl->setColour(Label::textColourId, Colours::yellow);
	}
	else {
		lbl->setColour(Label::textColourId, Colours::white);
	}
}


/**
@breif ラウドネス情報の更新
*/
void DonMainWindow::updateLoudness() {

	string momentary;
	string shotTerm;
	string integrated;

	if (this->processor.getMomentaryLoudness() < -250) {
		momentary = "---";
	}
	else {
		momentary = Float2String(processor.getMomentaryLoudness(), 2) + " LUFS";
		setLabelColor(momentaryLoudnessValue, processor.getMomentaryLoudness());
	}

	if (this->processor.getShortTermLoudness() < -250) {
		shotTerm = "---";
	}
	else {
		shotTerm = Float2String(processor.getShortTermLoudness(),2) + " LUFS";
		setLabelColor(shortTermLoudnessValue, processor.getShortTermLoudness());
	}

	if (this->processor.getIntegratedLoudness() < -250) {
		integrated = "---";
	}
	else {
		integrated = Float2String(processor.getIntegratedLoudness(),2) + " LUFS";
		setLabelColor(integratedLoudnessValue, processor.getIntegratedLoudness());
	}

	momentaryLoudnessValue->setText(momentary.c_str(), NotificationType::dontSendNotification);
	shortTermLoudnessValue->setText(shotTerm.c_str(), NotificationType::dontSendNotification);
	integratedLoudnessValue->setText(integrated.c_str(), NotificationType::dontSendNotification);

}


/**
@breif ラウドネス情報の更新
*/
void DonMainWindow::resetLoudness() {
	momentaryLoudnessValue->setText("---", NotificationType::dontSendNotification);
	shortTermLoudnessValue->setText("---", NotificationType::dontSendNotification);
}
