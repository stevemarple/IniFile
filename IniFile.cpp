#include <IniFile.h>

#include <string.h>

const uint8_t IniFile::maxFilenameLen = INI_FILE_MAX_FILENAME_LEN;

IniFile::IniFile(const char* filename, uint8_t mode)
{
  if (strlen(filename) <= maxFilenameLen)
    strcpy(_filename, filename);
  _mode = mode;
}

IniFile::~IniFile()
{
  //if (_file)
  //  _file.close();
}

boolean IniFile::open(void)
{
  if (!_file) 
    _file = SD.open(_filename, _mode);
  return isOpen();
}

void IniFile::close(void)
{
  if (_file)
    _file.close();
}

boolean IniFile::isOpen(void) const
{
  return (_file == true);
}

uint8_t IniFile::getMode(void) const
{
  return _mode;
}

const char* IniFile::getFilename(void) const
{
  return _filename;
}

boolean IniFile::validate(char* buffer, int len) const
{
  uint32_t pos = 0;
  int8_t done;
  while ((done = readLine(_file, buffer, len, pos)) == 0)
    ;
  return (done == 1 ? true : false);
}

int8_t IniFile::getValue(const char* section, const char* key,
			 char* buffer, int len, IniFileState &state) const
{
  int8_t done = 0;
  if (!_file)
    return errorFileNotOpen;
  
  switch (state.getValueState) {
  case IniFileState::funcUnset:
    state.getValueState = (section == NULL ? IniFileState::funcFindKey
			   : IniFileState::funcFindSection);
    state.readLinePosition = 0;
    break;
    
  case IniFileState::funcFindSection:
    if ((done = findSection(section, buffer, len, state)) == 1) {
      done = 0;
      state.getValueState = IniFileState::funcFindKey;
    }
    break;
    
  case IniFileState::funcFindKey:
    char *cp;
    if ((done = findKey(section, key, buffer, len, &cp, state)) == 1) {
      // Found key line in correct section
      cp = skipWhiteSpace(cp);
      removeTrailingWhiteSpace(cp);

      // Copy from cp to buffer, but the strings overlap so strcpy is out
      while (*cp != '\0')
	*buffer++ = *cp++;
      *buffer = '\0';
    }
    break;
    
  default:
    // How did this happen?
    done = errorUnknownError;
    break;
  }
  
  return done;
}

int8_t IniFile::getValue(const char* section, const char* key,
			 char* buffer, int len) const
{
  IniFileState state;
  int8_t done;
  while ((done = getValue(section, key, buffer, len, state)) == 0)
    ;
  return done;
}

// For true accept: true, yes, 1
 // For false accept: false, no, 0
boolean IniFile::getValue(const char* section, const char* key, 
			  char* buffer, int len, boolean& val) const
{
  if (getValue(section, key, buffer, len) < 0)
    return false; // error
  
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

boolean IniFile::getValue(const char* section, const char* key,
			  char* buffer, int len, int& val) const
{
  if (getValue(section, key, buffer, len) < 0)
    return false; // error
  
  val = atoi(buffer);
  return true;
}

boolean IniFile::getValue(const char* section, const char* key,	\
			  char* buffer, int len, uint16_t& val) const
{
  long longval;
  boolean r = getValue(section, key, buffer, len, longval);
  if (r)
    val = uint16_t(longval);
  return r;
}

boolean IniFile::getValue(const char* section, const char* key,
			  char* buffer, int len, long& val) const
{
  if (getValue(section, key, buffer, len) < 0)
    return false; // error
  
  val = atol(buffer);
  return true;
}

boolean IniFile::getIPAddress(const char* section, const char* key,
			      char* buffer, int len, uint8_t* ip) const
{
  // Need 16 chars minimum: 4 * 3 digits, 3 dots and a null character
  if (len < 16)
    return false;

  if (getValue(section, key, buffer, len) < 0) 
    return false; // error

  int i = 0;
  char* cp = buffer;
  ip[0] = ip[1] = ip[2] = ip[3] = 0;
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
      ip[0] = ip[1] = ip[2] = ip[3] = 0;
      return false;
    }
    ++cp;
  }
  return true;
}


#if defined(ARDUINO) && ARDUINO >= 100
boolean IniFile::getIPAddress(const char* section, const char* key,
			      char* buffer, int len, IPAddress& ip) const
{
  // Need 16 chars minimum: 4 * 3 digits, 3 dots and a null character
  if (len < 16)
    return false;

  if (getValue(section, key, buffer, len) < 0) 
    return false; // error

  int i = 0;
  char* cp = buffer;
  ip = IPAddress(0, 0, 0, 0);
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
#endif

boolean IniFile::getMACAddress(const char* section, const char* key,
			       char* buffer, int len, uint8_t mac[6]) const
{
  // Need 18 chars: 6 * 2 hex digits, 5 : or - and a null char
  if (len < 18)
    return false;

  if (getValue(section, key, buffer, len) < 0)
    return false; // error
  
  int i = 0;
  char* cp = buffer;
  memset(mac, 0, 6);

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

int8_t IniFile::readLine(File &file, char *buffer, int len, uint32_t &pos)
{
  if (!file)
    return errorFileNotOpen;
 
  if (len < 3) 
    return errorBufferTooShort;

  if (!file.seek(pos))
    return errorSeekError;

  int bytesRead = file.read(buffer, len);
  if (!bytesRead) {
    buffer[0] = '\0';
    return 1; // done
  }
  
  for (int i = 0; i < bytesRead && i < len-1; ++i) {
    // Test for '\n' with optional '\r' too
    // if (endOfLineTest(buffer, len, i, '\n', '\r')
	
    if (buffer[i] == '\n' || buffer[i] == '\r') {
      char match = buffer[i];
      char otherNewline = (match == '\n' ? '\r' : '\n'); 
      // end of line, discard any trailing character of the other sort
      // of newline
      buffer[i] = '\0';
      
      if (buffer[i+1] == otherNewline)
	++i;
      pos += (i + 1); // skip past newline(s)
      return (i+1 == bytesRead && !file.available());
    }
  }
  if (!file.available()) {
    // end of file without a newline
    buffer[bytesRead] = '\0';
    return 1; //done
  }
  
  buffer[len-1] = '\0'; // terminate the string
  return errorBufferTooShort;
}

boolean IniFile::isCommentChar(char c)
{
  return (c == ';' || c == '#');
}

char* IniFile::skipWhiteSpace(char* str)
{
  char *cp = str;
  while (isspace(*cp))
    ++cp;
  return cp;
}

void IniFile::removeTrailingWhiteSpace(char* str)
{
  char *cp = str + strlen(str) - 1;
  while (cp >= str && isspace(*cp))
    *cp-- = '\0';
}

int8_t IniFile::findSection(const char* section, char* buffer, int len, 
			    IniFileState &state) const
{
  if (section == NULL)
    return errorSectionNotFound;

  int8_t done = IniFile::readLine(_file, buffer, len, state.readLinePosition);
  if (done < 0)
    return done;

  char *cp = skipWhiteSpace(buffer);
  if (isCommentChar(*cp))
    return (done ? errorSectionNotFound : 0);

  if (*cp == '[') {
    // Start of section
    ++cp;
    cp = skipWhiteSpace(cp);
    char *ep = strchr(cp, ']');
    if (ep != NULL) {
      *ep = '\0'; // make ] be end of string
      removeTrailingWhiteSpace(cp);
      if (strcmp(cp, section) == 0)
	return 1;
    }
  }
  
  // Not a valid section line
  return (done ? errorSectionNotFound : 0);
}

// From the current file location look for the matching key
int8_t IniFile::findKey(const char* section, const char* key,
			char* buffer, int len, char** keyptr,
			IniFileState &state) const
{
  if (key == NULL || *key == '\0') 
    return errorKeyNotFound;

  int8_t done = IniFile::readLine(_file, buffer, len, state.readLinePosition);
  if (done < 0)
    return done;
  
  char *cp = skipWhiteSpace(buffer);
  if (isCommentChar(*cp))
    return (done ? errorKeyNotFound : 0);

  if (section && *cp == '[')
    // Stat of a new section
    return errorKeyNotFound;
   
  // Find '='
  char *ep = strchr(cp, '=');
  if (ep != NULL) {
    *ep = '\0'; // make = be the end of string
    removeTrailingWhiteSpace(cp);
    if (strcmp(cp, key) == 0) {
      *keyptr = ep + 1;
      return 1;
    }
  }
  return (done ? errorKeyNotFound : 0);
}

IniFileState::IniFileState()
{
  readLinePosition = 0;
  getValueState = funcUnset;
}
