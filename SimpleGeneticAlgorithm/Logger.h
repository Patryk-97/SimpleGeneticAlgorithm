#pragma once

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <fstream>

class Logger
{
public:
   Logger() = delete;
   Logger(const char* filename);
   ~Logger();
   void writeln(const std::string& text);
   void write(const std::string& text);

private:
   std::fstream file;
};

#endif