#ifndef __STRING_WRAPPER__
#define __STRING_WRAPPER__

#ifdef _MSC_VER
#pragma once
#endif

#include <string.h>
#include "QuickMakros.h"

class SafeChar {
private:
	char *data;
	char *endData;
	__inline void safeAssign(char* newData) {
		if (this->data) {
			delete[] this->data;
		}
		this->data = newData;
	}
public:
	SafeChar() {
		data = nullptr;
		endData = nullptr;
	}
	virtual ~SafeChar() {
		if (data) {
			delete[] data;
		}
		data = nullptr;
	}

	FORWARD_ITERATOR_CREATOR(iterator, char);

	__inline SafeChar& operator=(SafeChar&& other) {
		this->safeAssign(other.data);
		this->endData = other.endData;
		other.data = other.endData = nullptr;
		return (*this);
	}

	__inline SafeChar& operator=(std::nullptr_t) {
		this->safeAssign(nullptr);
		this->endData = nullptr;
		return (*this);
	}

	__inline iterator begin() {
		return iterator(this->data);
	}

	__inline iterator end() {
		return iterator(this->endData);
	}

	__inline iterator beginConst() const {
		return iterator(this->data);
	}

	__inline iterator endConst() const {
		return iterator(this->endData);
	}

	__inline void set(char *newData, const size_t size) {
		this->safeAssign(newData);
		this->data[size] = 0x00;
		this->endData = &newData[size];
	}

	operator char*() {
		return this->data;
	}

	__inline char* get() const {
		return this->data;
	}

	__inline const char* toConstChar() const {
		return this->data;
	}
};

class String {
private:
	SafeChar data;
	unsigned long length;

protected:
	void mergeData(const String& str) {
		String copy(*this);
		this->mergeData(copy.toConstChar(), copy.getLength(), str.toConstChar(), str.getLength());
	}
	void mergeData(const char* d) {
		String copy(*this);
		this->mergeData(copy.toConstChar(), copy.getLength(), d, (d == nullptr ? 0x00 : strlen(d)));
	}
	void mergeData(const char* d, const size_t len) {
		String copy(*this);
		this->mergeData(copy.toConstChar(), copy.getLength(), d, len);
	}
	void mergeData(const char* d1, const char *d2) {
		size_t length[2] = { 0x00 };
		length[0] = (d1 ? strlen(d1) : 0x00);
		length[1] = (d2 ? strlen(d2) : 0x00);
		this->mergeData(d1, length[0], d2, length[1]);
	}
	void mergeData(const char *d1, const size_t s1, const char *d2, const size_t s2) {
		this->data.set(new char[s1 + s2 + 1], s1 + s2);
		this->copyData(d1, s1);
		this->copyData(d2, s2, s1);

		this->length = s1 + s2;
	}

	void assignData(const char* data, const size_t len) {
		this->length = len;
		this->data.set(new char[this->length + 1], this->length);
		this->copyData(data, len);
	}

	__inline void copyData(const char *data, const size_t len) {
		this->copyData(data, len, 0);
	}

	__inline void copyData(const char *data, const size_t len, const size_t offset) {
		memcpy(this->data + offset, data, len);
		//std::copy(data, data + len, this->begin() + offset);
	}

	size_t getSafeSize(const char* token) const {
		if (!token) {
			return INVALID_LENGTH;
		}
		size_t len = strlen(token);
		if (len > this->getLength()) {
			len = INVALID_LENGTH;
		}
		return len;
	}
	template<class _ValueType>
	static String internalFromConverter(const char* format, _ValueType value) {
		char buf[0x80] = { 0x00 };
		sprintf_s(buf, format, value);
		return String(buf);
	}
public:
	typedef SafeChar::iterator iterator;
	const static size_t INVALID_LENGTH = static_cast<size_t>(-1);
	String() { 
		this->assignData(nullptr, 0);
		this->length = 0;
	}

	String(const char* str) : String() {
		this->assignData(str, str == nullptr ? 0x00 : strlen(str));
	}

	String(const String& other) {
		this->assignData(other.data.toConstChar(), other.length);
	}

	String(String&& other) {
		this->data = std::move(other.data);
		this->length = std::move(other.length);
	}

	__inline bool operator<(const String& other) const {
		return strcmp(this->data.toConstChar(), other.data.toConstChar()) < 0;
	}

	String& operator=(const String& other) {
		this->assignData(other.data.get(), other.getLength());
		return (*this);
	}

	String& operator=(String&& other) {
		this->data = std::move(other.data);
		this->length = std::move(other.length);
		return (*this);
	}

	String operator+(const char* str) {
		String result;
		result.mergeData(this->data, str);
		return result;
	}

	String operator+(const String& str) {
		String result;
		result.mergeData(this->data, str.data.get());
		return result;
	}

	String& operator+=(unsigned char c) {
		char asSigned = static_cast<char>(c);
		this->mergeData(&asSigned, static_cast<size_t>(1));
		return (*this);
	}

	String& operator+=(const char* str) {
		this->mergeData(str);
		return (*this);
	}

	String& operator+=(const String& str) {
		this->mergeData(str);
		return (*this);
	}

	__inline operator const char*() const {
		return this->data.toConstChar();
	}
	__inline operator char*() {
		return this->data;
	}

	__inline iterator begin(){
		return this->data.begin();
	}
	__inline iterator end() {
		return this->data.end();
	}

	__inline iterator beginConst() const {
		return this->data.beginConst();
	}
	__inline iterator endConst() const {
		return this->data.endConst();
	}

	__inline String substring(unsigned int start) const {
		return this->substring(start, this->getLength() - start);
	}

	__inline String substring(unsigned int start, unsigned int length) const {
		String result;
		if (length > 0) {
			result.assignData(this->toConstChar() + start, length);
		}
		return result;
	}

	bool startsWith(const char* token) const {
		size_t len = this->getSafeSize(token);
		if (len == INVALID_LENGTH) {
			return false;
		}
		const char* tmpData = this->toConstChar();
		for (unsigned int i = 0; i < len; i++) {
			if (*tmpData != *token) {
				return false;
			}
			tmpData++; token++;
		}
		return len > 0;
	}
	__inline bool startsWith(const String& other) const {
		return this->startsWith(other.toConstChar());
	}

	unsigned long findFirstOf(const char token) const {
		return this->findFirstOf(token, 0);
	}

	unsigned long findFirstOf(const char token, const size_t offset) const {
		char buf[0x02] = { token, 0x00 };
		return this->findFirstOf(buf, offset);
	}

	unsigned long findFirstOf(const char* token) const {
		return this->findFirstOf(token, 0);
	}

	unsigned long findFirstOf(const char* token, const size_t offset) const {
		size_t len = this->getSafeSize(token);
		if (len == INVALID_LENGTH || (len + offset) > this->getLength()) {
			return INVALID_LENGTH;
		}
		const char* thisData = this->toConstChar() + offset;
		for (unsigned int i = offset; i < this->getLength(); i++) {
			const char *otherToken = token;
			bool found = true;
			for (unsigned int j = 0; j < len && i < this->getLength(); i++, j++) {
				if (*thisData != *otherToken) {
					found = false;
					break;
				}
				thisData++; otherToken++;
			}
			if (found) {
				return i - len;
			}
			thisData++;
		}
		return INVALID_LENGTH;
	}

	unsigned long findLastOf(const char* token) const {
		size_t len = this->getSafeSize(token);
		if (len == INVALID_LENGTH) {
			return false;
		}
		const char* thisData = this->toConstChar() + this->getLength() - 1;
		for (int i = this->getLength() - 1; i >= 0; i--) {
			const char* tmpToken = token + len - 1;
			bool found = true;
			for (int j = len - 1; j >= 0 && i >= 0; j--, i--) {
				if (*thisData != *tmpToken) {
					found = false;
					break;
				}
				thisData--; tmpToken--;
			}
			if (found) {
				return i+1;
			}
			thisData--;
		}
		return INVALID_LENGTH;
	}
	unsigned long findLastOf(const char token) const {
		char buf[0x02] = { token, 0x00 };
		return this->findLastOf(buf);
	}

	bool contains(const char* token) const {
		return this->contains(token, false);
	}

	bool contains(const char* token, bool ignoreCaseSensitivity) const {
		size_t len = this->getSafeSize(token);
		if (len == INVALID_LENGTH) {
			return false;
		}
		bool ranThrough = true;

		const char *thisData = this->toConstChar();
		for (unsigned int i = 0; i < this->getLength(); i++) {
			ranThrough = true;
			const char* tmpToken = token;
			for (unsigned int j = 0; j < len && i < this->getLength(); i++, j++) {
				char currentToken = static_cast<char>(ignoreCaseSensitivity ? toupper(*tmpToken) : *tmpToken);
				char currentChar = static_cast<char>(ignoreCaseSensitivity ? toupper(*thisData) : *thisData);
				if (currentChar != currentToken) {
					ranThrough = false;
					break;
				}
				thisData++; tmpToken++;
			}
			if (ranThrough) {
				return true;
			}
			thisData++;
		}
		return false;
	}

	bool contentEquals(const char* str) const {
		size_t len = this->getSafeSize(str);
		if (len == INVALID_LENGTH || len != this->getLength()) {
			return false;
		}
		const char *dataPtr = this->toConstChar();
		for (unsigned int i = 0; i < len; i++) {
			if (toupper(*dataPtr) != toupper(*str)) {
				return false;
			}
			dataPtr++; str++;
		}
		return len > 0;
	}

	bool endsWith(const char* token) const {
		size_t len = this->getSafeSize(token);
		if (len == INVALID_LENGTH) {
			return false;
		}
		const char *dataPtr = this->toConstChar() + this->getLength() - 1;
		const char *tokenEnd = token + len - 1;
		for (int i = this->getLength() - 1, j = len - 1; i >= 0 && j >= 0; i--, j--) {
			if (*dataPtr != *tokenEnd) {
				return false;
			}
			dataPtr--; tokenEnd--;
		}
		return len > 0;
	}

	__inline bool endsWith(const String& other) const {
		return this->endsWith(other.toConstChar());
	}

	void replace(const char* tokenToFind, const char* tokenToReplaceWith) {
		size_t len = this->getSafeSize(tokenToFind);
		if (len == INVALID_LENGTH) {
			return;
		}
		String replaceToken = String(tokenToReplaceWith);
		String result = (*this);
		for (int i = this->getLength() - len - 1; i >= 0; i--) {
			size_t pos = result.findFirstOf(tokenToFind, i);
			if (pos != INVALID_LENGTH) {
				result = result.substring(0, pos) + replaceToken + result.substring(pos + len);
			}
		}
		(*this) = std::move(result);
	}

	void replaceOnce(const char tokenToFind, const char* tokenToReplaceWith) {
		char buf[2] = { tokenToFind, 0x00 };
		this->replaceOnce(buf, tokenToReplaceWith);
	}

	void replaceOnce(const char* tokenToFind, const char* tokenToReplaceWith) {
		size_t len = this->getSafeSize(tokenToFind);
		if (len == INVALID_LENGTH) {
			return;
		}
		unsigned long pos = this->findFirstOf(tokenToFind);
		if (pos != INVALID_LENGTH) {
			(*this) = this->substring(0, pos) + String(tokenToReplaceWith) + this->substring(pos + len);
		}
	}

	unsigned long getAmountOfTokens(const char token) {
		unsigned long result = 0x00;
		char* thisData = this->data;
		for (unsigned int i = 0; i < this->getLength(); i++, thisData++) {
			if (*thisData == token) {
				result++;
			}
		}
		return result;
	}

	String* split(const char token, unsigned long* lenOut) {
		unsigned int amount = getAmountOfTokens(token) + 1;
		String *holder = new String[amount];
		unsigned int lastPos = 0x00;
		unsigned int idx = 0x00;
		String cpy(*this);
		while ((lastPos = cpy.findFirstOf(token)) != INVALID_LENGTH) {
			holder[idx++] = cpy.substring(0, lastPos);
			cpy = cpy.substring(lastPos + 1);
		}
		holder[idx] = cpy;
		if (lenOut) {
			(*lenOut) = amount;
		}
		return holder;
	}

	String toUpper() const {
		String str; str.reserve(this->getLength());
		for (unsigned int i = 0; i < this->getLength(); i++) {
			str += static_cast<unsigned char>(toupper((*this)[i]));
		}
		str[this->getLength()] = 0x00;
		return str;
	}

	__inline void reserve(const unsigned long len) {
		this->data.set(new char[len + 1], len);
	}
	__inline void clear() {
		(*this) = String();
	}

	__inline const char* toConstChar() const {
		return this->data.toConstChar();
	}

	__inline size_t getLength() const {
		return this->length;
	}
	__inline bool isEmpty() const {
		return this->getLength() <= 0;
	}

	__inline bool toBool() const {
		return (this->contentEquals("true") ? true : false);
	}
	__inline float toFloat() const {
		return static_cast<float>(atof(this->toConstChar()));
	}
	__inline unsigned long toUInt() const {
		return atoi(this->toConstChar());
	}
	__inline long toInt() const {
		return static_cast<signed long>(this->toUInt());
	}
	__inline unsigned short toUShort() const {
		return static_cast<unsigned short>(this->toUInt());
	}
	__inline short toShort() const {
		return static_cast<short>(this->toUInt());
	}
	__inline unsigned char toByte() const {
		return static_cast<unsigned char>(this->toUInt());
	}

	static String fromBool(bool flag) {
		return internalFromConverter<char*>("%s", (flag == true ? "true" : "false"));
	}
	static String fromLong(const unsigned long long val) {
		return internalFromConverter<unsigned long long>("%lld", val);
	}
	static String fromHex(const unsigned long val) {
		return internalFromConverter<unsigned long>("0x%x", val);
	}
	static String fromInt(const unsigned long val) {
		return internalFromConverter<unsigned long>("%d", val);
	}
};

#endif