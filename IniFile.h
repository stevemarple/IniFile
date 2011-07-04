#ifndef _INIFILE_H
#define _INIFILE_H

#include "SD.h"
#include "Ethernet.h"

class IniFile {
public:
  enum returnValues {
    keyNotFound = -1,
    sectionNotFound = -2,
    fileNotOpen = -3,
    fileReadOnly = -4
  };

  IniFile(void);
  IniFile(SDClass& SD, char* filename, uint8_t mode = FILE_READ);
  ~IniFile();

  boolean open(SDClass& SD, char* filename, uint8_t mode = FILE_READ);
  void close(void);
  boolean isOpen(void) const;
  uint8_t mode(void) const;

  boolean hasSection(const char* section);
  //boolean hasKey(const char* key, const char* section = NULL);

  int getValue(const char* section, const char* key, \
	       char* buffer, int len) const; 
  boolean getValue(const char* section, const char* key, boolean& b) const;
  boolean getValue(const char* section, const char* key, int& val) const;
  boolean getValue(const char* section, const char* key, uint16_t& val) const;
  boolean getValue(const char* section, const char* key, long& val) const;
  
  boolean getIPAddress(const char* section, const char* key, IPAddress& ip) const;
  boolean getMACAddress(const char* section, const char* key, uint8_t mac[6]) const;
  
protected:
  void skipWhiteSpace(void) const;
  void skipComments(void) const;
  void skipToEndOfLine(void) const;
  boolean findSection(const char* section) const;
  boolean findKey(const char* key) const;

private:
  uint8_t _mode;
  boolean _isOpen;
  mutable File _file;
};

  
#endif
