#include "Networking.h"
#include "network/HttpClient.h"
#include "string"
#include "vector"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "cocos2d.h"


USING_NS_CC;
NetworkingService::NetworkingService() {
	this->_serverUrl = "http://localhost:3000";
	this->_serverSignInUrl = "http://localhost:3000/sign-in";
}

NetworkingService* NetworkingService::getInstance() {
	static NetworkingService instance;
	return &instance;
}

std::string NetworkingService::jsonify(std::initializer_list<std::pair<std::string, std::string>> list) {
	rapidjson::Document d;
	d.SetObject();
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

	for (auto& item : list) {
		d.AddMember(rapidjson::Value(item.first.c_str(), allocator), rapidjson::Value(item.second.c_str(), allocator), allocator);
	}

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	return buffer.GetString();
}

void NetworkingService::signIn(std::string name) {

	if (name.empty()) {
		log("name is empty");
		return;
	}

	// Http Client
	auto* req = new network::HttpRequest();
	req->setUrl(this->_serverSignInUrl);
	req->setRequestType(cocos2d::network::HttpRequest::Type::POST);

	std::string postData = jsonify({ 
		{"name", name}
	});
	req->setRequestData(postData.c_str(), postData.size());

	std::vector<std::string> headers;
	headers.push_back("Content-Type: application/json");
	req->setHeaders(headers);

	req->setResponseCallback([&](network::HttpClient* sender, network::HttpResponse* response) {
		
		if (!response || !response->isSucceed()) {
			log("Error in req");
			return;
		}

		long resCode = response->getResponseCode();
		log("Https status code recieved: %ld", resCode);

		std::vector<char>*buffer = response->getResponseData();
		std::string responseStr(buffer->begin(), buffer->end());
		log("Response data: %s", responseStr.c_str());

		rapidjson::Document doc;
		doc.Parse(responseStr.c_str());

		if (doc.HasParseError()) {
			log("Got error while parsing in doc");
			return;
		}

		if (resCode == 200 && doc.HasMember("succes") && doc["success"].GetBool()) {
			log("Login successfully");

		}

		if (doc.IsObject()) {
			std::string message = doc.HasMember("message") && doc["message"].IsString() ? doc["message"].GetString() : "";
			std::string name = doc.HasMember("name") && doc["name"].IsString() ? doc["name"].GetString() : "";
			std::string deviceId = doc.HasMember("deviceId") && doc["deviceId"].IsString() ? doc["deviceId"].GetString() : "";
			std::string userId = doc.HasMember("userId") && doc["userId"].IsString() ? doc["userId"].GetString() : "";


			log("Server Message: %s", message.c_str());
			log("User Name: %s", name.c_str());
			log("Device ID: %s", deviceId.c_str());
			log("User ID: %s", userId.c_str());

			UserDefault::getInstance()->setStringForKey("userName", name);
			UserDefault::getInstance()->setStringForKey("userId", userId);

		}
	});

	network::HttpClient::getInstance()->send(req);
	req->release();
}