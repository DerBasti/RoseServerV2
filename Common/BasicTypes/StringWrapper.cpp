#include "StringWrapper.h"

String* String::split(const char *splitToken, unsigned long *splitAmount) const {
    if(this->content.size() == 0) {
        return NULL;
    }
    int offPos = 0;
    std::vector<String> tmpRes;
    std::string tmp = std::string(this->content);
    while ((offPos = (int)tmp.find(splitToken)) >= 0) {
        tmpRes.push_back(tmp.substr(0, offPos));
        tmp = tmp.substr(offPos+1);
    }
    tmpRes.push_back(tmp);
                        
	String* splitArray = new String[tmpRes.size()];
	for (size_t idx = 0; idx < tmpRes.size(); idx++) {
        splitArray[idx] = String(tmpRes[idx]);
    }
	if (splitAmount) {
		(*splitAmount) = static_cast<unsigned long>(tmpRes.size());
	}
    return splitArray;
}

void String::replace(const char tokenToFind, const char tokenToReplaceWith) {
	String result = String();
	const char replaceBuf[0x02] = { tokenToReplaceWith, 0x00 };
	for (int i = this->length() - 1; i >= 0; i--) {
		if (this->content[i] == tokenToFind) {
			result = String(replaceBuf) + result;
		}
		else {
			result = String(this->content[i]) + result;
		}
	}
	(*this) = result;
}

void String::remove(const char token) {
	for (int i = this->length() - 1; i >= 0; i--) {
		if (this->content[i] == token) {
			if (i > 0) {
				std::string beforePart = this->content.substr(0, i);
				std::string afterPart = std::string();
				if (i + 1 < this->length()) {
					afterPart = this->content.substr(i + 1);
				}
				this->content = beforePart + afterPart;
			} else {
				if (this->content.length() > 1) {
					this->content = this->content.substr(1);
				} else {
					this->content = std::string();
				}
			}
		}
	}
}
String String::removeEx(const char token) {
	String cpy = (*this);
	cpy.remove(token);
	return cpy;
}