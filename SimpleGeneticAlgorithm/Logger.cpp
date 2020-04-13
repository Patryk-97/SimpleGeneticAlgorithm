#include "Logger.h"

Logger::Logger(const char* filename)
{
   this->file.open(filename, std::ios::out);
}

Logger::~Logger()
{
   this->file.close();
}

void Logger::writeln(const std::string& text)
{
   if (true == this->file.good())
   {
      this->file << text << "\n";
   }
}

void Logger::write(const std::string& text)
{
   if (true == this->file.good())
   {
      this->file << text;
   }
}