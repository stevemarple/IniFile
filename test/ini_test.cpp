#include <iostream>

#include "IniFile.h"

using namespace std;


const char noError[] = "no error";
const char fileNotOpen[] = "file not open";
const char bufferTooShort[] = "buffer too short";
const char seekError[] = "seek error";
const char sectionNotFound[] = "section not found";
const char keyNotFound[] = "key not found";
const char unknownError[] = "unknown error";

const char* getError(int e)
{
  const char *cp = unknownError;

  switch (e) {
  case IniFile::errorFileNotOpen:
    cp = fileNotOpen;
    break;
  case IniFile::errorBufferTooShort:
    cp = bufferTooShort;
    break;
  case IniFile::errorSeekError:
    cp = seekError;
    break;
  case IniFile::errorSectionNotFound:
    cp = sectionNotFound;
    break;
  case IniFile::errorKeyNotFound:
    cp = keyNotFound; 
    break;
  default:
    if (e >= 0)
      cp = noError;
    break;
  };
  return cp;
}

void testForKey(IniFile &ini, const char *key, const char *section = NULL)
{
  cout << "    Looking for key \"" << key << '"';
  if (section)
    cout << " in section " << section;
  cout << endl;

  const int len = 80;
  char buffer[len];

  int8_t i = ini.getValue(section, key, buffer, len);
  if (i < 0) {
    cout << "      Error: " << getError(i) << " (" << int(i) << ")" << endl;
    if (i == IniFile::errorBufferTooShort)
      cout << "Buffer too short for line \"" << buffer << "...\"" << endl;
  }
  else
    cout << "      Value of " << key << " is \"" << buffer << '"' << endl;

}

void runTest(IniFile &ini)
{
  cout << "Using file " << ini.getFilename() << endl;
  cout << "  File open? " << (ini.isOpen() ? "true" : "false") << endl;

  testForKey(ini, "mac");
  testForKey(ini, "mac", "network");
  testForKey(ini, "mac", "network2");
  testForKey(ini, "mac", "fake");
  testForKey(ini, "ip");
  testForKey(ini, "gateway");
  testForKey(ini, "hosts allow", "network");
  testForKey(ini, "hosts allow", "network");
  testForKey(ini, "hosts allow", "network2");
  testForKey(ini, "hosts allow", "network2");
  testForKey(ini, "string", "misc");
  testForKey(ini, "string2", "misc");
  cout << "----" << endl;
  

}


int main(void)
{

  // Cannot cleanly pass string constants to IniFile constructor
  // because is doesn't take const char*, but that is because
  // SD.open() isn't const char*.
  char missingIniFilename[] = "missing.ini";
  char testIniFilename[] = "test.ini";

  // Try the construtor which opens a file
  IniFile missingIni(missingIniFilename);
  IniFile testIni(testIniFilename);
 
  cout << "*** Testing IniFile(char*) ***" << endl;
  missingIni.open();
  runTest(missingIni);
  testIni.open();
  runTest(testIni);
  cout << "Done" << endl;
  
}


