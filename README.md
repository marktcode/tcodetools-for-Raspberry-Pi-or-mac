# tcodetools
unix/linux command line tools for working with variable length self-synchronizing codes


Currently binary executables for Raspberry pi are available. The sources 
for these tools can be compiled for use in any UNIX or linux environment.
These enable one to create and work with variable-length self-synchronizing T-codes.

The process of constructiong the codes can be approached in several ways.

Starting from an alphabet and selecting prefix codes and copy factors in turn, to augment 
the alphabet and resultant intermediate code sets recursively.

Or to select an alphabet and then use a single string to specify the set through a process of decomposition.

The tools now include tc_mrsdcdr which is CW morse decoder accepts NRZ output as a stream of '0'/'1' chars from for example rtl_cw which is available in the repository RTL-SDR.
