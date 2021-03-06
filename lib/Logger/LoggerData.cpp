////////////////////////////////////////////////////////////////////////////////
/// @brief logger info
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2004-2013 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
/// @author Achim Brandt
/// @author Copyright 2007-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "LoggerInfo.h"

using namespace triagens::basics;
using namespace triagens::basics::LoggerData;
using namespace std;

// -----------------------------------------------------------------------------
// --SECTION--                                           static public variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Logging
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief the application name
////////////////////////////////////////////////////////////////////////////////

ApplicationName Info::_applicationName;

////////////////////////////////////////////////////////////////////////////////
/// @brief the facility name
////////////////////////////////////////////////////////////////////////////////

Facility Info::_facility;

////////////////////////////////////////////////////////////////////////////////
/// @brief the hostname
////////////////////////////////////////////////////////////////////////////////

HostName Info::_hostName;

////////////////////////////////////////////////////////////////////////////////
/// @brief the extra attribute
////////////////////////////////////////////////////////////////////////////////

size_t const Extra::npos = (size_t) -1;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Logging
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief the application name
////////////////////////////////////////////////////////////////////////////////

ApplicationName::ApplicationName ()
  : _name() {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief the facility name
////////////////////////////////////////////////////////////////////////////////

Facility::Facility ()
  : _name() {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief the hostname
////////////////////////////////////////////////////////////////////////////////

HostName::HostName ()
  : _name() {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief the message identifier, automatically generated
////////////////////////////////////////////////////////////////////////////////

MessageIdentifier::MessageIdentifier ()
  : _name() {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief the proc and thread identifiers, automatically generated
////////////////////////////////////////////////////////////////////////////////

ProcessIdentifier::ProcessIdentifier ()
  : _process(Thread::currentProcessId()),
    _threadProcess(Thread::currentThreadProcessId()),
    _thread(Thread::currentThreadId()) {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief the fucntional name
////////////////////////////////////////////////////////////////////////////////

Functional::Functional (string const& name)
  : _name(name) {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief the peg name
////////////////////////////////////////////////////////////////////////////////

Peg::Peg (string const& name)
  : _name(name) {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief the task name
////////////////////////////////////////////////////////////////////////////////

Task::Task (string const& name)
  : _name(name) {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief position information
////////////////////////////////////////////////////////////////////////////////

Position::Position (string const& function, string const& file, int line)
  : _function(function), _file(file), _line(line) {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief the measure
////////////////////////////////////////////////////////////////////////////////

Measure::Measure (double measure, unit_e unit)
  : _value(measure), _unit(unit) {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief the extra attribute
////////////////////////////////////////////////////////////////////////////////

Extra::Extra (size_t pos, string const& name)
  : _position(pos), _name(name) {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief the extra attribute
////////////////////////////////////////////////////////////////////////////////

Extra::Extra (string const& name)
  : _position(npos), _name(name) {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief the user identifier
////////////////////////////////////////////////////////////////////////////////

UserIdentifier::UserIdentifier (string const& name)
  : _user(name) {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief the info block
////////////////////////////////////////////////////////////////////////////////

Info::Info ()
  : _messageIdentifier(),
    _level(TRI_LOG_LEVEL_FATAL),
    _category(TRI_LOG_CATEGORY_WARNING),
    _severity(TRI_LOG_SEVERITY_UNKNOWN),
    _functional(),
    _peg(),
    _task(),
    _position(),
    _measure(),
    _extras(),
    _userIdentifier(),
    _processIdentifier(),
    _prefix() {
}

Info::Info (const Info& originalInfo) {
  _messageIdentifier._name = originalInfo._messageIdentifier._name;
  _level                   = originalInfo._level;
  _category                = originalInfo._category;
  _severity                = originalInfo._severity;
  _functional._name        = originalInfo._functional._name;
  _peg._name               = originalInfo._peg._name;
  _task._name              = originalInfo._task._name;
  _position._file          = originalInfo._position._file;
  _position._function      = originalInfo._position._function;
  _position._line          = originalInfo._position._line;
  _measure._unit           = originalInfo._measure._unit;
  _measure._value          = originalInfo._measure._value;
  _extras.clear();
  for (vector<Extra>::const_iterator i = originalInfo._extras.begin(); i != originalInfo._extras.end(); ++i) {
    _extras.push_back(Extra((*i)._position, (*i)._name));
  }
  _userIdentifier._user    = originalInfo._userIdentifier._user;
  _prefix                  = originalInfo._prefix;
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
