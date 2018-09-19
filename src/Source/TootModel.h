/**
 * @file TootModel.h
 * @brief マストドンのタイムラインに関する各種モデルクラス用のヘッダファイル
 * @author h.wakimoto
 * @date 2017/05/01
 */


#pragma once

#include <string>
#include <vector>
#include <picojson.h>
#include <functional>
#include <algorithm>

#include "Utils.h"

/*
Toot関係のモデル

最終的にはファイル分割か？
今のところヘッダにすべて記述している。

・DonAccountModelクラス
・DonTootModelクラス
・DonTimeLineModelクラス
・DonAccountListModelクラス

*/


using namespace std;


typedef enum {
	tl_mode_home,
	tl_mode_local,
	tl_mode_super_local,
	tl_mode_public
}DonTimeLineMode;


/****
DonInstanceModel クラス
・インスタンス情報のエンティティ
*/
//暫定。最終的には全部のフィールドを実装
class DonInstanceModel {
public:
	DonInstanceModel() {

	}
	~DonInstanceModel() {
		//
	}

	string getUri() {
		return uri;
	}
	string getTitle() {
		return title;
	}
	string getDescription(){
		return description;
	}
	string getEmail() {
		return email;
	}
	string getVersion() {
		return version;
	}

	void setUri(string value) {
		uri = value;
	}
	void setTitle(string value) {
		title = value;
	}
	void setDescription(string value) {
		description = value;
	}
	void setEmail(string value) {
		email = value;
	}
	void setVersion(string value) {
		version = value;
	}

private:
	string uri;
	string title;
	string description;
	string email;
	string version;
};

/****
DonAccountModelクラス

・ユーザ情報のエンティティ
・このインスタンスのポインタはDonAccountListModel内のリストに追加される。
・上記リストに追加されないもの（重複など）は即時deleteされる。メモリ管理はリストで行う。

*/
//暫定。最終的には全部のフィールドを実装
class DonAccountModel {
public:
	DonAccountModel() {
		//
	}
	DonAccountModel(picojson::object obj) {
		id       = stoi(obj["id"].to_str());
		username = obj["username"].to_str();
		acct     = obj["acct"].to_str();
		display_name = obj["display_name"].to_str();
		avatar   = obj["avatar"].to_str();
		url      = obj["url"].to_str();
	}
	~DonAccountModel() {
		//
	}
	unsigned __int64 getId() {
		return id;
	}
	string getUserName() {
		return username;
	}
	string getAcct() {
		return acct;
	}
	string getDisplayName() {
		return display_name;
	}
	string getAvatar() {
		return avatar;
	}
	string getURL() {
		return url;
	}
	void setId(unsigned __int64 value) {
		id = value;
	}
	void setUserName(string value) {
		username = value;
	}
	void setAcct(string value) {
		acct = value;
	}
	void setDisplayName(string value) {
		display_name = value;
	}
	void setAvatar(string value) {
		avatar = value;
	}
	void setURL(string value) {
		url = value;
	}
private:
	unsigned __int64 id;
	string username;
	string acct;
	string display_name;//
	string avatar;//
	string url;//
};

/***
DonMediaAttachmentModelクラス

・Tootの添付ファイル。
・Toot1に対して多で存在しうる。
・リストはTootが保持。
*/
class DonMediaAttachmentModel {
public:

	DonMediaAttachmentModel(unsigned __int64 pId, string pType, string pPreviewUrl, string pSize) {
		this->id = pId;
		this->type = pType;
		this->preview_url = pPreviewUrl;
		this->size = pSize;
	}
	~DonMediaAttachmentModel() {
		//
	}

	unsigned __int64 getId() {
		return id;
	}
	string getType() {
		return type;
	}
	string getPreviewURL() {
		return preview_url;
	}
	string getSize() {
		return size;
	}
	void setId(unsigned __int64 value) {
		id = value;
	}
	void setType(string value) {
		type = value;
	}
	void setPreviewURL(string value) {
		preview_url = value;
	}
	void setSize(string value) {
		size = value;
	}
private:
	unsigned __int64 id;
	string type;
	string preview_url;
	string size;
};


/****
DonTootModelクラス

・トゥート情報のエンティティ
・このインスタンスのポインタはDonTimeLineModel内のリストに追加される。
・上記リストに追加されないもの（重複など）は即時deleteされる。
・アカウント情報を内包しているが、これはDonAccountListModelに入っているものと
　同じものを参照している。メモリ管理はリスト側で行う。
・DonMediaAttachmentのインスタンスのリストを保持する。メモリ管理はここで行う。
・ブーストは本来はDonTootModelのネスト構造だが2階層かつ1件に限定されるため、同一階層上に展開している。
*/
//暫定。最終的には全部のフィールドを実装
class DonTootModel {
public:
	DonTootModel() {
		//
	}
	DonTootModel(picojson::object obj, DonAccountModel* acnt) {
		//メインToot
		id = stoull(obj["id"].to_str());
		created_at = time8601toJP(obj["created_at"].to_str());
		visibility = obj["visibility"].to_str();
		account = acnt;
		content = obj["content"].to_str();
		//ブースト
		if (obj["reblog"].to_str() != "null") {
			is_boost = true;
			picojson::object boost_obj = obj["reblog"].get<picojson::object>();
			picojson::object boost_acnt_obj = boost_obj["account"].get<picojson::object>();
			reblog_acct = boost_acnt_obj["acct"].to_str();
			reblog_name = boost_acnt_obj["display_name"].to_str();
			reblog_avatar = boost_acnt_obj["avatar"].to_str();
			reblog_created_at = time8601toJP(boost_obj["created_at"].to_str());
			reblog_content = boost_obj["content"].to_str();

			picojson::array boost_medias = boost_obj["media_attachments"].get<picojson::array>();
			picojson::array::iterator itr = boost_medias.begin();
			while (itr != boost_medias.end()) {
				picojson::object boost_media = itr->get<picojson::object>();
				unsigned __int64 m_id = stoi(boost_media["id"].to_str());
				string m_type = boost_media["type"].to_str();
				string m_url = boost_media["preview_url"].to_str();
				string m_size = boost_media["size"].to_str();
				DonMediaAttachmentModel* attach = new DonMediaAttachmentModel(m_id, m_type, m_url, m_size);
				reblog_media_attachments.push_back(attach);
				++itr;
			}

		}
	}
	~DonTootModel() {
		vector<DonMediaAttachmentModel*>::iterator itr = media_attachments.begin();
		while (itr != media_attachments.end()) {
			delete *itr;
			++itr;
		}
		vector<DonMediaAttachmentModel*>::iterator itr2 = reblog_media_attachments.begin();
		while (itr2 != reblog_media_attachments.end()) {
			delete *itr2;
			++itr2;
		}
	}
	unsigned __int64 getId() {
		return id;
	}
	string getCreatedAt() {
		return created_at;
	}
	string getVisibility() {
		return visibility;
	}
	DonAccountModel* getAccount() { //リスト上のアカウントへのポインタ
		return account;
	}
	string getContent() {
		return content;
	}
	vector<DonMediaAttachmentModel*> getMediaAttachments() {
		return media_attachments;
	}

	bool isBoost() {
		return is_boost;
	}
	string getReblogContent() {
		return reblog_content;
	}
	string getReblogAcct() {
		return reblog_acct;
	}
	string getReblogName() {
		return reblog_name;
	}
	string getReblogAvatar() {
		return reblog_avatar;
	}
	string getReblogCreatedAt() {
		return reblog_created_at;
	}
	vector<DonMediaAttachmentModel*> getReblogMediaAttachments() {
		return reblog_media_attachments;
	}


	void setId(unsigned __int64 value) {
		id = value;
	}

	void setCreatedAt(string value) {
		created_at = value;
	}
	void setVisibility(string value) {
		visibility = value;
	}
	void setAccount(DonAccountModel* value) {
		account = value;
	}
	void setContent(string value) {
		content = value;
	}
	void addMediaAttachments(unsigned __int64 pId,string pType,string pPreviewUrl,string pSize) {
		DonMediaAttachmentModel* attach = new DonMediaAttachmentModel(pId, pType, pPreviewUrl, pSize);
		media_attachments.push_back(attach);
	}
	

private:
	unsigned __int64 id;
	string created_at;//最終的には日付型
	string visibility;
	DonAccountModel* account;
	string content;
	vector<DonMediaAttachmentModel*> media_attachments;
	bool is_boost = false;

	//ブースト情報
	string reblog_avatar;
	string reblog_content;
	string reblog_acct;
	string reblog_name;
	string reblog_created_at;
	vector<DonMediaAttachmentModel*> reblog_media_attachments;
};



/***
DonTimeLineModelクラス

・DonTootModelクラス（エンティティ）のリストを保持する。
・上記のメモリ管理を行う。
・重複するものは即時deleteされる。

*/
class DonTimeLineModel {
public:
	DonTimeLineModel() {
	//
	}
	~DonTimeLineModel() {
		vector<DonTootModel*>::iterator itr = list.begin();
		while (itr!=list.end()) {
			delete *itr;
			++itr;
		}
	}
	//取得したトゥートがすでに登録されていない場合のみ登録
	//戻り値は追加したトゥートもしくはすでにあったトゥートのインスタンス（のポインタ）
	DonTootModel* addToot(DonTootModel* toot) {
		vector<DonTootModel*>::iterator itr;
		for (itr = list.begin(); itr != list.end(); ++itr) {
			if ((*itr)->getId() == toot->getId()) {
				delete toot;
				return *itr;
			}
		}
		list.push_back(toot);
		last_at = toot->getCreatedAt();
		return toot;
	}
	vector<DonTootModel*> getList() {
		return list;
	}
	string getLastAt() {
		return last_at;
	}
	void sort() {
		std::sort(list.begin(), list.end(), [](DonTootModel* left, DonTootModel* right) {
			return left->getCreatedAt() < right->getCreatedAt();
		});
	}
	void clear() {
		vector<DonTootModel*>::iterator itr = list.begin();
		while (itr != list.end()) {
			delete (*itr);
			++itr;
		}
		list.clear();
	}
private:
	vector<DonTootModel*> list;
	string last_at;
};

/***
DonAccountListModelクラス

・DonAccountModelクラス（エンティティ）のリストを保持する。
・上記のメモリ管理を行う。
・重複するものは即時deleteされる。

*/
class DonAccountListModel {
public:
	DonAccountListModel() {
		//
	}
	~DonAccountListModel() {
		vector<DonAccountModel*>::iterator itr = list.begin();
		while (itr != list.end()) {
			delete *itr;
			++itr;
		}
	}
	//取得したアカウントがすでに登録されていない場合のみ登録
	//戻り値は追加したアカウントもしくはすでにあったアカウントのインスタンス（のポインタ）
	DonAccountModel* addAccount(DonAccountModel* pAccount) {
		vector<DonAccountModel*>::iterator itr;
		for (itr = list.begin(); itr != list.end(); ++itr) {
			if ((*itr)->getId() == pAccount->getId()) {
				delete pAccount;
				return *itr;
			}
		}
		list.push_back(pAccount);
		return pAccount;
	}
	vector<DonAccountModel*> getList() {
		return list;
	}
private:
	vector<DonAccountModel*> list;

};