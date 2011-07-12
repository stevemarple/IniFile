IniFile is an Arduino library for parsing INI files. The format is
similar to that seen in Microsoft .ini files but the implementation is
completely independent. IniFile is designed to use minimal memory
requirements, and the only buffer used is one supplied by the user,
thus the user remains in charge of memory usage.

The ini file is separated into sections, where the section names are
written inside square brackets. If you don't wish to use sections then
pass a NULL pointer for the section name. Under each section are a set
of key-value pairs, separated by an equals sign. Spaces around keys
and values are ignored, but extra spaces inside the key are
significant. Whitespace inside the value string is preserved; if
leading or trailing whitespace is important you must quote the value
string inside the ini file, and you must strip out the quotes
yourself. If multiple entries for the same key exist inside the
selected section (if using) then only the first value is returned. If
a section is defined more than once only the first is used. The ini
file can contain comments, which begin with a semicolon (;) or hash
(#). The user-supplied buffer must be large enough to accomodate the
longest line in the file.

Example file format:

-----------------------------------------8<---------------------
; Semi-colon comment
[network]
mac = 01:23:45:67:89:AB

# hash comment, leading spaces below
 gateway = 192.168.1.1

# extraneous spaces before and after key and value
 ip   =   192.168.1.2             

hosts allow = example.com

# A similarly-named section
[network2]
mac = ee:ee:ee:ee:ee:ee
subnet mask=255.255.255.0

; Extra whitespace around the key and value is permitted
; (and ignored)
  hosts allow =    sloppy.example.com  

[misc]

string = 123456789012345678901234567890123456789001234567890
string2 = a string with spaces in it

; This section is a repeat of en existing section and will be ignored.
[network]
mac = 01:23:45:67:89:ab
ip = 192.168.1.2
gateway

-----------------------------------------8<---------------------
