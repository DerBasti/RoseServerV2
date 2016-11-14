#ifdef _MSC_VER
#pragma once
#endif

#ifndef __STRING_WRAPPER__
#define __STRING_WRAPPER__

#include <string>
#include <cstring>
#include <vector>
#include <time.h>
#include <iostream>

#ifdef _MSC_VER
#define SPRINTF sprintf_s
#else
#define SPRINTF sprintf
#endif

class String {
	private:
		std::string content;
	public:
		String() {
			this->content = std::string("");
		}
		String(const unsigned char c) {
			char buf[0x02] = { (char)c, 0x00 };
			this->content = std::string(buf);
		}
		explicit String(const char *buf) {
			this->content = std::string(buf == nullptr ? "" : buf);
		}
		String(const std::string other) {
			this->content = other;
		}
		String(const String& str) {
			(*this) = str;
		}
		~String() {
			this->content.clear();
		}

		__inline String& operator=(const char *buf) {
			this->content = std::string(buf);
			return (*this);
		}

		__inline String& operator=(const String& s) {
			this->content = std::string(s.content);
			return (*this);
		}

		__inline bool operator<(const String& s) const {
			return strcmp(this->content.c_str(), s.content.c_str()) < 0;
		}

		__inline String& operator+=(const String& other) {
			this->content = this->content.append(other.content);
			return (*this);
		}
		__inline String& operator+=(const char *other) {
			this->content = this->content.append(other);
			return (*this);
		}
		__inline String& operator+=(const unsigned char& c) {
			char buf[0x02] = {0x00};
			SPRINTF(buf, "%c", c);
			this->content += std::string(buf);
			return (*this);
		}

		__inline String operator+(const String& other) const {
			return String(std::string(this->content).append(other.content));
		}

		__inline String operator+(const char* other) const {
			return String(std::string(this->content).append(other));
		}

		__inline char operator[](unsigned int idx) const {
			return (idx >= this->length()) ? '?' : this->content[idx];
		}

		__inline bool operator==(const String& other) const {
			return this->contentEquals(other);
		}

		__inline bool operator!=(const String& other) const {
			return !(this->operator==(other));
		}

		__inline bool isEmpty() const {
			return this->length() <= 0;
		}

		__inline unsigned int length() const {
			return static_cast<unsigned int>(this->content.length());
		}

		__inline bool contains(const char *buf) const {
			return this->contains(std::string(buf));
		}

		__inline bool contains(const std::string other) const {
			return ((int)this->content.find(other.c_str()) >= 0);
		}

		__inline bool contentEquals(const char *arr) const {
			return this->contentEquals(String(arr));
		}

		bool contentEquals(const String& str) const {
			if (str.length() != this->length()) {
				return false;
			}
			for (unsigned int i = 0; i < this->length(); i++) {
				if (toupper(this->content[i]) != toupper(str.content[i])) {
					return false;
				}
			}
			return true;
		}

		void replace(const char tokenToFind, const char tokenToReplaceWith);
		String replaceEx(const char tokenToFind, const char tokenToReplaceWith) {
			String rep = (*this);
			rep.replace(tokenToFind, tokenToReplaceWith);
			return rep;
		}
		void replaceOnce(const char tokenToFind, const char *tokenToReplaceWith) {
			for (unsigned int i = 0; i < this->length(); i++) {
				if (this->content[i] == tokenToFind) {
					std::string newContent = std::string();
					if (i > 0) {
						newContent = this->content.substr(0, i);
					}
					newContent += std::string(tokenToReplaceWith);
					if (i + 1 < this->length()) {
						newContent += this->content.substr(i + 1);
					}
					this->content = newContent;
					break;
				}
			}
		}
		void replaceOnce(const char tokenToFind, const String &tokenToReplaceWith) {
			this->replaceOnce(tokenToFind, tokenToReplaceWith.toConstChar());
		}

		void remove(const char token);
		String removeEx(const char token);


		__inline String substring(const unsigned int startPos) const {
			return this->substring(startPos, 0);
		}

		__inline String substring(const unsigned int startPos, int length) const {
			unsigned int totalPos = startPos + length;
			if (static_cast<signed>(totalPos) < 0 || startPos >= this->length()) {
				return String();
			}
			if (totalPos > this->length()) {
				length = this->length() - startPos;
			}
			return this->content.substr(startPos, (length > 0 ? length : (this->length() - startPos)));
		}

		__inline bool endsWith(const String& cmp) const {
			return this->endsWith(cmp.toConstChar());
		}
		bool endsWith(const char *cmp) const {
			size_t len = strlen(cmp);
			if(this->content.length() < len) {
				return false;
			}
			for(size_t i=0, j=0;i<this->content.length() && j<len;i++, j++) {
				const char cont = this->content[this->content.length()-i-1];
				const char c = cmp[len-j-1];
				if(cont != c) {
					return false;
				}
			}
			return true;
		}

		__inline size_t lastPositionOf(const char *buf) const {
			return this->lastPositionOf(String(buf));
		}

		 size_t lastPositionOf(const String& str) const {
			 if (str.length() >= this->length()) {
				 return 0;
			 }
			 unsigned int pos = this->length();
			 bool overlap = true;
			 for (unsigned int i = this->length(); i > 0 && i >= str.length(); i--) {
				 if (this->content[i-1] == str.content[str.length()-1]) {
					 overlap = true;
					 for (int k=str.length()-1;k >= 0;k--) {
						 //fghi
						 //fg
						 //i-1 = 1, k = 1, len = 2
						 const char* tempArr = &this->content.c_str()[(i - 1) - (str.length() - k - 1)];
						 if (this->content[(i - 1) - (str.length() - k - 1)] != str[k]) {
							 i -= str.length() - k - 1;
							 overlap = false;
							 break;
						 }
					 }
					 if (overlap) {
						 return (i-str.length());
					 }
				 }
			 }
			 return pos;
		}

		 __inline void clear() {
			 this->content.clear();
		 }

		__inline size_t positionOf(const char *buf) const {
			return this->content.find(buf);
		}

		__inline size_t positionOf(const std::string buf) const {
			return this->positionOf(buf.c_str());
		}

		String* split(const char *splitToken, unsigned long *splitAmount) const;

		__inline const char *toConstChar() const {
			return this->content.c_str();
		}

		__inline bool toBool() const {
			return _stricmp("true", this->content.c_str()) == 0;
		}
		__inline unsigned char toByte() const {
			return static_cast<unsigned char>(this->toInt());
		}
		__inline unsigned short toShort() const {
			return static_cast<unsigned short>(this->toInt());
		}
		__inline unsigned int toInt() const {
			return atoi(this->content.c_str());
		}
		__inline double toDouble() const {
			return atof(this->content.c_str());
		}
		__inline float toFloat() const {
			return static_cast<float>(this->toDouble());
		}

		static String fromBool(const bool value) {
			char buf[0x6] = { 0x00 };
			SPRINTF(buf, "%s", (value == true ? "true" : "false"));
			return String(buf);
		}

		static String fromInt(const unsigned long num) {
			char buf[0x30] = { 0x00 };
			SPRINTF(buf, "%ld", num);
			return String(buf);
		}

		static String fromLong(const unsigned long long n) {
			char buf[0x80] = { 0x00 };
			SPRINTF(buf, "%llu", n);
			return String(buf);
		}

		static String fromHex(const void *p) {
			char buf[0x80] = { 0x00 };
			SPRINTF(buf, "0x%p", p);
			return String(buf);
		}

		static String fromHex(const unsigned long long p) {
			char buf[0x80] = { 0x00 };
			SPRINTF(buf, "0x%x", p);
			return String(buf);
		}

		static String fromFloat(const float f) {
			char buf[0x100] = { 0x00 };
			SPRINTF(buf, "%f", f);
			return String(buf);
		}

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif
		static String fromIntArray(const unsigned int amount, unsigned int *arr) {
			char buf[0x300] = {0x00};
			SPRINTF(buf, "%i", arr[0]);
			for(unsigned int i=1;i<amount;i++) {
				char *tmp = &buf[strlen(buf)];
				sprintf(tmp, ",%i", arr[i]);
			}
			return String(buf);
		}
#ifdef _MSC_VER
#pragma warning(default:4996)
#endif

		friend std::ostream& operator<<(std::ostream& out, const String& s) {
			const char *ptr = s.toConstChar();
			out << ptr;
			return out;
		}
};

#endif
