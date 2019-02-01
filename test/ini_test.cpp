#include <iostream>

#include "IniFile.h"

using namespace std;


const char noError[] = "no error";
const char fileNotFound[] = "file not found";
const char fileNotOpen[] = "file not open";
const char bufferTooSmall[] = "buffer too small";
const char seekError[] = "seek error";
const char sectionNotFound[] = "section not found";
const char keyNotFound[] = "key not found";
const char unknownError[] = "unknown error";
const char unknownErrorValue[] = "unknown error value";

const char* getErrorMessage(int e)
{
  const char *cp = unknownError;

  switch (e) {
  case IniFile::errorNoError:
    cp = noError;
    break;
  case IniFile::errorFileNotFound:
    cp = fileNotFound;
    break;
  case IniFile::errorFileNotOpen:
    cp = fileNotOpen;
    break;
  case IniFile::errorBufferTooSmall:
    cp = bufferTooSmall;
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
    cp = unknownErrorValue;
    break;
  };
  return cp;
}

void testForKey(IniFile &ini, const char *key, const char *section = NULL)
{
  cout << "    Looking for key \"" << key << '"';
  if (section)
    cout << " in section \"" << section << "\"";
  cout << endl;

  const int len = 80;
  char buffer[len];

  bool b = ini.getValue(section, key, buffer, len);
  if (b == false) {
    int e = ini.getError();
    cout << "      Error: " << getErrorMessage(e) << " (" << int(e) << ")"
	 << endl;
    if (ini.getError() == IniFile::errorBufferTooSmall)
      cout << "Buffer too small for line \"" << buffer << "...\"" << endl;
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
  testForKey(ini, "pi", "misc");
  float iniPi = 0.0;
  char buffer[80];
  if (ini.getValue("misc", "pi", buffer, sizeof(buffer), iniPi)) {
    cout << "    Pi: " << iniPi << endl;
    if (iniPi <= 3.1410 || iniPi >= 3.1416)
      cout << "    Pi out of range" << endl;
  }
  else {
    int e = ini.getError();
    cout << "    Could not read \"pi\" from section \"misc\" in " << ini.getFilename()
	 << endl
	 << "      Error: " << getErrorMessage(e) << " (" << int(e) << ")"
	 << endl;
  }
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


