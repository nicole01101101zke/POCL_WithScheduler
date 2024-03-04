<%!
        sub_page = "Portable Computing Language (pocl) v0.10 released"
%>
<%inherit file="basic_page.makt" />

<h1>September 4th, 2014: pocl v0.10 released</h2>

<p>Pocl's goal is to become an efficient open source (MIT-licensed)
implementation of the OpenCL standard (both 1.x and 2.x).</p>

<p>In addition to producing an easily portable open-source OpenCL
implementation, another major goal of this project is improving
performance portability of OpenCL programs with compiler
optimizations, reducing the need for target-dependent manual
optimizations.</p>

<p>At the core of pocl is the kernel compiler that consists of a set of
LLVM passes used to statically transform kernels into work-group
functions with multiple work-items, even in the presence of work-group
barriers. These functions are suitable for parallelization in multiple
ways (SIMD, VLIW, superscalar,...).</p>

<p>This release adds kernel compiler support for LLVM/Clang 3.5, 
experimental CMake build scripts, improved thread safety in the host libraries,
kernel compiler speedups, plenty of bug fixes, and several new OpenCL API 
implementations.</p>

<p>We consider pocl ready for wider scale testing, although the OpenCL
standard is not yet fully implemented, and it contains known bugs.
The pocl test suite compiles and runs most of the ViennaCL 1.5.1
examples, Rodinia 2.0.1 benchmarks, Parboil benchmarks, OpenCL
Programming Guide book samples, VexCL test cases, Luxmark v2.0, 
most of the AMD APP SDK v2.8 OpenCL samples and piglit OpenCL tests
among others.</p>

<h2>Acknowledgements</h2>

<p>We'd like to thank the Radio Implementation Research Team from Nokia
Research Center, Finnish Funding Agency for Technology and Innovation 
(project "Parallel Acceleration", funding decision 40115/13), Academy of 
Finland (funding decision 253087) and ARTEMIS joint undertaking under grant 
agreement no 621439 (ALMARVI) for funding the development of 
this release.<p>

<p>A pocl developer E. Schnetter acknowledges support from the Perimeter 
Institute, as well as funding from NSERC (Canada) via a Discovery Grant and 
from NSF (USA) via OCI Award 0905046.</p>

<p><a href="http://portablecl.org/download.html">Download</a>.</p>
