#pragma once

#include <string>
#include <iostream>

bool isValidNumberString(std::string str);

class LargeInt {
private:
	//properties to store large integer. Digits are stored in numberString as string, sign is stored in sign
	std::string numberString;
	enum Sign { Positive = 1, Negative = -1 };
	Sign sign;

	//helper functions for internal use
	void normalize();
	void trimzeroforstring(std::string& str) const;
	int compare(const std::string& a, const std::string& b) const;
	void invertSign();
	std::string addHelper(std::string a, std::string b) const;
	std::string subtractHelper(std::string a, std::string b) const;
	enum PaddingType { LEFT, RIGHT };
	std::string padString(const std::string& str, size_t n, PaddingType paddingType) const;
	std::string multiplyXY(const std::string& x, const std::string& y) const;
	std::string multipleHelper(std::string lhs, std::string rhs) const;
	std::string divideHelper(std::string str1, const std::string str2) const;
	std::string powerHelper(const std::string& a, const std::string exp) const;


public:
	//constructors and copy constructor
	LargeInt() {
		numberString = "0";
		sign = Positive;
	}
	LargeInt(std::string str) {
		if (!isValidNumberString(str))
			throw std::runtime_error("Invalid string");
		numberString = str;
		normalize();
	}
	LargeInt(int num) {
		numberString = std::to_string(num);
		normalize();
	}
	LargeInt(const LargeInt& num) {
		numberString = num.numberString;
		sign = num.sign;
	}

	friend std::ostream& operator << (std::ostream& stream, const LargeInt& n);
	bool operator==(const LargeInt& rhs) const;
	bool operator>(const LargeInt& rhs) const;
	LargeInt operator-() const;
	LargeInt operator+(const LargeInt& rhs) const;
	LargeInt operator-(const LargeInt& rhs) const;	
	LargeInt operator*(const LargeInt& rhs) const;
	LargeInt operator/(const LargeInt& rhs) const;
	static LargeInt l_pow(const LargeInt& base, const LargeInt& exp);
};

int main() {
	LargeInt x("-11899980");
	LargeInt y(938239);
	std::string power = "17", base = "-9";
	LargeInt r;
	std::cout << x << " + " << y << " = " << x + y << std::endl;
	std::cout << x << " - " << y << " = " << x - y << std::endl;
	std::cout << x << " * " << y << " = " << x * y << std::endl;
	std::cout << x << " / " << y << " = " << x / y << std::endl;
	std::cout << "-(" << x << ") = " << -x << std::endl;
	std::cout << base << "^" << power << " = " << LargeInt::l_pow(base, power) << std::endl;
	return 0;
}

bool isValidNumberString(std::string str) {
	if (str[0] == '-')
		str.erase(0, 1);
	for (int i = 0; i < str.length(); i++) {
		if (!((int)str[i] >= '0' && (int)str[i] <= '9'))
			return false;
	}
	return true;
}

// << overload for printing out LargeInt number
std::ostream& operator << (std::ostream& stream, const LargeInt& n) {
	if (n.sign == LargeInt::Negative)
		stream << "-";
	stream << n.numberString;
	return stream;
}

//taking sign and trimming leading zeros
void LargeInt::normalize() {
	int i = 0;
	numberString[0] == '-' ? i++, sign = Negative : sign = Positive;
	numberString.erase(0, i);
	trimzeroforstring(numberString);
}

void LargeInt::trimzeroforstring(std::string& str) const {
	int k = 0;
	while (str[k] == '0')
		k++;
	str.erase(0, k);
	if (!str.length())
		str = "0";
}

      //comparison
//compare 2 positive a and b. return 1 if a>b, -1 if a<b and 0 if a=b
int LargeInt::compare(const std::string& a, const std::string& b) const {
	if (a.length() != b.length()) {
		return (a.length() > b.length()) ? 1 : -1;
	}

	for (int i = 0; i < a.length(); ++i) {
		if (a[i] > b[i]) {
			return 1;
		}
		else if (a[i] < b[i]) {
			return -1;
		}
	}
	return 0;
}

bool LargeInt::operator==(const LargeInt& rhs) const {
	if (numberString.length() != rhs.numberString.length() || sign != rhs.sign) {
		return false;
	}
	return compare(numberString, rhs.numberString) == 0;
}

bool LargeInt::operator>(const LargeInt& rhs) const {
	if (sign != rhs.sign) {
		return sign == Positive;
	}
	int comparison = compare(numberString, rhs.numberString);
	return comparison == 1 ? (sign == Positive) : (comparison == -1 ? (sign == Negative) : false);
}

//negation
void LargeInt::invertSign() {
	if (numberString != "0") {
		sign = (sign == Positive) ? Negative : Positive;
	}
}

LargeInt LargeInt::operator-() const {
	LargeInt result = *this;
	result.invertSign();
	return result;
}

//addition using add and carry method
std::string LargeInt::addHelper(std::string a, std::string b) const {
	std::string result;
	int carry = 0;
	int i = a.length() - 1;
	int j = b.length() - 1;
	while (i >= 0 || j >= 0 || carry) {
		int x = i >= 0 ? a[i] - '0' : 0;
		int y = j >= 0 ? b[j] - '0' : 0;
		int sum = x + y + carry;
		carry = sum / 10;
		result.insert(result.begin(), sum % 10 + '0');
		i--;
		j--;
	}
	trimzeroforstring(result);
	return result;
}

LargeInt LargeInt::operator+(const LargeInt& rhs) const {
	LargeInt result;
	if (this->numberString == "0") {
		result = rhs;
		return result;
	}
	if (rhs.numberString == "0") {
		result = *this;
		return result;
	}

	if (this->sign != rhs.sign) {
		result = (*this) - (-rhs);
		return result;
	}
	result.numberString = addHelper(this->numberString, rhs.numberString);
	result.sign = this->sign;
	return result;
}

//subtraction using naive algorithm. the subtractHelper() only works if a > b > 0.
std::string LargeInt::subtractHelper(std::string a, std::string b) const
{
	int i = a.size() - 1, j = b.size() - 1;
	int carry = 0;
	std::string result;

	while (i >= 0 || j >= 0 || carry) {
		int x = i >= 0 ? a[i] - '0' : 0;
		int y = j >= 0 ? b[j] - '0' : 0;
		int diff = x - y - carry;
		carry = diff < 0 ? 1 : 0;
		result.insert(result.begin(), (diff + 10) % 10 + '0');
		i--;
		j--;
	}
	trimzeroforstring(result);
	return result;
}

LargeInt LargeInt::operator-(const LargeInt& rhs) const {
	LargeInt result;
	if (*this == rhs) {
		return LargeInt("0");
	}
	if (this->numberString == "0") {
		result = -rhs;
		return result;
	}
	if (rhs.numberString == "0") {
		result = *this;
		return result;
	}

	if (this->sign != rhs.sign) {
		result.numberString = addHelper(this->numberString, rhs.numberString);
		result.sign = this->sign;
	}
	else if (this->sign == Positive && rhs.sign == Positive) {
		if (*this > rhs) {
			result.numberString = subtractHelper(this->numberString, rhs.numberString);
			result.sign = Positive;
		}
		else {
			result.numberString = subtractHelper(rhs.numberString, this->numberString);
			result.sign = Negative;
		}
	}
	else if (this->sign == Negative && rhs.sign == Negative) {
		if (*this > rhs) {
			result.numberString = subtractHelper(rhs.numberString, this->numberString);
			result.sign = Positive;
		}
		else {
			result.numberString = subtractHelper(this->numberString, rhs.numberString);
			result.sign = Negative;
		}
	}
	return result;
}

//helper functions for multiplication
//padding zeros to left or right of a string
std::string LargeInt::padString(const std::string& str, size_t n, PaddingType paddingType) const {
	std::string padding(n, '0');
	return paddingType == LEFT ? padding + str : str + padding;
}

// multiply 2 single digit string-numbers
std::string LargeInt::multiplyXY(const std::string& x, const std::string& y) const {
	int product = ((x[0] - '0') * (y[0] - '0'));
	return std::to_string(product);
}

//multiply using Karatsuba method
std::string LargeInt::multipleHelper(std::string lhs, std::string rhs) const {
	std::string x = lhs, y = rhs;
	if (x.length() == 1 && y.length() == 1) {
		return multiplyXY(x, y);
	}

	if (x.length() > y.length()) {
		y = padString(y, x.length() - y.length(), LEFT);
	}
	else {
		x = padString(x, y.length() - x.length(), LEFT);
	}
	if (x.length() % 2 != 0) {
		x = "0" + x;
		y = "0" + y;
	}

	std::string a, b, c, d, ac, bd, e;
	int n = x.length();

	a = x.substr(0, n / 2);
	b = x.substr(n / 2);
	c = y.substr(0, n / 2);
	d = y.substr(n / 2);

	ac = multipleHelper(a, c);
	bd = multipleHelper(b, d);
	e = subtractHelper(subtractHelper(multipleHelper(addHelper(a, b), addHelper(c, d)), ac), bd);

	return addHelper(addHelper(padString(ac, n, RIGHT), padString(e, n / 2, RIGHT)), bd);
}

LargeInt LargeInt::operator*(const LargeInt& rhs) const {
	if (numberString == "0" || rhs.numberString == "0") {
		return LargeInt("0");
	}

	LargeInt result;
	std::string x = numberString;
	std::string y = rhs.numberString;

	if (sign == rhs.sign) {
		result.sign = Positive;
	}
	else {
		result.sign = Negative;
	}

	result.numberString = multipleHelper(x, y);
	return result;
}

/* divide str1 / str2 by repeated subtraction method.Length difference of str1 and str2 is calculated
and then padString() is used to pad 0 to str2 (equivalent to multiply str2 to power of 10) */
std::string LargeInt::divideHelper(std::string str1, const std::string str2) const {
	std::string count = "0";
	while (compare(str1, str2) >= 0) {
		int lenDiff = str1.length() - str2.length();
		if (lenDiff > 0 && str1[0] > str2[0]) {
			count = addHelper(count, padString("1", lenDiff, RIGHT));
			str1 = subtractHelper(str1, padString(str2, lenDiff, RIGHT));
		}
		else if (lenDiff > 0) {
			count = addHelper(count, padString("1", lenDiff - 1, RIGHT));
			str1 = subtractHelper(str1, padString(str2, lenDiff - 1, RIGHT));
		}
		else {
			count = addHelper(count, "1");
			str1 = subtractHelper(str1, str2);
		}
	}
	trimzeroforstring(count);
	return count;
}

LargeInt LargeInt::operator/(const LargeInt& rhs) const{
	if (compare(numberString, rhs.numberString) < 0) {
		return LargeInt("0");
	}
	LargeInt res;
	res.numberString = divideHelper(numberString, rhs.numberString);
	res.sign = (sign == rhs.sign) ? Positive : Negative;
	return res;
}

//exponential using the squaring method
std::string LargeInt::powerHelper(const std::string& base, const std::string exp) const {
	if (exp == "1") {
		return base;
	}
	if (exp == "0") {
		return "1";
	}
	std::string p = powerHelper(base, (divideHelper(exp, "2")));
	std::string res;
	if (exp.back() % 2 == 0) {
		res = "1";
	}
	else {
		res = base;
	}
	std::string x = multipleHelper(p, p);
	return multipleHelper(x, res);
}

LargeInt LargeInt::l_pow(const LargeInt& base, const LargeInt& exp) {
	if (base.numberString == "0" && exp.sign == LargeInt::Negative) {
		throw std::invalid_argument("Base cannot be 0 if the exponent is negative");
	}
	if (exp.sign == LargeInt::Negative) {
		return LargeInt("0");
	}
	LargeInt result;
	result.numberString = result.powerHelper(base.numberString, exp.numberString);
	if (exp.numberString.back() % 2 == 0 || base.sign == LargeInt::Positive) {
		result.sign = LargeInt::Positive;
	}
	else {
		result.sign = LargeInt::Negative;
	}
	return result;
}




