/**
 * @file WebAccess.cpp
 * @brief DonWebBrowserクラス用のソースファイル
 * @author h.wakimoto
 * @date 2017/05/01
 */


#include "WebAccess.h"


#include "../include/curl/curl.h"
#include <iostream>
#include "../include/picojson.h"

/*****

API処理関係
curlの処理はここに集約。

・WebAccessクラス
・その他関連

*/


using namespace picojson;


typedef struct {
	char *data;  // response data from server
	size_t size; // response size of data
} MEMFILE;

MEMFILE *memfopen() {
	MEMFILE *mf = (MEMFILE *)malloc(sizeof(MEMFILE));
	mf->data = NULL;
	mf->size = 0;
	return mf;
}

void memfclose(MEMFILE *mf) {
	if (mf->data)
		free(mf->data);
	free(mf);
}

size_t memfwrite(char *ptr, size_t size, size_t nmemb, void *stream) {
	MEMFILE *mf = (MEMFILE *)stream;
	int block = size * nmemb;
	if (!mf->data)
		mf->data = (char *)malloc(block);
	else
		mf->data = (char *)realloc(mf->data, mf->size + block);
	if (mf->data) {
		memcpy(mf->data + mf->size, ptr, block);
		mf->size += block;
	}
	return block;
}

char *memfstrdup(MEMFILE *mf) {
	char *buf = (char *)malloc(mf->size + 1);
	memcpy(buf, mf->data, mf->size);
	buf[mf->size] = 0;
	return buf;
}


/* WebAccessクラス */

WebAccess::WebAccess(){
}

WebAccess::~WebAccess() {

}

/***
* Authコード取得のためのURLを取得
*
*/
string WebAccess::getCodeURL(LoginInfo* logininfo) {

	char error[256];
	string log;
	string access_token;

	//クライアントIDの取得

	MEMFILE *mf1 = memfopen();
	CURL *curl1 = curl_easy_init();

	string unique_id = "7";//!!TODOランダムな文字列を取得
	string client_name;
	client_name = "vocalodon_vst_" + unique_id;

	string url = logininfo->getHostUrl() + "/api/v1/apps";
	string postfields = "client_name=vocalodon_vst&redirect_uris=urn:ietf:wg:oauth:2.0:oob&scopes=read";

	curl_easy_setopt(curl1, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl1, CURLOPT_POSTFIELDS, postfields.c_str());
	curl_easy_setopt(curl1, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl1, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl1, CURLOPT_USERAGENT, "curl");
	curl_easy_setopt(curl1, CURLOPT_ERRORBUFFER, &error);
	curl_easy_setopt(curl1, CURLOPT_WRITEFUNCTION, memfwrite);
	curl_easy_setopt(curl1, CURLOPT_WRITEDATA, mf1);
	if (curl_easy_perform(curl1) != CURLE_OK) {
		cerr << error << endl;
		log += "mf1:"+string(error)+"\n";
	}
	else
	{
		log += mf1->data;

		value v;
		string err;
		parse(v, mf1->data, mf1->data + mf1->size, &err);
		if (err.empty()) {
			object obj = v.get<object>();
			logininfo->setClientInfo(obj["client_id"].to_str(), obj["client_secret"].to_str()); 

			//TODO!!デバッグ出力
			log += "client_id:" + obj["client_id"].to_str() + "\n";
			log += "client_secret:" + obj["client_secret"].to_str() + "\n";

		}
		else {
			log += "err:"+err + "\n";
		}
	}

	curl_easy_cleanup(curl1);
	memfclose(mf1);

	string return_url = logininfo->getHostUrl() + "/oauth/authorize?client_id=" + logininfo->getClientId() + "&response_type=code&redirect_uri=urn:ietf:wg:oauth:2.0:oob&scope=read";

	return return_url;

}

/***
* アクセストークンを取得（事前にAuthコードなどを取得済み）
*
*/
string WebAccess::getAccessToken(LoginInfo* logininfo) {
	char error[256];
	string log;
	string access_token;

	MEMFILE *mf = memfopen();
	CURL *curl = curl_easy_init();

	string url = logininfo->getHostUrl() + "/oauth/token";
	string postfields = "client_id=" + logininfo->getClientId() + "&client_secret=" + logininfo->getClientSecret() + "&code=" + logininfo->getAuthCode() + "&redirect_uri=urn:ietf:wg:oauth:2.0:oob&grant_type=authorization_code";

	log += "url:" + url + "\n";
	log += "postfields:" + postfields + "\n";

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl");
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &error);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memfwrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, mf);

	if (curl_easy_perform(curl) != CURLE_OK) {
		cerr << error << endl;
		log += "mf:"+ string(error) +"\n";
	}
	else if (mf==nullptr) {
		cerr << error << endl;
		log += "mf:null\n";
	}
	else
	{
		//!!TODOデバッグ出力
		value v;
		string err;
		parse(v, mf->data, mf->data + mf->size, &err);
		if (err.empty()) {
			object obj = v.get<object>();
			access_token = obj["access_token"].to_str();
			log += "access_token:"+ access_token +"\n";
		}
	}
	curl_easy_cleanup(curl);
	memfclose(mf);

	//return log; //DEBUG
	return access_token;
}



/***
ログインアカウント情報を取得。引数を直接書き換え
*/

void WebAccess::getAccessInfoAdditional(AccessInfo* accessinfo) {
	char error[256];
	string log;
	string timelinestr;

	MEMFILE *mf = memfopen();
	CURL *curl = curl_easy_init();

	string url = accessinfo->getHostUrl() + "/api/v1/accounts/verify_credentials";

	struct curl_slist *headerlist = NULL;
	string auth_str = "Authorization: Bearer " + accessinfo->getAccessToken();
	headerlist = curl_slist_append(headerlist, auth_str.c_str());

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl");
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &error);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memfwrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, mf);
	if (curl_easy_perform(curl) != CURLE_OK) {
		cerr << error << endl;
		log += "mf:" + string(error) + "\n";
	}
	else if (mf == nullptr) {
		cerr << error << endl;
		log += "mf:null\n";
	}
	else
	{
		value v;
		string err;
		log += mf->data;
		parse(v, mf->data, mf->data + mf->size, &err);
		if (err.empty()) {
			object obj = v.get<object>();
			accessinfo->getAccount()->setId(stoi(obj["id"].to_str()));
			accessinfo->getAccount()->setAcct(obj["acct"].to_str());
			accessinfo->getAccount()->setAvatar(obj["avatar"].to_str());
			accessinfo->getAccount()->setDisplayName(obj["display_name"].to_str());
			accessinfo->getAccount()->setURL(obj["url"].to_str());
			accessinfo->getAccount()->setUserName(obj["user_name"].to_str());
			log += "user:" + accessinfo->getAccount()->getAcct() + "\n";
		}
	}

	curl_easy_cleanup(curl);
	memfclose(mf);
}


void WebAccess::getInstanceInfo(AccessInfo* accessinfo, DonInstanceModel* instanceinfo) {
	char error[256];
	string log;
	string timelinestr;

	MEMFILE *mf = memfopen();
	CURL *curl = curl_easy_init();

	string url = accessinfo->getHostUrl() + "/api/v1/instance";

	struct curl_slist *headerlist = NULL;
	string auth_str = "Authorization: Bearer " + accessinfo->getAccessToken();
	headerlist = curl_slist_append(headerlist, auth_str.c_str());

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl");
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &error);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memfwrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, mf);
	if (curl_easy_perform(curl) != CURLE_OK) {
		cerr << error << endl;
		log += "mf:" + string(error) + "\n";
	}
	else if (mf == nullptr) {
		cerr << error << endl;
		log += "mf:null\n";
	}
	else
	{
		value v;
		string err;
		log += mf->data;
		parse(v, mf->data, mf->data + mf->size, &err);
		if (err.empty()) {
			object obj = v.get<object>();
			instanceinfo->setUri(obj["uri"].to_str());
			instanceinfo->setTitle(obj["title"].to_str());
			instanceinfo->setDescription(obj["description"].to_str());
			instanceinfo->setEmail(obj["email"].to_str());
			instanceinfo->setVersion(obj["version"].to_str());
			log += "user:" + accessinfo->getAccount()->getAcct() + "\n";
		}
	}

	curl_easy_cleanup(curl);
	memfclose(mf);
}



/***
タイムラインを取得（共通）
*/
void WebAccess::getTimeline(
	AccessInfo* accessinfo, DonTimeLineModel* timeline, DonAccountListModel* accounts,	
	string path
) {
	char error[256];
	string log;
	string timelinestr;

	MEMFILE *mf = memfopen();
	CURL *curl = curl_easy_init();

	string url = accessinfo->getHostUrl() + path;

	struct curl_slist *headerlist = NULL;
	string auth_str = "Authorization: Bearer " + accessinfo->getAccessToken();
	headerlist = curl_slist_append(headerlist, auth_str.c_str());

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl");
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &error);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memfwrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, mf);
	if (curl_easy_perform(curl) != CURLE_OK) {
		cerr << error << endl;
		log += "mf:" + string(error) + "\n";
	}
	else if (mf == nullptr) {
		cerr << error << endl;
		log += "mf:null\n";
	}
	else
	{
		value v;
		string err;
		log += mf->data;
		parse(v, mf->data, mf->data + mf->size, &err);
		if (err.empty()) {
			picojson::array arr = v.get<picojson::array>();
			picojson::array::iterator it;
			for (it = arr.begin(); it != arr.end(); it++) {
				object j_toot = it->get<object>();
				timelinestr += j_toot["id"].to_str() + "\n" + j_toot["created_at"].to_str() + "\n" + j_toot["visibility"].to_str() + "\n";

				object j_account = j_toot["account"].get<object>();
				timelinestr += j_account["username"].to_str() + ":" + j_account["acct"].to_str() + "\n";
				timelinestr += j_toot["content"].to_str() + "\n";
				timelinestr += "--------------------------------------------------------------\n";

				DonAccountModel* account = new DonAccountModel(j_account);
				account = accounts->addAccount(account);//追加または既存のアカウントのポインタが返される
				DonTootModel* toot = new DonTootModel(j_toot, account);

				picojson::array j_media_attachments = j_toot["media_attachments"].get<picojson::array>();
				picojson::array::iterator itr = j_media_attachments.begin();
				while (itr != j_media_attachments.end()) {
					object j_media_attachment = itr->get<object>();
					toot->addMediaAttachments(
						stoi(j_media_attachment["id"].to_str()),
						j_media_attachment["type"].to_str(),
						j_media_attachment["preview_url"].to_str(),
						j_media_attachment["size"].to_str()
					);
					++itr;
				}

				toot = timeline->addToot(toot);//追加または既存のトゥートのポインタが返される
			}
		}
	}

	curl_easy_cleanup(curl);
	memfclose(mf);

}


/***
* ホームタイムラインを取得
*
*/
void WebAccess::getCurrentHomeTimeline(AccessInfo* accessinfo, DonTimeLineModel* timeline, DonAccountListModel* accounts) {
	getTimeline(accessinfo, timeline, accounts, "/api/v1/timelines/home");
}


/***
 * 連合タイムラインを取得 
 *
 */
void WebAccess::getCurrentPublicTimeline(AccessInfo* accessinfo, DonTimeLineModel* timeline, DonAccountListModel* accounts) {
	getTimeline(accessinfo, timeline, accounts, "/api/v1/timelines/public");
}

/***
* ローカルタイムラインを取得
*
*/
void WebAccess::getCurrentLocalTimeline(AccessInfo* accessinfo, DonTimeLineModel* timeline, DonAccountListModel* accounts) {
	getTimeline(accessinfo, timeline, accounts, "/api/v1/timelines/public?local=true");
}
/***
* スーパーローカルタイムラインを取得
*
*/
void WebAccess::getCurrentSuperLocalTimeline(AccessInfo* accessinfo, DonTimeLineModel* timeline, DonAccountListModel* accounts) {
	getTimeline(accessinfo, timeline, accounts, "/api/v1/timelines/public?local=true");
	getTimeline(accessinfo, timeline, accounts, "/api/v1/timelines/home");//重複は自動的に削除される。
	
}

