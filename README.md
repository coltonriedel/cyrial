# CYRIAL
A C++ interface built on PyVISA to provide a communication interface for Jackson Labs GPSDO's (SCPI) and u-blox GNSS receivers (UBX). I also intend to include functions to control Symmetricom CSAC's.

If you happen to find this repository now be sure to check back later when it's a little more mature. In the mean time feedback and input is welcome.

##### Why PyVISA when you could interface with some other library?
The shortest answer is simply because it was the easiest. Prototyping for a larger project started in pure Python, so the next step was wrapping it up in C++. The ultimate objective is to have a core which could be exchanged with some other backend, but because I don't have strict performance requirements, am developing for devices connected via both USB and Serial ports, and am using everything from a Jetson TX2 to a MBP for development, PyVISA's portability is super attractive.
