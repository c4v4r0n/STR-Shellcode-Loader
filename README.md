# STR Shellcode Loader
This is a simple proof of concept that can "bypass" some static analysis tools.
This is just a poc and should not be used to execute malicious activities

NOTE: This is not meant to be a silver bullet againts AVs, this is just a PoC to load shellcode from the rsrc section as a string instead of using RAW data.

## Introduction
Traditionally, many PoCs for shellcode loaders store raw shellcode in the .rsrc section of a PE and then load it at runtime. A common pattern uses FindResource / LoadResource / LockResource and SizeofResource to retrieve the raw bytes.

While testing AV engines, I noticed that this pattern can itself trigger detections. To explore alternatives, I experimented with different resource types and discovered that string resources (RT_STRING) can be abused to store payload data retrievable via LoadStringA. Below is a short proof of concept I designed to store and retrieve a payload from the PE resource section.

## How to use it
1. Using the Python script, create an RT_STRING file for your shellcode, this will create the resources\strings.rc file and will be used by the compiler to save the resource
```cmd

python.exe encoder.py evil.bin
```

2. Edit the constant PAYLOAD_SIZE on main.c

3. Compile using ```compiler.bat```

4. Execute the ```loader.exe``` binary.
