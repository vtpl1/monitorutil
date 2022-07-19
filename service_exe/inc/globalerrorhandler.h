// *****************************************************
//	Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef globalerrorhandler_h
#define globalerrorhandler_h
#include <Poco/ErrorHandler.h>
#include <Poco/Exception.h>

namespace vtpl
{
class globalerrorhandler : public Poco::ErrorHandler
{
public:
  void exception(const Poco::Exception& exc);
  void exception(const std::exception& exc);
  void exception();
};
} // namespace vtpl

#endif // globalerrorhandler_h
