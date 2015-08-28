/*
 * HttpResult.cpp
 *
 *  Created on: 2014-8-22
 *      Author: manson
 */

#include "ota/net/HttpResult.h"
#include "utils/Logger.h"
#include "utils/StringUtils.h"

namespace ota {

HttpResult::HttpResult() {
	// TODO Auto-generated constructor stub

}

HttpResult::~HttpResult() {
	// TODO Auto-generated destructor stub
}

bool HttpResult::Load(const std::string &result) {
	if (result.size() == 0) {
		return false;
	}

	Json::Reader reader;
	if (!reader.parse(result, m_Result)) {
		Log("http result parse failed : %s", result.c_str());
		return false;
	}

	if (m_Result["status"].isNull() || m_Result["url"].isNull()
			|| m_Result["code"].isNull() || m_Result["md5"].isNull()
			|| m_Result["size"].isNull()) {
		Log("missing element in HttpResult");
		return false;
	}

	return true;
}

std::string HttpResult::ToString() {
	return m_Result.toString();
}

int HttpResult::GetStatus() {
	return m_Result["status"].asInt();
}

std::string HttpResult::GetUrl() {
	return m_Result["url"].asString();
}

std::string HttpResult::GetCode() {
	return m_Result["code"].asString();
}

std::string HttpResult::GetMd5() {
	return m_Result["md5"].asString();
}

std::string HttpResult::GetFileName() {
	return utils::StringUtils::GetFileNameFromUrl(GetUrl());
}

double HttpResult::GetFileSize() {
	return m_Result["size"].asDouble();
}

void HttpResult::LogSelf() {
	Log("result : %s", m_Result.toString().c_str());
}

}
