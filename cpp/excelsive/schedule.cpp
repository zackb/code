#include "schedule.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>

int parseTimeString(const std::string& timeStr) {
    std::regex timePattern(R"((\d{1,2})(?::(\d{1,2}))?(am|pm))", std::regex::icase);
    std::smatch match;

    if (!std::regex_match(timeStr, match, timePattern)) {
        throw std::runtime_error("Invalid time format: " + timeStr);
    }

    int hour = std::stoi(match[1]);
    int minute = match[2].matched ? std::stoi(match[2]) : 0;
    std::string meridian = match[3].str();

    std::transform(meridian.begin(), meridian.end(), meridian.begin(), ::tolower);

    if (hour == 12)
        hour = 0;
    if (meridian == "pm")
        hour += 12;

    return hour * 60 + minute;
}

Schedule parseSchedule(const std::string& input) {
    Schedule schedule;

    std::stringstream ss(input);
    std::string dayEntry;

    while (std::getline(ss, dayEntry, ',')) {
        auto colonPos = dayEntry.find(':');
        if (colonPos == std::string::npos)
            continue;

        std::string day = dayEntry.substr(0, colonPos);
        std::string timesStr = dayEntry.substr(colonPos + 1);

        std::transform(day.begin(), day.end(), day.begin(), ::tolower);
        if (timesStr == "off") {
            schedule[day] = {};
            continue;
        }

        std::stringstream rangeStream(timesStr);
        std::string range;
        while (std::getline(rangeStream, range, '+')) { // support multiple shifts like 9am-12pm+1pm-5pm
            auto dashPos = range.find('-');
            if (dashPos == std::string::npos)
                continue;

            std::string startStr = range.substr(0, dashPos);
            std::string endStr = range.substr(dashPos + 1);

            int start = parseTimeString(startStr);
            int end = parseTimeString(endStr);

            if (end <= start)
                throw std::runtime_error("End time must be after start time");

            schedule[day].emplace_back(start, end);
        }
    }

    return schedule;
}

std::string minutesToTimeString(int minutes) {
    int hour = minutes / 60;
    int minute = minutes % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hour << ":" << std::setw(2) << std::setfill('0') << minute;
    return oss.str();
}
