#include "schedule.h"
#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <libxl.h>
#include <sstream>

std::string getCurrentDateString() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
    localtime_r(&t, &tm);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d");
    return oss.str();
}

// Convert `std::chrono::sys_days` to (year, month, day)
std::tuple<int, int, int> to_ymd(std::chrono::sys_days date) {
    std::chrono::year_month_day ymd = std::chrono::year_month_day{date};
    return {int(ymd.year()), unsigned(ymd.month()), unsigned(ymd.day())};
}

// example: m:1pm-5pm,t:11:15am-3pm,w:off,th:9am-12pm,f:1pm-6pm
int writeSchedule(Schedule& schedule) {
    const char* inputFile = "/home/zack/Downloads/20250525_Bartel_Lynda_timecard.xlsx";

    std::string outputFile = getCurrentDateString() + "_Bartel_Lynda_timecard.xlsx";

    libxl::Book* book = xlCreateXMLBook();

    if (book->load(inputFile)) {
        libxl::Sheet* sheet = book->getSheet(0); // First sheet
        if (!sheet) {
            std::cerr << "No such sheet\n";
            return 1;
        }

        if (false) {
            for (int i = 0; i < 200; i++) {
                for (int j = 0; j < 200; j++) {
                    const char* str = sheet->readStr(i, j);
                    if (str) {
                        std::cout << "Row: " << i << " Col: " << j << std::endl;
                        std::cout << str << std::endl;
                    }
                }
            }
        }

        using namespace std::chrono;

        // Get today's date from system_clock
        sys_days today = floor<days>(system_clock::now());
        year_month_day today_ymd = year_month_day{today};
        weekday today_wd = weekday{today};

        // Compute days to next Sunday (0 = Sunday)
        days days_until_sunday = weekday{Sunday} - today_wd;

        sys_days next_sunday = today + days_until_sunday;
        std::cout << "Today is: " << today_ymd << "\n";
        std::cout << "Next Sunday is: " << next_sunday << "\n";

        // Monday
        sys_days monday = next_sunday - days{6};
        auto [year, month, day] = to_ymd(monday);
        libxl::Format* dateFormat = sheet->cellFormat(16, 2);
        double serial = book->datePack(year, month, day, 0, 0, 0);
        sheet->writeNum(16, 2, serial, dateFormat);

        // Tuesday
        sys_days tuesday = monday + days{1};
        std::tie(year, month, day) = to_ymd(tuesday);
        serial = book->datePack(year, month, day, 0, 0, 0);
        sheet->writeNum(17, 2, serial, dateFormat);

        // Wednesday
        sys_days wednesday = tuesday + days{1};
        std::tie(year, month, day) = to_ymd(wednesday);
        serial = book->datePack(year, month, day, 0, 0, 0);
        sheet->writeNum(18, 2, serial, dateFormat);

        // Thursday
        sys_days thursday = wednesday + days{1};
        std::tie(year, month, day) = to_ymd(thursday);
        serial = book->datePack(year, month, day, 0, 0, 0);
        sheet->writeNum(19, 2, serial, dateFormat);

        // Friday
        sys_days friday = thursday + days{1};
        std::tie(year, month, day) = to_ymd(friday);
        serial = book->datePack(year, month, day, 0, 0, 0);
        sheet->writeNum(20, 2, serial, dateFormat);

        // Saturday
        sys_days saturday = friday + days{1};
        std::tie(year, month, day) = to_ymd(saturday);
        serial = book->datePack(year, month, day, 0, 0, 0);
        sheet->writeNum(21, 2, serial, dateFormat);

        // Sunday
        sys_days sunday = saturday + days{1};
        std::tie(year, month, day) = to_ymd(sunday);
        serial = book->datePack(year, month, day, 0, 0, 0);
        sheet->writeNum(22, 2, serial, dateFormat);

        if (book->save(outputFile.c_str())) {
            std::cout << "Saved modified file as " << outputFile << "\n";
        } else {
            std::cerr << "Failed to save: " << book->errorMessage() << "\n";
            return 1;
        }
    } else {
        std::cerr << "Failed to load: " << book->errorMessage() << "\n";
        return 1;
    }

    book->release();
    return 0;
}

std::string ones[] = {"", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

std::string teens[] = {
    "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen"};

std::string tens[] = {"", "", "twenty", "thirty", "forty", "fifty"};

// Converts 0–59 to words
std::string numberToWords(int n) {
    if (n == 0)
        return "zero";
    if (n < 10)
        return ones[n];
    if (n < 20)
        return teens[n - 10];
    if (n < 60) {
        std::string result = tens[n / 10];
        if (n % 10 > 0)
            result += " " + ones[n % 10];
        return result;
    }
    return std::to_string(n); // fallback
}

// Capitalize first letter
std::string capitalize(const std::string& s) {
    if (s.empty())
        return s;
    std::string result = s;
    result[0] = std::toupper(result[0]);
    return result;
}

std::string timeToWords(double hoursDecimal) {
    int hours = static_cast<int>(hoursDecimal);
    int minutes = static_cast<int>(std::round((hoursDecimal - hours) * 60));

    std::string hourPart = numberToWords(hours) + " hour" + (hours != 1 ? "s" : "");
    std::string minutePart = numberToWords(minutes) + " minute" + (minutes != 1 ? "s" : "");

    return capitalize(hourPart + " and " + minutePart);
}

int usage(char* argv[]) {
    std::cout << "Usage: " << argv[0] << " <schedule> " << std::endl;
    std::cout << "Example: " << argv[0] << " m:1pm-5pm,t:11:15am-3pm,w:off,th:9am-12pm,f:1pm-6pm" << std::endl;

    return 1;
}

int main(int argc, char* argv[]) {
    if (argc < 2)
        return usage(argv);

    Schedule sched = parseSchedule(argv[1]);

    for (const auto& [day, ranges] : sched) {
        std::cout << day << ": ";
        if (ranges.empty()) {
            std::cout << "off";
        } else {
            for (const auto& [start, end] : ranges) {
                std::cout << "[" << minutesToTimeString(start) << "-" << minutesToTimeString(end) << "] ";
            }
        }
        std::cout << '\n';
    }
    return 0;
};
