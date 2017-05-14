#ifndef BASE64_H
#define BASE64_H

#include <vector>
#include <string>

namespace Base64
{
	std::string base64Encryption(const std::string &);

	const std::string &SALT1 = "SALT1::LOL";
	const std::string &SALT2 = "SALT2__/LMFAO";
	const std::string &SALT3 = "SALT3--]HAHA";
	const std::string &BASE64_CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	std::string CustomEncrypt(std::string s)
	{
		s = SALT1 + s + SALT2 + SALT3;
		s = base64Encryption(s);
		s.insert(7, SALT3);
		s += SALT1;
		s = base64Encryption(s);
		s = SALT2 + SALT3 + s + SALT1;
		s = base64Encryption(s);
		s.insert(1, "L");
		s.insert(7, "M");
		return s;
	}

	std::string base64Encryption(const std::string &stringToEncode)
	{
		std::string ret;
		int val = 0;
		int bits = -6;
		const unsigned int b63 = 0x3F;

		//c is the holder variable
		for (const auto &c : stringToEncode)
		{
			//val = val * 2^8
			val = (val << 8) + c;
			//Expects octates
			bits += 8;
			while (bits >= 0)
			{
				//& is a logical AND operation
				ret.push_back(BASE64_CODES[(val >> bits) & b63]);
				//Grouping bits in groups of 6
				bits -= 6;
			}
		}

		//at least one character is inserted
		if (bits > -6)
		{
			ret.push_back(BASE64_CODES[((val << 8) >> (bits + 8)) & b63]);
		}

		//if it's less than 6 characters
		while (ret.size() % 4)
		{
			ret.push_back('=');
		}
		return ret;
	}
}
#endif 