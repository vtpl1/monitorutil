// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Util/ServerApplication.h>

#include "globalerrorhandler.h"
#include "logging.h"
namespace vtpl
{
void globalerrorhandler::exception(const Poco::Exception& exc)
{
  RAY_LOG_ERR << "Critical error " << exc.displayText().data();
  // Poco::Util::ServerApplication::terminate();
}
void globalerrorhandler::exception(const std::exception& exc)
{
  RAY_LOG_ERR << "Critical error " << exc.what();
  // Poco::Util::ServerApplication::terminate();
}
void globalerrorhandler::exception()
{
  RAY_LOG_ERR << "Critical error no details";
  // Poco::Util::ServerApplication::terminate();
}
} // namespace vtpl
