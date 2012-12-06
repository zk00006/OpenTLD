/* ----------------------------------------------------------------------------
   libconfig - A library for processing structured configuration files
   Copyright (C) 2005-2010  Mark A Lindner

   This file is part of libconfig.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, see
   <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------------
*/

#include "libconfig.h++"

#ifdef _MSC_VER
#pragma warning (disable: 4996)
#endif

#include "wincompat.h"
#include "libconfig.h"

#include <cstring>
#include <cstdlib>
#include <sstream>

namespace libconfig {

// ---------------------------------------------------------------------------

ParseException::ParseException(const char *file, int line, const char *error)
  : _file(file ? ::strdup(file) : NULL), _line(line), _error(error)
{
}

// ---------------------------------------------------------------------------

ParseException::ParseException(const ParseException &other)
  : _file(other._file ? ::strdup(other._file) : NULL),
    _line(other._line),
    _error(other._error)
{
}

// ---------------------------------------------------------------------------

ParseException::~ParseException() throw()
{
  ::free((void *)_file);
}

// ---------------------------------------------------------------------------

const char *ParseException::what() const throw()
{
  return("ParseException");
}

// ---------------------------------------------------------------------------

static int __toTypeCode(Setting::Type type)
{
  int typecode;

  switch(type)
  {
    case Setting::TypeGroup:
      typecode = CONFIG_TYPE_GROUP;
      break;

    case Setting::TypeInt:
      typecode = CONFIG_TYPE_INT;
      break;

    case Setting::TypeInt64:
      typecode = CONFIG_TYPE_INT64;
      break;

    case Setting::TypeFloat:
      typecode = CONFIG_TYPE_FLOAT;
      break;

    case Setting::TypeString:
      typecode = CONFIG_TYPE_STRING;
      break;

    case Setting::TypeBoolean:
      typecode = CONFIG_TYPE_BOOL;
      break;

    case Setting::TypeArray:
      typecode = CONFIG_TYPE_ARRAY;
      break;

    case Setting::TypeList:
      typecode = CONFIG_TYPE_LIST;
      break;

    default:
      typecode = CONFIG_TYPE_NONE;
  }

  return(typecode);
}

// ---------------------------------------------------------------------------

static void __constructPath(const Setting &setting,
                            std::stringstream &path)
{
  // head recursion to print path from root to target

  if(! setting.isRoot())
  {
    __constructPath(setting.getParent(), path);
    if(path.tellp() > 0)
      path << '.';

    const char *name = setting.getName();
    if(name)
      path << name;
    else
      path << '[' << setting.getIndex() << ']';
  }
}

// ---------------------------------------------------------------------------

SettingException::SettingException(const Setting &setting)
{
  std::stringstream sstr;
  __constructPath(setting, sstr);

  _path = ::strdup(sstr.str().c_str());
}

// ---------------------------------------------------------------------------

SettingException::SettingException(const Setting &setting, int idx)
{
  std::stringstream sstr;
  __constructPath(setting, sstr);
  sstr << ".[" << idx << "]";

  _path = ::strdup(sstr.str().c_str());
}

// ---------------------------------------------------------------------------

SettingException::SettingException(const Setting &setting, const char *name)
{
  std::stringstream sstr;
  __constructPath(setting, sstr);
  sstr << '.' << name;

  _path = ::strdup(sstr.str().c_str());
}

// ---------------------------------------------------------------------------

SettingException::SettingException(const char *path)
{
  _path = ::strdup(path);
}

// ---------------------------------------------------------------------------

const char *SettingException::getPath() const
{
  return(_path);
}

// ---------------------------------------------------------------------------

SettingException::SettingException(const SettingException &other)
{
  _path = ::strdup(other._path);
}

// ---------------------------------------------------------------------------

SettingException &SettingException::operator=(const SettingException &other)
{
  ::free(_path);
  _path = ::strdup(other._path);

  return(*this);
}

// ---------------------------------------------------------------------------

const char *SettingException::what() const throw()
{
  return("SettingException");
}

// ---------------------------------------------------------------------------

SettingException::~SettingException() throw()
{
  ::free(_path);
}

// ---------------------------------------------------------------------------

SettingTypeException::SettingTypeException(const Setting &setting)
  : SettingException(setting)
{
}

// ---------------------------------------------------------------------------

SettingTypeException::SettingTypeException(const Setting &setting, int idx)
  : SettingException(setting, idx)
{
}

// ---------------------------------------------------------------------------

SettingTypeException::SettingTypeException(const Setting &setting,
                                           const char *name)
  : SettingException(setting, name)
{
}

// ---------------------------------------------------------------------------

const char *SettingTypeException::what() const throw()
{
  return("SettingTypeException");
}

// ---------------------------------------------------------------------------

SettingNotFoundException::SettingNotFoundException(const Setting &setting,
                                                   int idx)
  : SettingException(setting, idx)
{
}

// ---------------------------------------------------------------------------

SettingNotFoundException::SettingNotFoundException(const Setting &setting,
                                                   const char *name)
  : SettingException(setting, name)
{
}

// ---------------------------------------------------------------------------

SettingNotFoundException::SettingNotFoundException(const char *path)
  : SettingException(path)
{
}

// ---------------------------------------------------------------------------

const char *SettingNotFoundException::what() const throw()
{
  return("SettingNotFoundException");
}

// ---------------------------------------------------------------------------

SettingNameException::SettingNameException(const Setting &setting,
                                           const char *name)
  : SettingException(setting, name)
{
}

// ---------------------------------------------------------------------------

const char *SettingNameException::what() const throw()
{
  return("SettingNameException");
}

// ---------------------------------------------------------------------------

const char *FileIOException::what() const throw()
{
  return("FileIOException");
}

// ---------------------------------------------------------------------------

void Config::ConfigDestructor(void *arg)
{
  delete reinterpret_cast<Setting *>(arg);
}

// ---------------------------------------------------------------------------

Config::Config()
  : _defaultFormat(Setting::FormatDefault)
{
  _config = new config_t;
  config_init(_config);
  config_set_destructor(_config, ConfigDestructor);
}

// ---------------------------------------------------------------------------

Config::~Config()
{
  config_destroy(_config);
  delete _config;
}

// ---------------------------------------------------------------------------

void Config::setAutoConvert(bool flag)
{
  config_set_auto_convert(_config, (flag ? CONFIG_TRUE : CONFIG_FALSE));
}

// ---------------------------------------------------------------------------

bool Config::getAutoConvert() const
{
  return(config_get_auto_convert(_config) != CONFIG_FALSE);
}

// ---------------------------------------------------------------------------

void Config::setDefaultFormat(Setting::Format format)
{
  if(format == Setting::FormatHex)
    _defaultFormat = Setting::FormatHex;
  else
    _defaultFormat = Setting::FormatDefault;

  config_set_default_format(_config, static_cast<short>(_defaultFormat));
}

// ---------------------------------------------------------------------------

void Config::setTabWidth(unsigned short width) throw()
{
  config_set_tab_width(_config, width);
}

// ---------------------------------------------------------------------------

unsigned short Config::getTabWidth() const throw()
{
  return(config_get_tab_width(_config));
}

// ---------------------------------------------------------------------------

void Config::setIncludeDir(const char *includeDir) throw()
{
  config_set_include_dir(_config, includeDir);
}

// ---------------------------------------------------------------------------

const char *Config::getIncludeDir() const throw()
{
  return(config_get_include_dir(_config));
}

// ---------------------------------------------------------------------------

void Config::handleError() const
{
  switch(config_error_type(_config))
  {
    case CONFIG_ERR_NONE:
      break;

    case CONFIG_ERR_PARSE:
      throw ParseException(config_error_file(_config),
                           config_error_line(_config),
                           config_error_text(_config));
      break;

    case CONFIG_ERR_FILE_IO:
    default:
      throw FileIOException();
  }
}

// ---------------------------------------------------------------------------

void Config::read(FILE *stream) throw(ParseException)
{
  if(! config_read(_config, stream))
    handleError();
}

// ---------------------------------------------------------------------------

void Config::readString(const char *str) throw(ParseException)
{
  if(! config_read_string(_config, str))
    handleError();
}

// ---------------------------------------------------------------------------

void Config::write(FILE *stream) const
{
  config_write(_config, stream);
}

// ---------------------------------------------------------------------------

void Config::readFile(const char *filename) throw(FileIOException,
                                                  ParseException)
{
  if(! config_read_file(_config, filename))
    handleError();
}

// ---------------------------------------------------------------------------

void Config::writeFile(const char *filename) throw(FileIOException)
{
  if(! config_write_file(_config, filename))
    handleError();
}

// ---------------------------------------------------------------------------

Setting & Config::lookup(const char *path) const
  throw(SettingNotFoundException)
{
  config_setting_t *s = config_lookup(_config, path);
  if(! s)
    throw SettingNotFoundException(path);

  return(Setting::wrapSetting(s));
}

// ---------------------------------------------------------------------------

bool Config::exists(const char *path) const throw()
{
  config_setting_t *s = config_lookup(_config, path);

  return(s != NULL);
}

// ---------------------------------------------------------------------------

#define CONFIG_LOOKUP_NO_EXCEPTIONS(P, T, V)    \
  try                                           \
  {                                             \
    Setting &s = lookup(P);                     \
    V = (T)s;                                   \
    return(true);                               \
  }                                             \
  catch(ConfigException)                        \
  {                                             \
    return(false);                              \
  }

// ---------------------------------------------------------------------------

bool Config::lookupValue(const char *path, bool &value) const throw()
{
  CONFIG_LOOKUP_NO_EXCEPTIONS(path, bool, value);
}

// ---------------------------------------------------------------------------

bool Config::lookupValue(const char *path, int &value) const throw()
{
  CONFIG_LOOKUP_NO_EXCEPTIONS(path, int, value);
}

// ---------------------------------------------------------------------------

bool Config::lookupValue(const char *path, unsigned int &value) const throw()
{
  CONFIG_LOOKUP_NO_EXCEPTIONS(path, unsigned int, value);
}

// ---------------------------------------------------------------------------

bool Config::lookupValue(const char *path, long long &value) const throw()
{
  CONFIG_LOOKUP_NO_EXCEPTIONS(path, long long, value);
}

// ---------------------------------------------------------------------------

bool Config::lookupValue(const char *path, unsigned long long &value)
  const throw()
{
  CONFIG_LOOKUP_NO_EXCEPTIONS(path, unsigned long long, value);
}

// ---------------------------------------------------------------------------

bool Config::lookupValue(const char *path, double &value) const throw()
{
  CONFIG_LOOKUP_NO_EXCEPTIONS(path, double, value);
}

// ---------------------------------------------------------------------------

bool Config::lookupValue(const char *path, float &value) const throw()
{
  CONFIG_LOOKUP_NO_EXCEPTIONS(path, float, value);
}

// ---------------------------------------------------------------------------

bool Config::lookupValue(const char *path, const char *&value) const throw()
{
  CONFIG_LOOKUP_NO_EXCEPTIONS(path, const char *, value);
}

// ---------------------------------------------------------------------------

bool Config::lookupValue(const char *path, std::string &value) const throw()
{
  CONFIG_LOOKUP_NO_EXCEPTIONS(path, const char *, value);
}

// ---------------------------------------------------------------------------

Setting & Config::getRoot() const
{
  return(Setting::wrapSetting(config_root_setting(_config)));
}

// ---------------------------------------------------------------------------

Setting::Setting(config_setting_t *setting)
  : _setting(setting)
{
  switch(config_setting_type(setting))
  {
    case CONFIG_TYPE_GROUP:
      _type = TypeGroup;
      break;

    case CONFIG_TYPE_INT:
      _type = TypeInt;
      break;

    case CONFIG_TYPE_INT64:
      _type = TypeInt64;
      break;

    case CONFIG_TYPE_FLOAT:
      _type = TypeFloat;
      break;

    case CONFIG_TYPE_STRING:
      _type = TypeString;
      break;

    case CONFIG_TYPE_BOOL:
      _type = TypeBoolean;
      break;

    case CONFIG_TYPE_ARRAY:
      _type = TypeArray;
      break;

    case CONFIG_TYPE_LIST:
      _type = TypeList;
      break;

    case CONFIG_TYPE_NONE:
    default:
      _type = TypeNone;
      break;
  }

  switch(config_setting_get_format(setting))
  {
    case CONFIG_FORMAT_HEX:
      _format = FormatHex;
      break;

    case CONFIG_FORMAT_DEFAULT:
    default:
      _format = FormatDefault;
      break;
  }
}

// ---------------------------------------------------------------------------

Setting::~Setting() throw()
{
  _setting = NULL;
}

// ---------------------------------------------------------------------------

void Setting::setFormat(Format format) throw()
{
  if((_type == TypeInt) || (_type == TypeInt64))
  {
    if(format == FormatHex)
      _format = FormatHex;
    else
      _format = FormatDefault;
  }
  else
    _format = FormatDefault;

  config_setting_set_format(_setting, static_cast<short>(_format));
}

// ---------------------------------------------------------------------------

Setting::operator bool() const throw(SettingTypeException) 
{
  assertType(TypeBoolean);

  return(config_setting_get_bool(_setting) ? true : false);
}

// ---------------------------------------------------------------------------

Setting::operator int() const throw(SettingTypeException)
{
  assertType(TypeInt);

  return(config_setting_get_int(_setting));
}

// ---------------------------------------------------------------------------

Setting::operator unsigned int() const throw(SettingTypeException)
{
  assertType(TypeInt);

  int v = config_setting_get_int(_setting);

  if(v < 0)
    v = 0;

  return(static_cast<unsigned int>(v));
}

// ---------------------------------------------------------------------------

Setting::operator long() const throw(SettingTypeException)
{
  if(sizeof(long) == sizeof(long long))
    return operator long long();
  else
    return operator int();
}

// ---------------------------------------------------------------------------

Setting::operator unsigned long() const throw(SettingTypeException)
{
  if(sizeof(long) == sizeof(long long))
    return operator unsigned long long();
  else
    return operator unsigned int();
}

// ---------------------------------------------------------------------------

Setting::operator long long() const throw(SettingTypeException)
{
  assertType(TypeInt64);

  return(config_setting_get_int64(_setting));
}

// ---------------------------------------------------------------------------

Setting::operator unsigned long long() const throw(SettingTypeException)
{
  assertType(TypeInt64);

  long long v = config_setting_get_int64(_setting);

  if(v < INT64_CONST(0))
    v = INT64_CONST(0);

  return(static_cast<unsigned long long>(v));
}

// ---------------------------------------------------------------------------

Setting::operator double() const throw(SettingTypeException)
{
  assertType(TypeFloat);

  return(config_setting_get_float(_setting));
}

// ---------------------------------------------------------------------------

Setting::operator float() const throw(SettingTypeException)
{
  assertType(TypeFloat);

  // may cause loss of precision:
  return(static_cast<float>(config_setting_get_float(_setting)));
}

// ---------------------------------------------------------------------------

Setting::operator const char *() const throw(SettingTypeException)
{
  assertType(TypeString);

  return(config_setting_get_string(_setting));
}

// ---------------------------------------------------------------------------

Setting::operator std::string() const throw(SettingTypeException)
{
  assertType(TypeString);

  const char *s = config_setting_get_string(_setting);

  std::string str;
  if(s)
    str = s;

  return(str);
}

// ---------------------------------------------------------------------------

Setting & Setting::operator=(bool value) throw(SettingTypeException)
{
  assertType(TypeBoolean);

  config_setting_set_bool(_setting, value);

  return(*this);
}

// ---------------------------------------------------------------------------

Setting & Setting::operator=(int value) throw(SettingTypeException)
{
  assertType(TypeInt);

  config_setting_set_int(_setting, value);

  return(*this);
}

// ---------------------------------------------------------------------------

Setting & Setting::operator=(long value) throw(SettingTypeException)
{
  if(sizeof(long) == sizeof(long long))
    return(operator=(static_cast<long long>(value)));
  else
    return(operator=(static_cast<int>(value)));
}

// ---------------------------------------------------------------------------

Setting & Setting::operator=(const long long &value)
  throw(SettingTypeException)
{
  assertType(TypeInt64);

  config_setting_set_int64(_setting, value);

  return(*this);
}

// ---------------------------------------------------------------------------

Setting & Setting::operator=(const double &value) throw(SettingTypeException)
{
  assertType(TypeFloat);

  config_setting_set_float(_setting, value);

  return(*this);
}

// ---------------------------------------------------------------------------

Setting & Setting::operator=(float value) throw(SettingTypeException)
{
  assertType(TypeFloat);

  double cvalue = static_cast<double>(value);

  config_setting_set_float(_setting, cvalue);

  return(*this);
}

// ---------------------------------------------------------------------------

Setting & Setting::operator=(const char *value) throw(SettingTypeException)
{
  assertType(TypeString);

  config_setting_set_string(_setting, value);

  return(*this);
}

// ---------------------------------------------------------------------------

Setting & Setting::operator=(const std::string &value)
  throw(SettingTypeException)
{
  assertType(TypeString);

  config_setting_set_string(_setting, value.c_str());

  return(*this);
}

// ---------------------------------------------------------------------------

Setting & Setting::operator[](int i) const
  throw(SettingTypeException, SettingNotFoundException)
{
  if((_type != TypeArray) && (_type != TypeGroup) && (_type != TypeList))
    throw SettingTypeException(*this, i);

  config_setting_t *setting = config_setting_get_elem(_setting, i);

  if(! setting)
    throw SettingNotFoundException(*this, i);

  return(wrapSetting(setting));
}

// ---------------------------------------------------------------------------

Setting & Setting::operator[](const char *key) const
  throw(SettingTypeException, SettingNotFoundException)
{
  assertType(TypeGroup);

  config_setting_t *setting = config_setting_get_member(_setting, key);

  if(! setting)
    throw SettingNotFoundException(*this, key);

  return(wrapSetting(setting));
}

// ---------------------------------------------------------------------------

#define SETTING_LOOKUP_NO_EXCEPTIONS(K, T, V)   \
  try                                           \
  {                                             \
    Setting &s = operator[](K);                 \
    V = (T)s;                                   \
    return(true);                               \
  }                                             \
  catch(ConfigException)                        \
  {                                             \
    return(false);                              \
  }

// ---------------------------------------------------------------------------

bool Setting::lookupValue(const char *name, bool &value) const throw()
{
  SETTING_LOOKUP_NO_EXCEPTIONS(name, bool, value);
}

// ---------------------------------------------------------------------------

bool Setting::lookupValue(const char *name, int &value) const throw()
{
  SETTING_LOOKUP_NO_EXCEPTIONS(name, int, value);
}

// ---------------------------------------------------------------------------

bool Setting::lookupValue(const char *name, unsigned int &value)
  const throw()
{
  SETTING_LOOKUP_NO_EXCEPTIONS(name, unsigned int, value);
}

// ---------------------------------------------------------------------------

bool Setting::lookupValue(const char *name, long long &value) const throw()
{
  SETTING_LOOKUP_NO_EXCEPTIONS(name, long long, value);
}

// ---------------------------------------------------------------------------

bool Setting::lookupValue(const char *name, unsigned long long &value)
  const throw()
{
  SETTING_LOOKUP_NO_EXCEPTIONS(name, unsigned long long, value);
}

// ---------------------------------------------------------------------------

bool Setting::lookupValue(const char *name, double &value) const throw()
{
  SETTING_LOOKUP_NO_EXCEPTIONS(name, double, value);
}

// ---------------------------------------------------------------------------

bool Setting::lookupValue(const char *name, float &value) const throw()
{
  SETTING_LOOKUP_NO_EXCEPTIONS(name, float, value);
}

// ---------------------------------------------------------------------------

bool Setting::lookupValue(const char *name, const char *&value) const throw()
{
  SETTING_LOOKUP_NO_EXCEPTIONS(name, const char *, value);
}

// ---------------------------------------------------------------------------

bool Setting::lookupValue(const char *name, std::string &value) const throw()
{
  SETTING_LOOKUP_NO_EXCEPTIONS(name, const char *, value);
}

// ---------------------------------------------------------------------------

bool Setting::exists(const char *name) const throw()
{
  if(_type != TypeGroup)
    return(false);

  config_setting_t *setting = config_setting_get_member(_setting, name);

  return(setting != NULL);
}

// ---------------------------------------------------------------------------

int Setting::getLength() const throw()
{
  return(config_setting_length(_setting));
}

// ---------------------------------------------------------------------------

const char * Setting::getName() const throw()
{
  return(config_setting_name(_setting));
}

// ---------------------------------------------------------------------------

std::string Setting::getPath() const
{
  std::stringstream path;

  __constructPath(*this, path);

  return(path.str());
}

// ---------------------------------------------------------------------------

const Setting & Setting::getParent() const throw(SettingNotFoundException)
{
  config_setting_t *setting = config_setting_parent(_setting);

  if(! setting)
    throw SettingNotFoundException(NULL);

  return(wrapSetting(setting));
}

// ---------------------------------------------------------------------------

Setting & Setting::getParent() throw(SettingNotFoundException)
{
  config_setting_t *setting = config_setting_parent(_setting);

  if(! setting)
    throw SettingNotFoundException(NULL);

  return(wrapSetting(setting));
}

// ---------------------------------------------------------------------------

unsigned int Setting::getSourceLine() const throw()
{
  return(config_setting_source_line(_setting));
}

// ---------------------------------------------------------------------------

const char *Setting::getSourceFile() const throw()
{
  return(config_setting_source_file(_setting));
}

// ---------------------------------------------------------------------------

bool Setting::isRoot() const throw()
{
  return(config_setting_is_root(_setting));
}

// ---------------------------------------------------------------------------

int Setting::getIndex() const throw()
{
  return(config_setting_index(_setting));
}

// ---------------------------------------------------------------------------

void Setting::remove(const char *name)
  throw(SettingTypeException, SettingNotFoundException)
{
  assertType(TypeGroup);

  if(! config_setting_remove(_setting, name))
    throw SettingNotFoundException(*this, name);
}

// ---------------------------------------------------------------------------

void Setting::remove(unsigned int idx)
  throw(SettingTypeException, SettingNotFoundException)
{
  if((_type != TypeArray) && (_type != TypeGroup) && (_type != TypeList))
    throw SettingTypeException(*this, idx);

  if(! config_setting_remove_elem(_setting, idx))
    throw SettingNotFoundException(*this, idx);
}

// ---------------------------------------------------------------------------

Setting & Setting::add(const char *name, Setting::Type type)
  throw(SettingNameException, SettingTypeException)
{
  assertType(TypeGroup);

  int typecode = __toTypeCode(type);

  if(typecode == CONFIG_TYPE_NONE)
    throw SettingTypeException(*this, name);

  config_setting_t *setting = config_setting_add(_setting, name, typecode);

  if(! setting)
    throw SettingNameException(*this, name);

  return(wrapSetting(setting));
}

// ---------------------------------------------------------------------------

Setting & Setting::add(Setting::Type type) throw(SettingTypeException)
{
  if((_type != TypeArray) && (_type != TypeList))
    throw SettingTypeException(*this);

  if(_type == TypeArray)
  {
    int idx = getLength();

    if(idx > 0)
    {
      Setting::Type atype = operator[](0).getType();
      if(type != atype)
        throw SettingTypeException(*this, idx);
    }
    else
    {
      if((type != TypeInt) && (type != TypeInt64) && (type != TypeFloat)
         && (type != TypeString) && (type != TypeBoolean))
        throw SettingTypeException(*this, idx);
    }
  }

  int typecode = __toTypeCode(type);
  config_setting_t *s = config_setting_add(_setting, NULL, typecode);

  Setting &ns = wrapSetting(s);

  switch(type)
  {
    case TypeInt:
      ns = 0;
      break;

    case TypeInt64:
      ns = INT64_CONST(0);
      break;

    case TypeFloat:
      ns = 0.0;
      break;

    case TypeString:
      ns = (char *)NULL;
      break;

    case TypeBoolean:
      ns = false;
      break;

    default:
      // won't happen
      break;
  }

  return(ns);
}

// ---------------------------------------------------------------------------

void Setting::assertType(Setting::Type type) const throw(SettingTypeException)
{
  if(type != _type)
  {
    if(!(isNumber() && config_get_auto_convert(_setting->config)
         && ((type == TypeInt) || (type == TypeInt64) || (type == TypeFloat))))
      throw SettingTypeException(*this);
  }
}

// ---------------------------------------------------------------------------

Setting & Setting::wrapSetting(config_setting_t *s)
{
  Setting *setting = NULL;

  void *hook = config_setting_get_hook(s);
  if(! hook)
  {
    setting = new Setting(s);
    config_setting_set_hook(s, reinterpret_cast<void *>(setting));
  }
  else
    setting = reinterpret_cast<Setting *>(hook);

  return(*setting);
}

// ---------------------------------------------------------------------------

}; // namespace libconfig

// eof
