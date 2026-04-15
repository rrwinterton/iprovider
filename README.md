# iprovider
a dll that is a provider of instrumentation for the ireporter. the providers are tested in itemplate.

iprovider/  
├── CMakeLists.txt  
├── include/  
│   ├── api_exports.h     (The unmangled C-API header)  
│   └── math_engine.h     (Your actual C++ class)  
├── src/  
│   ├── api_exports.cpp   (Implementation of the bridge)  
│   └── math_engine.cpp   (Implementation of your class)  
└── build.bat             (Build script)  

