#include <map>
#include <string>
#include <utility>
#include <vector>

// Example: m:1pm-5pm,t:11:15am-3pm,w:off,th:9am-12pm,f:1pm-6pm,sa:9am-12pm,su:1:30pm-5:30pm

using TimeRange = std::pair<int, int>; // minutes since midnight
using Schedule = std::map<std::string, std::vector<TimeRange>>;

int parseTimeString(const std::string& timeStr);

Schedule parseSchedule(const std::string& input);

std::string minutesToTimeString(int minutes);
