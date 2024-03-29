
**************************
Release Notes for PoCL 5.0
**************************

===================
New backend: Remote
===================

PoCL now has a new backend (called 'remote') for offloading OpenCL commands
across a network to one or more servers that are running the 'pocld' daemon
shipped with PoCL. See the `announcement <http://portablecl.org/remote-backend.html>`_
and the `documentation <http://portablecl.org/docs/html/remote.html>`_ for more details.

========================
Driver-specific features
========================

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CUDA driver: More OpenCL 3.0 features and extensions implemented
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CUDA driver received new features to support more optional OpenCL 3.0
features and various extensions that are needed to (better)
run `chipStar <http://github.com/CHIP-SPV/chipStar>`_:

* Coarse-grain and fine-grain SVM, if supported by the CUDA device
* Program scope variables
* OpenCL 2.x/3.x atomics
* Initial subgroup support: intel_sub_group_shuffle, intel_sub_group_shuffle_xor,
  get_sub_group_local_id, sub_group_barrier, sub_group_ballot
* Enable FP16 & generic address space support with SPIR-V input
* cl_ext_float_atomics (on FP32 + FP64) when using LLVM 17

~~~~~~~~~~~~~~~~~~~~~~~~~~~
RISC-V CPU support improved
~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this release we improved support for RISC-V CPUs. We tested PoCL on
a Starfive VisionFive 2 using a Ubuntu 23.10 preinstalled image. With
LLVM 17 and GCC 13.2, 98% tests pass (only 4 tests fail out of 253).

~~~~~~~~~~~~~~~~~~~~~~
Other CPU improvements
~~~~~~~~~~~~~~~~~~~~~~

There were also many other CPU driver improvements, this time
mostly driven by `chipStar's <http://github.com/CHIP-SPV/chipStar>`_ needs:

* cl_ext_float_atomics has been implemented with support for FP32 and FP64
* cl_khr_command_buffer implementation has been updated to 0.9.4

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Experimental built-in-kernel library based FPGA backend
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

AlmaIF backend added a DBDevice backend, which can be used to transparently
reconfigure FPGAs from different vendors using a database of bitstreams.
This work is from an in-progress research work and can be considered
as a preview. The database with the bitstreams is generated by
`AFOCL project <https://github.com/cpc/AFOCL>`_. For more information of
the research project, see the publication `"AFOCL: Portable OpenCL
Programming of FPGAs via Automated Built-in Kernel Management" (NorCAS 2023)
<https://trepo.tuni.fi/handle/10024/152831>`_

===================================
Deprecation/feature removal notices
===================================

There are some features we plan to deprecate or remove to clean up
the codebase a bit.

~~~~~~~~~~~~~~~~~~~~~~~~~~
LLVM: old versions cleanup
~~~~~~~~~~~~~~~~~~~~~~~~~~

Support for LLVM versions 10 to 13 inclusive is deprecated and will be
removed in next PoCL release.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The old SPIR support will be removed
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CPU drivers currently report `cl_khr_spir` for SPIR 1.x/2.0 support, but the
support has never been finished nor tested properly and is likely completely
unused. Thus, we believe it's time to remove the partial implementation in the
next release to clean up the code base. SPIR-V remains the supported and
recommended intermediate language input option.
