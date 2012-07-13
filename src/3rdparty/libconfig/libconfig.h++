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

#ifndef __libconfig_hpp
#define __libconfig_hpp

#include <stdio.h>
#include <exception>
#include <string>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#if defined(LIBCONFIGXX_STATIC)
#define LIBCONFIGXX_API
#elif defined(LIBCONFIGXX_EXPORTS)
#define LIBCONFIGXX_API __declspec(dllexport)
#else /* ! LIBCONFIGXX_EXPORTS */
#define LIBCONFIGXX_API __declspec(dllimport)
#endif /* LIBCONFIGXX_STATIC */
#else /* ! WIN32 */
#define LIBCONFIGXX_API
#endif /* WIN32 */

#define LIBCONFIGXX_VER_MAJOR    1
#define LIBCONFIGXX_VER_MINOR    4
#define LIBCONFIGXX_VER_REVISION 8

struct config_t; // fwd decl
struct config_setting_t; // fwd decl

namespace libconfig {

class LIBCONFIGXX_API ConfigException : public std::exception { };

class Setting; // fwd decl

class LIBCONFIGXX_API SettingException : public ConfigException
{
  friend class Config;

  public:

  SettingException(const SettingException &other);
  SettingException& operator=(const SettingException &other);

  virtual ~SettingException() throw();

  const char *getPath() const;

  virtual const char *what() const throw();

  protected:

  SettingException(const Setting &setting);
  SettingException(const Setting &setting, int idx);
  SettingException(const Setting &setting, const char *name);
  SettingException(const char *path);

  private:

  char *_path;
};

class LIBCONFIGXX_API SettingTypeException : public SettingException
{
  friend class Config;
  friend class Setting;

  public:

  const char *what() const throw();

  private:

  SettingTypeException(const Setting &setting);
  SettingTypeException(const Setting &setting, int idx);
  SettingTypeException(const Setting &setting, const char *name);
};

class LIBCONFIGXX_API SettingNotFoundException : public SettingException
{
  friend class Config;
  friend class Setting;

  public:

  const char *what() const throw();

  private:

  SettingNotFoundException(const Setting &setting, int idx);
  SettingNotFoundException(const Setting &setting, const char *name);
  SettingNotFoundException(const char *path);
};

class LIBCONFIGXX_API SettingNameException : public SettingException
{
  friend class Config;
  friend class Setting;

  public:

  const char *what() const throw();

  private:

  SettingNameException(const Setting &setting, const char *name);
};

class LIBCONFIGXX_API FileIOException : public ConfigException
{
  public:

  const char *what() const throw();
};

class LIBCONFIGXX_API ParseException : public ConfigException
{
  friend class Config;

  public:

  ParseException(const ParseException &other);

  virtual ~ParseException() throw();

  inline const char *getFile() const throw()
  { return(_file); }

  inline int getLine() const throw()
  { return(_line); }

  inline const char *getError() const throw()
  { return(_error); }

  const char *what() const throw();

  private:

  ParseException(const char *file, int line, const char *error);

  const char *_file;
  int _line;
  const char *_error;
};

class LIBCONFIGXX_API Setting
{
  friend class Config;

  public:

  enum Type
  {
    TypeNone = 0,
    // scalar types
    TypeInt,
    TypeInt64,
    TypeFloat,
    TypeString,
    TypeBoolean,
    // aggregate types
    TypeGroup,
    TypeArray,
    TypeList
  };

  enum Format
  {
    FormatDefault = 0,
    FormatHex = 1
  };

  private:

  config_setting_t *_setting;
  Type _type;
  Format _format;

  Setting(config_setting_t *setting);

  void assertType(Type type) const
    throw(SettingTypeException);
  static Setting & wrapSetting(config_setting_t *setting);

  Setting(const Setting& other); // not supported
  Setting& operator=(const Setting& other); // not supported

  public:

  virtual ~Setting() throw();

  inline Type getType() const throw() { return(_type); }

  inline Format getFormat() const throw() { return(_format); }
  void setFormat(Format format) throw();

  operator bool() const throw(SettingTypeException);
  operator int() const throw(SettingTypeException);
  operator unsigned int() const throw(SettingTypeException);
  operator long() const throw(SettingTypeException);
  operator unsigned long() const throw(SettingTypeException);
  operator long long() const throw(SettingTypeException);
  operator unsigned long long() const throw(SettingTypeException);
  operator double() const throw(SettingTypeException);
  operator float() const throw(SettingTypeException);
  operator const char *() const throw(SettingTypeException);
  operator std::string() const throw(SettingTypeException);

  inline const char *c_str() const throw(SettingTypeException)
  { return operator const char*(); }

  Setting & operator=(bool value) throw(SettingTypeException);
  Setting & operator=(int value) throw(SettingTypeException);
  Setting & operator=(long value) throw(SettingTypeException);
  Setting & operator=(const long long &value) throw(SettingTypeException);
  Setting & operator=(const double &value) throw(SettingTypeException);
  Setting & operator=(float value) throw(SettingTypeException);
  Setting & operator=(const char *value) throw(SettingTypeException);
  Setting & operator=(const std::string &value) throw(SettingTypeException);

  Setting & operator[](const char *key) const
    throw(SettingTypeException, SettingNotFoundException);

  inline Setting & operator[](const std::string &key) const
    throw(SettingTypeException, SettingNotFoundException)
  { return(operator[](key.c_str())); }

  Setting & operator[](int index) const
    throw(SettingTypeException, SettingNotFoundException);

  bool lookupValue(const char *name, bool &value) const throw();
  bool lookupValue(const char *name, int &value) const throw();
  bool lookupValue(const char *name, unsigned int &value) const throw();
  bool lookupValue(const char *name, long long &value) const throw();
  bool lookupValue(const char *name, unsigned long long &value)
    const throw();
  bool lookupValue(const char *name, double &value) const throw();
  bool lookupValue(const char *name, float &value) const throw();
  bool lookupValue(const char *name, const char *&value) const throw();
  bool lookupValue(const char *name, std::string &value) const throw();

  inline bool lookupValue(const std::string &name, bool &value)
    const throw()
  { return(lookupValue(name.c_str(), value)); }

  inline bool lookupValue(const std::string &name, int &value)
    const throw()
  { return(lookupValue(name.c_str(), value)); }

  inline bool lookupValue(const std::string &name, unsigned int &value)
    const throw()
  { return(lookupValue(name.c_str(), value)); }

  inline bool lookupValue(const std::string &name, long long &value)
    const throw()
  { return(lookupValue(name.c_str(), value)); }

  inline bool lookupValue(const std::string &name,
                          unsigned long long &value) const throw()
  { return(lookupValue(name.c_str(), value)); }

  inline bool lookupValue(const std::string &name, double &value) const
    throw()
  { return(lookupValue(name.c_str(), value)); }

  inline bool lookupValue(const std::string &name, float &value) const
    throw()
  { return(lookupValue(name.c_str(), value)); }

  inline bool lookupValue(const std::string &name, const char *&value) const
    throw()
  { return(lookupValue(name.c_str(), value)); }

  inline bool lookupValue(const std::string &name, std::string &value) const
    throw()
  { return(lookupValue(name.c_str(), value)); }

  void remove(const char *name)
    throw(SettingTypeException, SettingNotFoundException);

  inline void remove(const std::string & name)
    throw(SettingTypeException, SettingNotFoundException)
  { remove(name.c_str()); }

  void remove(unsigned int idx)
    throw(SettingTypeException, SettingNotFoundException);

  inline Setting & add(const std::string & name, Type type)
    throw(SettingNameException, SettingTypeException)
  { return(add(name.c_str(), type)); }

  Setting & add(const char *name, Type type)
    throw(SettingNameException, SettingTypeException);

  Setting & add(Type type) throw(SettingTypeException);

  inline bool exists(const std::string &name) const throw()
  { return(exists(name.c_str())); }

  bool exists(const char *name) const throw();

  int getLength() const throw();
  const char *getName() const throw();
  std::string getPath() const;
  int getIndex() const throw();

  const Setting & getParent() const throw(SettingNotFoundException);
  Setting & getParent() throw(SettingNotFoundException);

  bool isRoot() const throw();

  inline bool isGroup() const throw()
  { return(_type == TypeGroup); }

  inline bool isArray() const throw()
  { return(_type == TypeArray); }

  inline bool isList() const throw()
  { return(_type == TypeList); }

  inline bool isAggregate() const throw()
  { return(_type >= TypeGroup); }

  inline bool isScalar() const throw()
  { return((_type > TypeNone) && (_type < TypeGroup)); }

  inline bool isNumber() const throw()
  { return((_type == TypeInt) || (_type == TypeInt64)
           || (_type == TypeFloat)); }

  unsigned int getSourceLine() const throw();
  const char *getSourceFile() const throw();
};

class LIBCONFIGXX_API Config
{
  private:

  config_t *_config;

  static void ConfigDestructor(void *arg);
  Config(const Config& other); // not supported
  Config& operator=(const Config& other); // not supported

  public:

  Config();
  virtual ~Config();

  void setAutoConvert(bool flag);
  bool getAutoConvert() const;

  void setDefaultFormat(Setting::Format format);
  inline Setting::Format getDefaultFormat() const
  { return(_defaultFormat); }

  void setTabWidth(unsigned short width) throw();
  unsigned short getTabWidth() const throw();

  void setIncludeDir(const char *includeDir) throw();
  const char *getIncludeDir() const throw();

  void read(FILE *stream) throw(ParseException);
  void write(FILE *stream) const;

  void readString(const char *str) throw(ParseException);
  inline void readString(const std::string &str) throw(ParseException)
  { return(readString(str.c_str())); }

  void readFile(const char *filename) throw(FileIOException, ParseException);
  void writeFile(const char *filename) throw(FileIOException);

  inline Setting & lookup(const std::string &path) const
    throw(SettingNotFoundException)
  { return(lookup(path.c_str())); }

  Setting & lookup(const char *path) const throw(SettingNotFoundException);

  inline bool exists(const std::string & path) const throw()
  { return(exists(path.c_str())); }

  bool exists(const char *path) const throw();

  bool lookupValue(const char *path, bool &value) const throw();
  bool lookupValue(const char *path, int &value) const throw();
  bool lookupValue(const char *path, unsigned int &value) const throw();
  bool lookupValue(const char *path, long long &value) const throw();
  bool lookupValue(const char *path, unsigned long long &value)
    const throw();
  bool lookupValue(const char *path, double &value) const throw();
  bool lookupValue(const char *path, float &value) const throw();
  bool lookupValue(const char *path, const char *&value) const throw();
  bool lookupValue(const char *path, std::string &value) const throw();

  inline bool lookupValue(const std::string &path, bool &value) const throw()
  { return(lookupValue(path.c_str(), value)); }

  inline bool lookupValue(const std::string &path, int &value) const throw()
  { return(lookupValue(path.c_str(), value)); }

  inline bool lookupValue(const std::string &path, unsigned int &value)
    const throw()
  { return(lookupValue(path.c_str(), value)); }

  inline bool lookupValue(const std::string &path, long long &value)
    const throw()
  { return(lookupValue(path.c_str(), value)); }

  inline bool lookupValue(const std::string &path,
                          unsigned long long &value) const throw()
  { return(lookupValue(path.c_str(), value)); }

  inline bool lookupValue(const std::string &path, double &value)
    const throw()
  { return(lookupValue(path.c_str(), value)); }

  inline bool lookupValue(const std::string &path, float &value)
    const throw()
  { return(lookupValue(path.c_str(), value)); }

  inline bool lookupValue(const std::string &path, const char *&value)
    const throw()
  { return(lookupValue(path.c_str(), value)); }

  inline bool lookupValue(const std::string &path, std::string &value)
    const throw()
  { return(lookupValue(path.c_str(), value)); }

  Setting & getRoot() const;

  private:

  Setting::Format _defaultFormat;

  void handleError() const;
};

} // namespace libconfig

#endif // __libconfig_hpp
