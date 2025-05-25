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

int main() {
  const char *inputFile =
      "/home/zack/Downloads/20250525_Bartel_Lynda_timecard.xlsx";

  std::string outputFile = getCurrentDateString() + "_foo.xlsx";

  libxl::Book *book = xlCreateXMLBook();

  if (book->load(inputFile)) {
    libxl::Sheet *sheet = book->getSheet(0); // First sheet
    if (sheet) {
      int lastRow = sheet->lastRow();
      for (int row = 0; row < lastRow; ++row) {
        const char *val = sheet->readStr(row, 0); // Column A (0-based)
        if (val) {
          std::string newVal = std::string(val) + "_mod";
          sheet->writeStr(row, 0, newVal.c_str());
        }
      }
    }

    if (book->save(outputFile.c_str())) {
      std::cout << "Saved modified file as " << outputFile << "\n";
    } else {
      std::cerr << "Failed to save: " << book->errorMessage() << "\n";
    }
  } else {
    std::cerr << "Failed to load: " << book->errorMessage() << "\n";
  }

  book->release();
  return 0;
}
