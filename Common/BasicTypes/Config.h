#ifdef _MSC_VER
#pragma once
#endif

#ifndef __CONFIG__
#define __CONFIG__

#include "BasicObject.h"
#include "FileReader.h"
#include <map>

class Config {
private:
	std::map<String, String> content;
	String filePath;
public:
	Config() {
		content.clear();
	}
	explicit Config(const char *path) : Config(String(path)) {
	}
	explicit Config(const String& path) {
		this->filePath = path;

		FileReader fr(this->getPath());
		String line = String();
		while (!fr.hasInvalidState()) {
			line = fr.readLine();
			if (line.isEmpty() || (line.getLength() == 1 && (line[0] == '\r' || line[0] == '\n'))) {
				continue;
			}
			SharedArrayPtr<String> tokenized(line.split('=', nullptr), 2);
			if (tokenized.at(0)[0] == '#') {
				continue;
			}
			this->content[tokenized.at(0)] = tokenized.at(1);
		}
	}
	Config& operator=(const Config& conf) {
		std::for_each(conf.content.begin(), conf.content.end(), [&](std::pair<String, String> p) {
			this->content[p.first] = p.second;
		});
		this->filePath = conf.getPath();
		return (*this);
	}
	__inline String getPath() const {
		return this->filePath;
	}
	__inline String get(const char* key) const {
		return this->get(String(key));
	}
	__inline String get(const String& key) const {
		return this->get(key, String());
	}
	__inline String get(const char* key, const char* defaultValue) const {
		return this->get(String(key), String(defaultValue));
	}
	String get(const String& key, const String& defaultValue) const {
		auto it = this->content.find(key);
		return (it != this->content.end() ? (*it).second : defaultValue);
	}
};

#endif //__CONFIG_READER__