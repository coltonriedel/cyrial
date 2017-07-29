# CYRIAL
A C++ interface built on PyVISA to provide a communication interface for Jackson Labs GPSDO's (SCPI), u-blox GNSS receivers (UBX), and Microsemi/Symmetricom CSAC's.

##### Why PyVISA when you could interface with some other library?
The shortest answer is simply because it was the easiest, given what I had already done.

Prototyping for a larger project started in pure Python, so the next step was wrapping it up in C++. The ultimate objective is to have a core (manager and interface) which can be exchanged with some other backend in the future.

For now however, I don't have strict performance requirements, and am developing on a diverse set of devices, so PyVISA works well.
