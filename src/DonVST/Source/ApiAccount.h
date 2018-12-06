/**
* @file ApiAccount.h
* @brief APIアクセスに必要な各種コンテナクラス用のヘッダファイル
* @author h.wakimoto
* @date 2017/05/01
*/

#pragma once

#include "TootModel.h"

#include <string>

using namespace std;

/***
LoginInfoクラス。
AccessTokenを取得するまでに必要な情報を格納するためのエンティティ。
*/
class LoginInfo {
public:

	/**
	* @brief コンストラクタ
	*/	LoginInfo() {

	}

    /**
    * @brief デストラクタ
    */
	~LoginInfo() {

	}

	/**
	* @brief 基本事項をセットする
	* @detail AuthCode取得のためのサーバへの初回アクセス用。ブラウザでのコード取得を行う方式を想定。
	* パスワード方式の場合はユーザ名とパスワードもセットする必要があるが
	* やってみたところなぜかこれはうまく行かなかったし、セキュリティ的にものぞましくない。
	* @todo HostUrlには最後のスラッシュがあってはならないのであれば削除する。
	*/
	void setInfo(string pHosturl) {
		hostUrl = pHosturl;
	}

	/**
	* @brief クライアント情報をセットする
	* @detail AuthCode取得のためのサーバへの2回めのアクセス用。
	*/
	void setClientInfo(string pClientId, string pClientSecuret) {
		clientId = pClientId;
		clientSecret = pClientSecuret;
	};

	/**
	* @brief authCodeのSetter
	*/
	void setAuthCode(string pAuthCode) {
		authCode = pAuthCode;
	}

	/**
	* @brief hostUrlのGetter
	*/
	string getHostUrl() {
		return hostUrl;
	}

	/**
	* @brief clientIdのGetter
	*/
	string getClientId() {
		return clientId;
	}

	/**
	* @brief clientSecretのGetter
	*/
	string getClientSecret() {
		return clientSecret;
	}

	/**
	* @brief authCodeのGetter
	*/
	string getAuthCode() {
		return authCode;
	}


private:
	//!APIアクセスするインスタンスのURL
	string hostUrl;

	//!クライアントID(AuthCodeを取得するために必要)
	string clientId;

	//!クライアントシークレットキー(AuthCodeを取得するために必要)
	string clientSecret;

	//!Auth Code。これを取得するのが目的
	string authCode;
};


/***
AccessInfoクラス。
AccessTokenを取得してからの情報を格納するためのエンティティ。
*/
class AccessInfo {

public:

	/**
	* @brief コンストラクタ
	*/
	AccessInfo() {
		account = new DonAccountModel();
	}

	/**
	* @brief デストラクタ
	*/
	~AccessInfo() {
		delete account;
	}

	/**
	* @brief 基本情報をセットする
	* @detail APIにアクセスするのに必要なのはURLとAccessToken。
	* アカウント情報はAccessTokenに紐付いてサーバ側が保持している。
	* accountにはセッターはないが、コンストラクタで生成済みなので、getAccount()->set???()で各パラメータをセットする。
	* @todo HostUrlには最後のスラッシュがあってはならないのであれば削除する。
	*/
	void setInfo(string pHostUrl, string pAccessToken) {
		this->hostUrl = pHostUrl;
		this->accessToken = pAccessToken;
	}

	/**
    * @brief hostUrlのGetter
	*/
	string getHostUrl() {
		return hostUrl;
	}

	/**
	* @brief accessTokenのGetter
	*/
	string getAccessToken() {
		return accessToken;
	}

	/**
	* @brief accountのGetter
	*/
	DonAccountModel* getAccount() {
		return account;
	}

private:

	//!APIアクセスするインスタンスのURL
	string hostUrl;

	//!APIアクセスに必要なアクセストークン
	string accessToken;

	//!アカウント情報
	DonAccountModel* account;
};


