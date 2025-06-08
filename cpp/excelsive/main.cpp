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

// HH:MH
libxl::Format* timeFormat;

// 6.5
libxl::Format* numberFormat;

// MM/DD
libxl::Format* dateFormat;

void writeShift(
    libxl::Book* book, libxl::Sheet* sheet, Schedule& schedule, std::string cday, std::chrono::sys_days sday, int row) {
    auto [year, month, day] = to_ymd(sday);

    double serial = book->datePack(year, month, day, 0, 0, 0);
    sheet->writeNum(row, 2, serial, dateFormat);

    const auto& shifts = schedule[cday];
    if (!shifts.empty()) {
        const TimeRange& r = shifts[0];
        int startHour = r.first / 60;
        int startMin = r.first % 60;

        int endHour = r.second / 60;
        int endMin = r.second % 60;

        double startSerial = book->datePack(year, month, day, startHour, startMin, 0);
        double endSerial = book->datePack(year, month, day, endHour, endMin, 0);

        sheet->writeNum(row, 3, startSerial, timeFormat);
        sheet->writeNum(row, 6, endSerial, timeFormat);

        int durationMinutes = r.second - r.first;
        double durationHours = durationMinutes / 60.0;
        sheet->writeNum(row, 7, durationHours); //, numberFormat);
        sheet->writeStr(row, 9, "Pool Monitor");
    } else {
        sheet->writeStr(row, 3, "");
        sheet->writeStr(row, 6, "");
        sheet->writeNum(row, 7, 0.0);
        sheet->writeStr(row, 9, "");
    }
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

        // HH:MH
        timeFormat = book->addFormat();
        timeFormat->setNumFormat(libxl::NUMFORMAT_CUSTOM_HMM_AM);

        // 6.5
        numberFormat = book->addFormat();
        numberFormat->setNumFormat(libxl::NUMFORMAT_GENERAL);

        // MM/DD
        dateFormat = sheet->cellFormat(16, 2); // grab monday's format

        // Get today's date from system_clock
        using namespace std::chrono;
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
        writeShift(book, sheet, schedule, "m", monday, 16);

        // Tuesday
        sys_days tuesday = monday + days{1};
        writeShift(book, sheet, schedule, "t", tuesday, 17);

        // Wednesday
        sys_days wednesday = tuesday + days{1};
        writeShift(book, sheet, schedule, "w", wednesday, 18);

        // Thursday
        sys_days thursday = wednesday + days{1};
        writeShift(book, sheet, schedule, "th", thursday, 19);

        // Friday
        sys_days friday = thursday + days{1};
        writeShift(book, sheet, schedule, "f", friday, 20);

        // Saturday
        sys_days saturday = friday + days{1};
        writeShift(book, sheet, schedule, "sa", saturday, 21);

        // Sunday
        sys_days sunday = saturday + days{1};
        writeShift(book, sheet, schedule, "su", sunday, 22);

        // total up hours worked
        int totalMinutes = 0;
        for (const auto& [day, ranges] : schedule) {
            if (!ranges.empty()) {
                for (const auto& [start, end] : ranges) {
                    totalMinutes += end - start;
                }
            }
        }
        double totalHours = totalMinutes / 60.0;
        sheet->writeNum(23, 7, totalHours);
        sheet->writeStr(26, 8, timeToWords(totalHours).c_str());
        std::cout << "Total: " << timeToWords(totalHours) << std::endl;

        // Sunday's date for the signature line
        auto [year, month, day] = to_ymd(sunday);
        double serial = book->datePack(year, month, day, 0, 0, 0);
        libxl::Format* sigFormat = sheet->cellFormat(28, 5);
        sheet->writeNum(28, 5, serial, sigFormat);

        // and on the manager's signature
        sheet->writeNum(28, 12, serial, sigFormat);

        // and on the thing
        sheet->writeNum(7, 8, serial, sigFormat);

        // write a copy of our book to the new date stamped excel file
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
    writeSchedule(sched);
    return 0;
};
