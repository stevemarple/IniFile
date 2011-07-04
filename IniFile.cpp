#include <IniFile.h>

IniFile::IniFile(void)
{
  _mode = FILE_READ;
  _isOpen = false;
}

IniFile::IniFile(SDClass& SD, char* filename, uint8_t mode)
{
  _mode = mode;
  _file = SD.open(filename, mode);
  _isOpen = (_file ? true : false); 
}

IniFile::~IniFile()
{
  //if (_isOpen)
  //  _file.close();
}

boolean IniFile::open(SDClass& SD, char* filename, uint8_t mode)
{
  close();
  _mode = mode;
  _file = SD.open(filename, mode);
  _isOpen = (_file ? true : false); 
  return _isOpen;
}

void IniFile::close(void)
{
  if (_isOpen)
    _file.close();
  _isOpen = false;
}

boolean IniFile::isOpen(void) const
{
  return _isOpen;
}

uint8_t IniFile::mode(void) const
{
  return _mode;
}

boolean IniFile::hasSection(const char* section)
{
  if (!isOpen())
    return false;
  
  _file.seek(0); // rewind
  return findSection(section);
}

// Return zero or positive values if key is found, if not the negative
// value indicates why
int IniFile::getValue(char* buffer, int len, const char* key,
		      const char* section) const
{
  if (!isOpen())
    return fileNotOpen;

  _file.seek(0); // rewind
  if (section != NULL && findSection(section) == false)
    return sectionNotFound;

  if (findKey(key) == false)
    return keyNotFound;

  skipWhiteSpace(); // skip leading WS
  int i = 0;
  int lastNonWS = -1;
  while (_file.available()) {
    char c = _file.read();
    if (c == '\r' || c == '\n')
      break; // end of line

    if (!isspace(c))
      lastNonWS = i; // keep track of last non-whitespace char found

    if (i < len)
      buffer[i] = c; // copy if buffer large enough
    ++i;
  }

  if (len) {
    // terminate the string (only if buffer has space!)
    if (lastNonWS + 1 > len)
      // value is longer than the buffer
      buffer[len-1] = '\0';
    else
      buffer[lastNonWS + 1] = '\0';
  }
  return lastNonWS + 1; // return the length of the string *found*
}

extern void IniFile::skipWhiteSpace(void) const
{
  while (_file.available() && isspace(_file.peek()))
    _file.read();
}

void IniFile::skipComments(void) const
{
  char c;
  while (_file.available() && ((c = _file.peek()) == '#' || c == ';'))
    skipToEndOfLine();
}

void IniFile::skipToEndOfLine(void) const
{
  char c;
  while (_file.available() && (c = _file.peek()) != '\r' && c != '\n') 
    _file.read();
  
  while (_file.available() && ((c = _file.peek()) == '\r' || c == '\n')) 
    _file.read();
}
  
// Assumes starting from start of line
boolean IniFile::findSection(const char* section) const
{
  int c;
  if (section == NULL)
    return false;
  
  while (_file.available()) {
    skipComments();
    skipWhiteSpace();

    c = _file.read();
    if (c == '[') {
      // At start of a section
      const char* s = section;
      while (c != -1 && *s != '\0' && *s == _file.read())
	++s;
      if (*s == '\0' && _file.read() == ']') {
	// found the section
	skipToEndOfLine();
	return true;
      }
    }

    skipToEndOfLine();
  }
  return false;
}

// Assumes starting from start of line
boolean IniFile::findKey(const char* key) const
{
  int c;

  if (key == NULL || *key == '\0') {
    return false;
  }

  while (_file.available()) {
    skipComments();
    skipWhiteSpace();

    const char* k = key;
    //while (_file.available() && *k != '\0' && *k == _file.read())
    //       ++k;
    while (_file.available() && *k != '\0' &&
	   ((c = _file.read()) == *k  || (isspace(*k) && isspace(c)))) {
      ++k;
      if (isspace(c)) {
	skipWhiteSpace(); // skip any extra whitespace in the file
	while (isspace(*k)) // skip any extra whitespace in key 
	  ++k;
      }
    }
        
    if (*k == '\0' && (isspace(c = _file.peek()) || c == '=')) {
      skipWhiteSpace();
      if (_file.available() && _file.read() != '=') {
	// supplied key is only a partial match to the key in the file
	skipToEndOfLine();
      }
      return true; // still have any leading WS in front of value
    }
    skipToEndOfLine();
  }
  return false;
}


// For true accept: true, yes, 1
// For false accept: false, no, 0
boolean IniFile::getValue(boolean& val, const char* key, \
			  const char* section) const
{
  const int len = 16;
  char buffer[len];
  int i = getValue(buffer, len, key, section);

  if (i <= 0 || i >= len) {
    // error or length incorrect
    return false;
  }
  
  if (strcasecmp(buffer, "true") == 0 ||
      strcasecmp(buffer, "yes") == 0 ||
      strcasecmp(buffer, "1") == 0) {
    val = true;
    return true;
  }
  if (strcasecmp(buffer, "false") == 0 ||
      strcasecmp(buffer, "no") == 0 ||
      strcasecmp(buffer, "0") == 0) {
    val = false;
    return true;
  }
  return false; // does not match any known strings      
}

boolean IniFile::getValue(int& val, const char* key, \
			  const char* section) const
{
  const int len = 16;
  char buffer[len];
  int i = getValue(buffer, len, key, section);
  if (i <= 0 || i >= len)
    return false; // error or length incorrect
  
  val = atoi(buffer);
  return true;
}

boolean IniFile::getValue(uint16_t& val, const char* key,	\
			  const char* section) const
{
  long longval;
  boolean r = getValue(longval, key, section);
  val = uint16_t(longval);
  return r;
}

boolean IniFile::getValue(long& val, const char* key, \
			  const char* section) const
{
  const int len = 16;
  char buffer[len];
  int i = getValue(buffer, len, key, section);
  long origVal = val;
  if (i <= 0 || i >= len)     
    return false; // error or length incorrect
  
  val = atol(buffer);
  return true;
}

boolean IniFile::getIPAddress(IPAddress& ip, const char* key, \
			      const char* section) const
{
  const int len = 16; // enough for 4 * 3 digits, 3 dots and a null character
  char buffer[len];
  int i = getValue(buffer, len, key, section);

  ip = IPAddress(0, 0, 0, 0);
  if (i <= 0 || i >= len) {
    // error or length incorrect
    return false;
  }

  i = 0;
  char* cp = buffer;
  while (*cp != '\0' && i < 4) {
    if (*cp == '.') {
      ++i;
      ++cp;
      continue;
    }
    if (isdigit(*cp)) {
      ip[i] *= 10;
      ip[i] += (*cp - '0');
    }
    else {
      ip = IPAddress(0, 0, 0, 0);
      return false;
    }
    ++cp;
  }
  return true;
}

boolean IniFile::getMACAddress(uint8_t mac[6], const char* key, \
			   const char* section) const
{
  const int len = 18; // enough for 6 * 2 hex digits, 5 : or - and a null char
  char buffer[len];

  memset(mac, 0, 6);
  int i = getValue(buffer, len, key, section);
  if (i <= 0 || i >= len) {
    // error or length incorrect
    return false;
  }

  i = 0;
  char* cp = buffer;
  while (*cp != '\0' && i < 6) {
    if (*cp == ':' || *cp == '-') {
      ++i;
      ++cp;
      continue;
    }
    if (isdigit(*cp)) {
      mac[i] *= 16; // working in hex!
      mac[i] += (*cp - '0');
    }
    else {
      if (isxdigit(*cp)) {
	mac[i] *= 16; // working in hex!
	mac[i] += (toupper(*cp) - 55); // convert A to 0xA, F to 0xF
      }
      else {
	memset(mac, 0, sizeof(mac));
	return false;
      }
    }
    ++cp;
  }
  return true;
}

