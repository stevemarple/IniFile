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

  int getValue(char* buffer, int len, const char* key, \
	       const char* section = NULL) const; 
  boolean getValue(boolean& b, const char* key,	const char* section) const;
  boolean getValue(int& val, const char* key, const char* section) const;
  boolean getValue(uint16_t& val, const char* key, const char* section) const;
  boolean getValue(long& val, const char* key,const char* section) const;
  
  boolean getIPAddress(IPAddress& ip, const char* key, \
		   const char* section = NULL) const;
  boolean getMACAddress(uint8_t mac[6], const char* key, \
		    const char* section = NULL) const;
  
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
