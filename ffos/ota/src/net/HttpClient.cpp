/*
 * HttpClient.cpp
 *
 *  Created on: 2014-5-21
 *      Author: manson
 */

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sstream>
#include <iostream>
#include <fstream>

#include "net/HttpClient.h"
#include "utils/Logger.h"
#include "net/curl/curl.h"

namespace net {

size_t saveToBuffer(char *data, size_t size, size_t nmemb,
		std::string *writerData);

HttpClient::HttpClient() {
}

HttpClient::~HttpClient() {
}

std::string HttpClient::GetRequest(const std::string &url) {
	CURL *curl;
	std::string buffer;
	char errbuf[CURL_ERROR_SIZE] = { 0 };
	long responseCode = 0;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, HTTP_REQUEST_TIME_OUT);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void * )&buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, saveToBuffer);
		CURLcode ret = curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
		curl_easy_cleanup(curl);
		if (CURLE_OK != ret || responseCode != 200) {
			Log("err info = %s", errbuf);
			Log("response code = %ld", responseCode);
		}
	}
	return buffer;
}

std::string HttpClient::PostRequest(const std::string &url,
		const std::string &data) {
	CURL *curl;
	std::string buffer;
	char errbuf[CURL_ERROR_SIZE] = { 0 };
	long responseCode = 0;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, HTTP_REQUEST_TIME_OUT);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void * )&buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, saveToBuffer);
		CURLcode ret = curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
		curl_easy_cleanup(curl);
		if (CURLE_OK != ret || responseCode != 200) {
			Log("err info = %s", errbuf);
			Log("response code = %ld", responseCode);
		}
	}
	return buffer;
}

size_t saveToBuffer(char *data, size_t size, size_t nmemb,
		std::string *writerData) {
	size_t sizes = size * nmemb;
	if (NULL == data) {
		return 0;
	}
	writerData->append(data);
	return sizes;
}

}
