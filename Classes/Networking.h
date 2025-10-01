#pragma once
#include "string"
#include "list"

class NetworkingService {

	std::string _serverUrl;
	std::string _serverSignInUrl;

	std::string jsonify(std::initializer_list<std::pair<std::string, std::string>> list);
public:
	NetworkingService();
	void signIn(std::string name);
	static NetworkingService* getInstance();
};