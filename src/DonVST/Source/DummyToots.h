/**
 * @file DummyToots.h
 * @brief DummyTootFactoryクラス用のヘッダファイル
 * @author h.wakimoto
 * @date 2017/05/01
 */


#pragma once

#include "TootModel.h"
#include "Utils.h"

/***
DummyTootFactory
応援メッセージを生成するクラス
*/
class DummyTootFactory {
public:
	/**
	* @brief コンストラクタ
	* @param pTimeline ダミートゥートを流すタイムライン
	* @param pAcounts キャッシュ済みアカウントリスト
	* @param pSelfAccount リプライ先（＝VSTユーザ）アカウント
	*/
	DummyTootFactory(DonTimeLineModel* pTimeline, DonAccountListModel* pAccounts, DonAccountModel* pSelfAccount, string pLanguage) {
		this->timeline = pTimeline;
		this->accounts = pAccounts;
		this->self_account = pSelfAccount;
		this->language = pLanguage;
	}

	/**
	* @brief デストラクタ
	*/
	~DummyTootFactory() {
		//
	}

	void setLanguage(string value) {
		this->language = value;
	}

	/**
	* @brief ダミートゥートを生成
	* @return ダミートゥート
	* @detail ダミートゥートを1件生成する。生成されたトゥートはタイムラインに追加される。
	* 念のため追加されたトゥートが戻り値になっているが、基本的にはタイムラインを再描画するだけで良い。
	* @todo ダミーユーザのidをどうするか検討。idはそれほど重要な情報ではないが、キャッシュ上での
	* 重複チェックに用いられるため、既存ユーザと重複してしまうとそのユーザに置き換わってしまう。
	*/
	DonTootModel* makeDummyToot() {

		//!ダミーユーザのアカウント名
		string acct;

		//!ダミーユーザのアバターURL
		string avatar_url;

		//!ダミートゥートのポスト日時
		string created_at = getCurrentTimeJP();

		//!ダミートゥート本文
		string content;

		//!ダミーユーザの表示名
		string display_name;

		//!乱数初期化
		Random random = Random();

	//ダミーユーザを決定
		
		//!ダミーユーザファイル
		File usersfile(DUMMY_DATA_DIR + "cheerup.users."+language);

		//!ダミーユーザ一覧
		StringArray* users = new StringArray();
		usersfile.readLines(*users);

		try {
			const char* delim = ",";

			//!ランダムに抜き出した1ユーザのCSV情報
			string selected_user = users->getReference(random.nextInt(users->size() - 1)).toStdString();

			//!ユーザ情報。0がアカウント名、1が表示名
			vector<string> user = split(selected_user, *delim);
			acct = user[0];
			display_name = user[1];

		}
		catch (const exception& e) {
			showMessageDialog("エラー：DT001");
		}

		delete(users);

		if (acct!=""){

			avatar_url = "file://" + DUMMY_ICON_DIR + acct + ".png";

			//!ダミーアカウントインスタンス
			DonAccountModel* account = new DonAccountModel();

			account->setId(random.nextInt(10000)+1000000);//暫定。100万ユーザ超えのインスタンスの場合、既存ユーザと重複する可能性もあるが
														  //エラーにはならずその人のダミートゥートということになる。キャッシュ済みユーザとの
														  //重複の場合だけなので可能性は低い。
			account->setUserName(acct);
			account->setAcct("."+acct+ ".");//マストドンの仕様上ドット付きのアカウントは実在しない
			account->setDisplayName(display_name);
			account->setAvatar(avatar_url);
			account->setURL("");
			account = this->accounts->addAccount(account); //キャッシュに追加し、キャッシュ上でヒットしたらそれに置き換わる。

		//トゥートの決定

			//!ダミートゥートファイル
			File wordsfile(DUMMY_DATA_DIR + "cheerup.messages."+language);

			//!ダミートゥート一覧
			StringArray* words = new StringArray();
			wordsfile.readLines(*words);

			try {
				//ランダムに抜き出した1ダミートゥート
				content = words->getReference(random.nextInt(words->size() - 1)).toStdString();
				content = Replace(content,"@", this->self_account->getDisplayName());
				content = '@' + this->self_account->getAcct() + " " + content;

			}
			catch (const exception& e) {
				showMessageDialog("エラー：DT002");
			}
			delete(words);

			//!ダミートゥートインスタンス
			DonTootModel* toot = new DonTootModel();
			toot->setAccount(account);
			toot->setId(random.nextInt(1000));//暫定。1000トゥートに満たないインスタンスの場合、読み込みタイムライン中のIDとかぶる可能性があり
											 //その場合トゥートが追加されない。また1000トゥート以上のダミー追加はできない。（エラーにはならない）
			toot->setCreatedAt(created_at);
			toot->setVisibility("public");
			toot->setContent(content);

			toot = this->timeline->addToot(toot); //これが重要
			return toot; //こちらはおまけ
		}
		else {
			return nullptr;
		}

	}
private:

	//!ダミートゥートを流すタイムライン
	DonTimeLineModel* timeline;

	//!キャッシュ済みアカウントリスト
	DonAccountListModel* accounts;

	//!アイコンファイルの場所
	string DUMMY_ICON_DIR = (File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName() + File::separator + "VocalodonVST_files" + File::separator + "images" + File::separator + "icons" + File::separator).toStdString();

	//!ユーザ情報ファイルおよびダミートゥート情報ファイルの置き場
	string DUMMY_DATA_DIR = (File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName() + File::separator + "VocalodonVST_files" + File::separator + "data" + File::separator).toStdString();

	//!リプライ先（＝VSTユーザ）アカウント
	DonAccountModel* self_account;

	//!言語
	string language;
};


