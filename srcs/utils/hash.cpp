#include <string>

const size_t HASH_LEN = 16;

std::string generateHash(const std::string &input)
{
	const std::string digits = "0123456789abcdef";
	std::string hash(HASH_LEN, '0');
	unsigned long simpleHash = 0;

	for (size_t i = 0; i < input.size(); ++i)
		simpleHash = input[i] + (simpleHash << 5) - simpleHash;

	for (size_t i = 0; i < HASH_LEN; ++i)
	{
		hash[i] = digits[simpleHash % digits.size()];
		simpleHash /= digits.size();
	}

	return (hash);
}
