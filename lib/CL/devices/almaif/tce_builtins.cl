/* tce_builtins.cl - built-in kernel implementations to be compiled for TTAs

   Copyright (c) 2022 Topi Leppänen / Tampere University

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.
*/



__kernel void
pocl_mul_i32 (global const int *__restrict input1,
              global const int *__restrict input2,
              global int *__restrict output)
{
  size_t i = get_global_id (0);
  output[i] = input1[i] * input2[i];
}

__kernel void
pocl_add_i32 (global const int *__restrict input1,
              global const int *__restrict input2,
              global int *__restrict output)
{
  size_t i = get_global_id (0);
#ifdef cl_TCE_ADD
  clADDTCE (input1[i], input2[i], output[i]);
#else
  output[i] = input1[i] + input2[i];
#endif
}

__kernel void
pocl_abs_f32 (global const int *__restrict input,
              global int *__restrict output)
{
  size_t i = get_global_id (0);
#ifdef cl_TCE_ABSF
  clABSFTCE (input[i], output[i]);
#else
  output[i] = fabs (input[i]);
#endif
}
