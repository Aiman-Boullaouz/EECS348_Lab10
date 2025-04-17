#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>  // For reverse() and other algorithms

using namespace std;  // So we don't have to type std:: everywhere

// Structure to hold broken-down parts of a number
struct NumberParts {
    char sign;        // '+' or '-'
    string integer;   // Whole number part (before decimal)
    string fractional;// Decimal part (after decimal)
    int scale;        // Number of digits after decimal
};

// Check if a string represents a valid double number
bool is_valid_double(const string& s) {
    // Empty string isn't valid
    if (s.empty()) return false;

    size_t pos = 0;  // Start at beginning of string

    // Check for optional sign (+ or -)
    if (s[pos] == '+' || s[pos] == '-') {
        pos++;  // Move past the sign
        // If sign was last character, invalid
        if (pos >= s.size()) return false;
    }

    // Check for required digits (integer part)
    size_t digits_a = 0;
    while (pos < s.size() && isdigit(s[pos])) {
        digits_a++;
        pos++;
    }
    // Must have at least one digit in integer part
    if (digits_a == 0) return false;

    // Check for decimal point
    if (pos < s.size() && s[pos] == '.') {
        pos++;  // Move past the decimal
        size_t digits_b = 0;
        // Check for required digits after decimal
        while (pos < s.size() && isdigit(s[pos])) {
            digits_b++;
            pos++;
        }
        // If no digits after decimal, invalid
        if (digits_b == 0) return false;
    }

    // We must have processed entire string to be valid
    return pos == s.size();
}

// Break a valid number string into its components
NumberParts parse_number(const string& s) {
    NumberParts parts;
    size_t pos = 0;

    // Extract sign if present
    if (s[pos] == '+' || s[pos] == '-') {
        parts.sign = s[pos];
        pos++;
    } else {
        parts.sign = '+';  // Default to positive
    }

    // Extract integer part (everything until decimal)
    size_t integer_start = pos;
    while (pos < s.size() && s[pos] != '.') {
        pos++;
    }
    parts.integer = s.substr(integer_start, pos - integer_start);

    // Extract fractional part if decimal exists
    if (pos < s.size() && s[pos] == '.') {
        pos++;  // Skip decimal point
        size_t fractional_start = pos;
        // Take remaining digits as fractional part
        while (pos < s.size()) {
            pos++;
        }
        parts.fractional = s.substr(fractional_start);
        parts.scale = parts.fractional.size();
    } else {
        // No fractional part
        parts.fractional = "";
        parts.scale = 0;
    }

    return parts;
}

// Compare two absolute value strings (without signs)
int compare_absolute(const string& a, const string& b) {
    // Longer number is bigger
    if (a.length() > b.length()) return 1;
    if (a.length() < b.length()) return -1;

    // Same length: compare digit by digit
    for (size_t i = 0; i < a.length(); ++i) {
        if (a[i] > b[i]) return 1;
        if (a[i] < b[i]) return -1;
    }

    return 0;  // Numbers are equal
}

// Add two absolute value strings (both positive)
string add_absolute(string a, string b) {
    // Make sure a is longer for easier processing
    if (a.length() < b.length()) {
        swap(a, b);
    }

    string result;
    int carry = 0;  // Carry-over between digits
    int i = a.length() - 1;  // Start from least significant digit
    int j = b.length() - 1;

    // Process all digits and any remaining carry
    while (i >= 0 || j >= 0 || carry > 0) {
        // Get current digits (0 if beyond string length)
        int digit_a = (i >= 0) ? (a[i--] - '0') : 0;
        int digit_b = (j >= 0) ? (b[j--] - '0') : 0;

        // Calculate sum and carry
        int sum = digit_a + digit_b + carry;
        carry = sum / 10;  // Carry for next digit
        result.push_back(sum % 10 + '0');  // Store current digit
    }

    // We built the number backwards, reverse to correct
    reverse(result.begin(), result.end());

    // Remove leading zeros (but leave at least one zero)
    size_t first_non_zero = result.find_first_not_of('0');
    if (first_non_zero != string::npos) {
        result = result.substr(first_non_zero);
    } else {
        result = "0";  // All zeros case
    }

    return result;
}

// Subtract absolute value strings (a - b, both positive)
string subtract_absolute(string a, string b) {
    string result;
    bool negative = false;

    // If b > a, swap and mark result as negative
    if (compare_absolute(a, b) < 0) {
        negative = true;
        swap(a, b);
    }

    int borrow = 0;  // Borrow flag
    int i = a.length() - 1;
    int j = b.length() - 1;

    while (i >= 0 || j >= 0) {
        // Get current digits (0 if beyond string length)
        int digit_a = (i >= 0) ? (a[i--] - '0') : 0;
        int digit_b = (j >= 0) ? (b[j--] - '0') : 0;

        // Apply any previous borrow
        digit_a -= borrow;
        borrow = 0;

        // Handle need to borrow
        if (digit_a < digit_b) {
            digit_a += 10;
            borrow = 1;
        }

        result.push_back(digit_a - digit_b + '0');
    }

    // Reverse digits to correct order
    reverse(result.begin(), result.end());

    // Remove leading zeros
    size_t first_non_zero = result.find_first_not_of('0');
    if (first_non_zero != string::npos) {
        result = result.substr(first_non_zero);
    } else {
        result = "0";  // All zeros case
    }

    // Add negative sign if needed
    if (negative) {
        result = "-" + result;
    }

    return result;
}

// Add two numbers with signs considered
string add_signed_numbers(const string& a, const string& b) {
    // Split first number into sign and absolute value
    char sign_a = '+';
    string abs_a;
    if (!a.empty() && a[0] == '-') {
        sign_a = '-';
        abs_a = a.substr(1);
    } else {
        abs_a = a;
    }

    // Split second number into sign and absolute value
    char sign_b = '+';
    string abs_b;
    if (!b.empty() && b[0] == '-') {
        sign_b = '-';
        abs_b = b.substr(1);
    } else {
        abs_b = b;
    }

    // Handle different sign combinations
    if (sign_a == '+' && sign_b == '+') {
        // Both positive: simple addition
        return add_absolute(abs_a, abs_b);
    } else if (sign_a == '+' && sign_b == '-') {
        // Positive + Negative = Subtraction
        return subtract_absolute(abs_a, abs_b);
    } else if (sign_a == '-' && sign_b == '+') {
        // Negative + Positive = Subtraction (swap order)
        return subtract_absolute(abs_b, abs_a);
    } else {
        // Both negative: add and keep negative sign
        return "-" + add_absolute(abs_a, abs_b);
    }
}

// Format the sum string with proper decimal placement
string format_result(const string& sum_str, int max_scale) {
    string sign = "";
    string absolute_sum_str = sum_str;

    // Handle negative results
    if (!sum_str.empty() && sum_str[0] == '-') {
        sign = "-";
        absolute_sum_str = sum_str.substr(1);
    }

    // Special case for zero
    if (absolute_sum_str == "0") {
        string result = sign + "0";
        // Add .000... if needed
        if (max_scale > 0) {
            result += "." + string(max_scale, '0');
        }
        return result;
    }

    // No decimal needed case
    if (max_scale == 0) {
        return sign + absolute_sum_str;
    }

    int sum_len = absolute_sum_str.length();
    string formatted;

    // When sum is smaller than scale (needs leading zeros)
    if (sum_len <= max_scale) {
        string fractional = string(max_scale - sum_len, '0') + absolute_sum_str;
        formatted = sign + "0." + fractional;
    } else {
        // Split into integer and fractional parts
        string integer_part = absolute_sum_str.substr(0, sum_len - max_scale);
        string fractional_part = absolute_sum_str.substr(sum_len - max_scale);
        formatted = sign + integer_part + "." + fractional_part;
    }

    return formatted;
}

// Read numbers from file into a vector
vector<string> load_numbers(const string& filename) {
    vector<string> numbers;
    ifstream file(filename);
    string line;
    
    // Read each line and add to vector
    while (getline(file, line)) {
        numbers.push_back(line);
    }
    return numbers;
}

int main() {
    string filename;
    cout << "Enter filename: ";
    cin >> filename;

    // Read candidate numbers from file
    vector<string> candidates = load_numbers(filename);

    // Prepare the fixed number -123.456 as NumberParts
    NumberParts b_parts;
    b_parts.sign = '-';
    b_parts.integer = "123";
    b_parts.fractional = "456";
    b_parts.scale = 3;

    // Process each candidate number
    for (const string& candidate : candidates) {
        // Skip invalid numbers
        if (!is_valid_double(candidate)) {
            cout << candidate << " is invalid" << endl;
            continue;
        }

        // Parse candidate number
        NumberParts a_parts = parse_number(candidate);
        
        // Determine maximum decimal places needed
        int max_scale = max(a_parts.scale, b_parts.scale);

        // Process candidate number (a)
        // Remove leading zeros from integer part
        string stripped_integer_a = a_parts.integer;
        stripped_integer_a.erase(0, stripped_integer_a.find_first_not_of('0'));
        if (stripped_integer_a.empty()) stripped_integer_a = "0";
        
        // Pad fractional part with zeros if needed
        string padded_fractional_a = a_parts.fractional;
        if (max_scale > a_parts.scale)
            padded_fractional_a += string(max_scale - a_parts.scale, '0');
        
        // Combine integer and fractional parts
        string combined_a_str = stripped_integer_a + padded_fractional_a;
        combined_a_str.erase(0, combined_a_str.find_first_not_of('0'));
        if (combined_a_str.empty()) combined_a_str = "0";
        // Add sign back
        string signed_a_str = (a_parts.sign == '-') ? "-" + combined_a_str : combined_a_str;

        // Process fixed number (b) similarly
        string stripped_integer_b = b_parts.integer;
        stripped_integer_b.erase(0, stripped_integer_b.find_first_not_of('0'));
        if (stripped_integer_b.empty()) stripped_integer_b = "0";
        
        string padded_fractional_b = b_parts.fractional;
        if (max_scale > b_parts.scale)
            padded_fractional_b += string(max_scale - b_parts.scale, '0');
        
        string combined_b_str = stripped_integer_b + padded_fractional_b;
        combined_b_str.erase(0, combined_b_str.find_first_not_of('0'));
        if (combined_b_str.empty()) combined_b_str = "0";
        string signed_b_str = (b_parts.sign == '-') ? "-" + combined_b_str : combined_b_str;

        // Perform the addition
        string sum_str = add_signed_numbers(signed_a_str, signed_b_str);
        
        // Format the final result
        string result = format_result(sum_str, max_scale);

        // Output the result
        cout << result << endl;
    }

    return 0;
}