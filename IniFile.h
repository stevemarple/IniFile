#ifndef _INIFILE_H
#define _INIFILE_H

// Maximum length for filename, excluding NULL char 26 chars allows an
// 8.3 filename instead and 8.3 directory with a leading slash
#define INI_FILE_MAX_FILENAME_LEN 26

#include "SD.h"
#include "Ethernet.h"

class IniFileState;

class IniFile {
public:
  enum returnValues {
    errorFileNotOpen = -1,
    errorBufferTooShort = -2,
    errorSeekError = -3,
    errorSectionNotFound = -4,
    errorKeyNotFound = -5,
  };

  static const uint8_t maxFilenameLen;

  // Create an IniFile object. It isn't opened until open() is called on it.
  IniFile(const char* filename, uint8_t mode = FILE_READ);
  ~IniFile();

  boolean open(void);
  void close(void);

  boolean isOpen(void) const;
  uint8_t getMode(void) const;
  const char* getFilename(void) const;
  
  boolean hasSection(const char* section);
  int getValue(const char* section, const char* key, \
	       char* buffer, int len) const; 
  boolean getValue(const char* section, const char* key, boolean& b) const;
  boolean getValue(const char* section, const char* key, int& val) const;
  boolean getValue(const char* section, const char* key, uint16_t& val) const;
  boolean getValue(const char* section, const char* key, long& val) const;
  
  boolean getIPAddress(const char* section, const char* key, IPAddress& ip) const;
  boolean getMACAddress(const char* section, const char* key, uint8_t mac[6]) const;

  // Utility function to read a line from a file, make available to all
  static int readLine(File &file, char *buffer, int len, uint32_t &pos);
  static boolean isCommentChar(char c);
  static char* skipWhiteSpace(char* str);
  static void removeTrailingWhiteSpace(char* str);
  
protected:
  void skipWhiteSpace(void) const;
  void skipComments(void) const;
  void skipToEndOfLine(void) const;
  boolean findSection(const char* section) const;
  int findSection(const char* section, char* buffer, int len,	\
		  IniFileState &state);

  boolean findKey(const char* key, const char* section) const;

private:
  char _filename[INI_FILE_MAX_FILENAME_LEN];
  uint8_t _mode;
  //boolean _isOpen;
  mutable File _file;
};

class IniFileState {
public:
  IniFileState();

private:
  enum {funcUnset = 0,
	funcFindSection,
	funcFindKey,
  };
	
  uint32_t readLinePosition;
  uint8_t getValueState;

  friend class IniFile;
};

  
#endif

