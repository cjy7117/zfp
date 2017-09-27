.. include:: defs.rst
.. _tutorial:

Tutorial
========

This tutorial provides examples that illustrate how to use the |zfp|
library and compressed arrays, and includes code snippets that show
the order of declarations and function calls needed to use the
compressor.

This tutorial is divided into three parts: the high-level |libzfp|
:ref:`library <tut-hl>`; the low-level
:ref:`compression codecs <tut-ll>`; and the
:ref:`compressed array classes <tut-arrays>` (in that order).  Users
interested only in the compressed arrays, which do not directly expose
anything related to compression other than compression
:ref:`rate control <mode-fixed-rate>`, may safely skip the next two
sections.

All code examples below are for 3D arrays of doubles, but it should be
clear how to modify the function calls for single precision and for 1D
or 2D arrays.

.. _tut-hl:

High-Level C Interface
----------------------

Users concerned only with storing their floating-point data compressed may
use |zfp| as a black box that maps a possibly non-contiguous floating-point
array to a compressed bit stream.  The intent of |libzfp| is to provide both
a high- and low-level interface to the compressor that can be called from
both C and C++ (and possibly other languages).  |libzfp| supports strided
access, e.g. for compressing vector fields one scalar at a time, or for
compressing arrays of structs.

Consider compressing the 3D C/C++ array
::

  // define an uncompressed array
  double a[nz][ny][nx];

where *nx*, *ny*, and *nz* can be any positive dimensions.  To invoke the
|libzfp| compressor, the dimensions and type must first be specified in a
:c:type:`zfp_field` parameter object that encapsulates the type, size, and
memory layout of the array::

  // allocate metadata for the 3D array a[nz][ny][nx]
  uint dims = 3;
  zfp_type type = zfp_type_double;
  zfp_field* field = zfp_field_3d(&a[0][0][0], type, nx, ny, nz);

For single-precision data, use :code:`zfp_type_float`.  As of version 0.5.1,
the the high-level API also supports integer arrays (:code:`zfp_type_int32`
and :code:`zfp_type_int64`).  See FAQs :ref:`#8 <q-integer>` and
:ref:`#9 <q-int32>` regarding integer compression.

Functions similar to :c:func:`zfp_field_3d` exist for declaring 1D and 2D
arrays.  If the dimensionality of the array is unknown at this point, then
a generic :c:func:`zfp_field_alloc` call can be made to just allocate a
:c:type:`zfp_field` struct, which can be filled in later using the
:ref:`set <zfp_field_set>` functions.  If the array is non-contiguous, then
:c:func:`zfp_field_set_stride_3d` should be called.

The :c:type:`zfp_field` parameter object holds information about the
uncompressed array.  To specify the compressed array, a :c:type:`zfp_stream`
object must be allocated::

  // allocate metadata for a compressed stream
  zfp_stream* zfp = zfp_stream_open(NULL);

We may now specify the rate, precision, or accuracy (see :ref:`modes`
for more details on the meaning of these parameters)::

  // set compression mode and parameters
  zfp_stream_set_rate(zfp, rate, type, dims, 0);
  zfp_stream_set_precision(zfp, precision);
  zfp_stream_set_accuracy(zfp, tolerance);

Note that only one of these three functions should be called.  The return
value from these functions gives the actual rate, precision, or tolerance,
and may differ slightly from the argument passed due to constraints imposed
by the compressor, e.g. each block must be stored using a whole number of
bits at least as large as the number of bits in the floating-point exponent;
the precision cannot exceed the number of bits in a floating-point value
(i.e. 32 for single and 64 for double precision); and the tolerance must
be a (possibly negative) power of two.

The compression parameters have now been specified, but before compression
can occur a buffer large enough to hold the compressed bit stream must be
allocated.  Another utility function exists for estimating how many bytes
are needed::

  // allocate buffer for compressed data
  size_t bufsize = zfp_stream_maximum_size(zfp, field);
  uchar* buffer = new uchar[bufsize];

Note that :c:func:`zfp_stream_maximum_size` returns the smallest buffer
size necessary to safely compress the data--the *actual* compressed size
may be smaller.  If the members of :code:`zfp` and :code:`field` are for
whatever reason not initialized correctly, then
:c:func:`zfp_stream_maximum_size` returns 0.

Before compression can commence, we must associate the allocated buffer
with a bit stream used by the compressor to read and write bits::

  // associate bit stream with allocated buffer
  bitstream* stream = stream_open(buffer, bufsize);
  zfp_stream_set_bit_stream(zfp, stream);

Finally, the array is compressed as follows::

  // compress entire array
  size_t size = zfp_compress(zfp, field);

The return value is the actual number of bytes of compressed storage,
and as already mentioned, *size* |leq| *bufsize*.  If *size* = 0, then the
compressor failed.  Since |zfp| 0.5.0, the compressor does not rewind the
bit stream before compressing, which allows multiple fields to be compressed
one after the other.  The return value from :c:func:`zfp_compress` is always
the total number of bytes of compressed storage so far relative to the memory
location pointed to by *buffer*.

To decompress the data, the field and compression parameters must be
initialized with the same values as used for compression, either via
the same sequence of function calls as above, or by recording these
fields and setting them directly.  Metadata such as array dimensions and
compression parameters are by default not stored in the compressed stream.
It is up to the caller to store this information, either separate from
the compressed data, or via the :c:func:`zfp_write_header` and
:c:func:`zfp_read_header` calls, which must precede the corresponding
:c:func:`zfp_compress` and :c:func:`zfp_decompress` calls, respectively.
These calls allow the user to specify what information to store in the header,
including a 'magic' format identifier, the field type and dimensions, and the
compression parameters (see the :ref:`ZFP_HEADER <header-macros>` macros).

In addition to this initialization, the bit stream has to be rewound to
the beginning (before reading the header and decompressing the data)::

  // rewind compressed stream and decompress array
  zfp_stream_rewind(zfp);
  int success = zfp_decompress(zfp, field);

The return value is zero if the decompressor failed.

Simple Example
^^^^^^^^^^^^^^

Tying it all together, the code example below (see also the
:ref:`simple <ex-simple>` program) shows how to compress a 3D array
:code:`double array[nz][ny][nx]`::

  // input: (void* array, int nx, int ny, int nz, double tolerance)

  // initialize metadata for the 3D array a[nz][ny][nx]
  zfp_type type = zfp_type_double;                          // array scalar type
  zfp_field* field = zfp_field_3d(array, type, nx, ny, nz); // array metadata

  // initialize metadata for a compressed stream
  zfp_stream* zfp = zfp_stream_open(NULL);                  // compressed stream and parameters
  zfp_stream_set_accuracy(zfp, tolerance);                  // set tolerance for fixed-accuracy mode
  //  zfp_stream_set_precision(zfp, precision);             // alternative: fixed-precision mode
  //  zfp_stream_set_rate(zfp, rate, type, 3, 0);           // alternative: fixed-rate mode

  // allocate buffer for compressed data
  size_t bufsize = zfp_stream_maximum_size(zfp, field);     // capacity of compressed buffer (conservative)
  void* buffer = malloc(bufsize);                           // storage for compressed stream

  // associate bit stream with allocated buffer
  bitstream* stream = stream_open(buffer, bufsize);         // bit stream to compress to
  zfp_stream_set_bit_stream(zfp, stream);                   // associate with compressed stream
  zfp_stream_rewind(zfp);                                   // rewind stream to beginning

  // compress array
  size_t zfpsize = zfp_compress(zfp, field);                // return value is byte size of compressed stream


.. _tut-ll:

Low-Level C Interface
---------------------

For applications that wish to compress or decompress portions of an array
on demand, a low-level interface is available.  Since this API is useful
primarily for supporting random access, the user also needs to manipulate
the :ref:`bit stream <bs-api>`, e.g. to position the bit pointer to where
data is to be read or written.  Please be advised that the bit stream
functions have been optimized for speed, and do not check for buffer
overruns or other types of programmer error.

Like the high-level API, the low-level API also makes use of the
:c:type:`zfp_stream` parameter object (see section above) to specify
compression parameters and storage, but does not encapsulate array
metadata in a :c:type:`zfp_field` object.  Functions exist for encoding
and decoding complete or partial blocks, with or without strided access.
In non-strided mode, the uncompressed block to be encoded or decoded is
assumed to be stored contiguously.  For example,
::

  // compress a single contiguous block
  double block[4 * 4 * 4] = { /* some set of values */ };
  uint bits = zfp_encode_block_double_3(zfp, block);

The return value is the number of bits of compressed storage for the block.
For fixed-rate streams, if random access is desired, then the stream should
also be flushed after each block is encoded::

  // flush any buffered bits
  zfp_stream_flush(zfp);

This flushing should be done only after the last block has been compressed in
fixed-precision and fixed-accuracy mode, or when random access is not needed
in fixed-rate mode.

The block above could also have been compressed as follows using strides::

  // compress a single contiguous block using strides
  double block[4][4][4] = { /* some set of values */ };
  int sx = &block[0][0][1] - &block[0][0][0]; // x stride =  1
  int sy = &block[0][1][0] - &block[0][0][0]; // y stride =  4
  int sz = &block[1][0][0] - &block[0][0][0]; // z stride = 16
  uint bits = zfp_encode_block_strided_double_3(zfp, block, sx, sy, sz);

The strides are measured in number of scalars, not in bytes.

For partial blocks, e.g. near the boundaries of arrays whose dimensions
are not multiples of four, there are corresponding functions that accept
parameters *nx*, *ny*, and *nz* to specify the actual block dimensions,
with 1 |leq| *nx*, *ny*, *nz* |leq| 4.  Corresponding functions exist for
decompression.  Such partial blocks typically do not compress as well as
full blocks and should be avoided if possible.

To position a bit stream for reading (decompression), use
::

  // position the stream at given bit offset for reading
  stream_rseek(stream, offset);

where the offset is measured in number of bits from the beginning of the
stream.  For writing (compression), a corresponding call exists::

  // position the stream at given bit offset for writing
  stream_wseek(stream, offset);

Note that it is possible to decompress fewer bits than are stored with a
compressed block to quickly obtain an approximation.  This is done by
setting :code:`zfp->maxbits` to fewer bits than used during compression,
e.g. to decompress only the first 256 bits of each block::

  // modify decompression parameters to decode 256 bits per block
  uint maxbits;
  uint maxprec;
  int minexp;
  zfp_stream_params(zfp, NULL, &maxbits, &maxprec, &minexp);
  assert(maxbits >= 256);
  zfp_stream_set_params(zfp, 256, 256, maxprec, minexp);

This feature may be combined with progressive decompression, as discussed
further in FAQ :ref:`#13 <q-progressive>`.

.. _tut-arrays:

Compressed C++ Arrays
---------------------

.. cpp:namespace:: zfp

The |zfp| API has been designed to facilitate integration with existing
applications.  After initial array declaration, a |zfp| array can often
be used in place of a regular C/C++ array or STL vector, e.g. using flat
indexing via :code:`a[index]` or using multidimensional indexing via
:code:`a(i)`, :code:`a(i, j)`, or :code:`a(i, j, k)`.  There are,
however, some important differences.  For instance, applications that
rely on addresses or references to array elements may have to be
modified to use special proxy classes that implement pointers and
references; see :ref:`limitations`.

|zfp| does not support special floating-point values like infinities and
NaNs, although denormalized numbers are handled correctly.  Similarly,
because the compressor assumes that the array values vary smoothly,
using finite but large values like :c:macro:`HUGE_VAL` in place of
infinities is not advised, as this will introduce large errors in smaller
values within the same block.  Future extensions will provide support for
a bit mask to mark the presence of non-values.

The |zfp| C++ classes are implemented entirely as header files and make
extensive use of C++ templates to reduce code redundancy.  Most classes
are wrapped in the :cpp:any:`zfp` namespace.

Currently there are six array classes for 1D, 2D, and 3D arrays, each of
which can represent single- or double-precision values.  Although these
arrays store values in a form different from conventional single- and
double-precision floating point, the user interacts with the arrays via
floats and doubles.

The description below is for 3D arrays of doubles--the necessary changes
for other array types should be obvious.  To declare and zero initialize
an array, use
::

  // declare nx * ny * nz array of compressed doubles
  zfp::array3<double> a(nx, ny, nz, rate);

This declaration is conceptually equivalent to
::

  double a[nz][ny][nx] = {};

or using STL vectors
::

  std::vector<double> a(nx * ny * nz, 0.0);

but with the user specifying the amount of storage used via the *rate*
parameter.  (A predefined type :cpp:type:`array3d` also exists, while
the suffix 'f' is used for floats.)  Note that the array dimensions can
be arbitrary, and need not be multiples of four (see above for a discussion
of incomplete blocks).  The *rate* argument specifies how many bits per
value (amortized) to store in the compressed representation.  By default
the block size is restricted to a multiple of 64 bits, and therefore the
rate argument can be specified in increments of 64 / |4powd| bits in *d*
dimensions, i.e.
::

  1D arrays: 16-bit granularity
  2D arrays: 4-bit granularity
  3D arrays: 1-bit granularity

For finer granularity, the :c:macro:`BIT_STREAM_WORD_TYPE` macro needs to
be set to a type narrower than 64 bits during compilation of |libzfp|,
e.g. if set to :c:type:`uint8` the rate granularity becomes 8 / |4powd|
bits in *d* dimensions, or
::

  1D arrays: 2-bit granularity
  2D arrays: 1/2-bit granularity
  3D arrays: 1/8-bit granularity

Note that finer granularity usually implies slightly lower performance.
Also note that because the arrays are stored compressed, their effective
precision is likely to be higher than the user-specified rate.

The array can also optionally be initialized from an existing contiguous
floating-point array stored at *pointer* with an *x* stride of 1, *y*
stride of *nx*, and *z* stride of *nx* |times| *ny*::

  // declare and initialize 3D array of doubles
  zfp::array3d a(nx, ny, nz, precision, pointer, cache_size);

The optional *cache_size* argument specifies the minimum number of bytes
to allocate for the cache of uncompressed blocks (see :ref:`tut-caching`
below for more details).

If not already initialized, a function :cpp:func:`array::set` can be used
to copy uncompressed data to the compressed array::

  const double* pointer; // pointer to uncompressed, initialized data
  a.set(pointer); // initialize compressed array with floating-point data

Similarly, an :cpp:func:`array::get` function exists for retrieving
uncompressed data::

  double* pointer; // pointer to where to write uncompressed data
  a.get(pointer); // decompress and store the array at pointer

The compressed representation of an array can also be queried or initialized 
directly without having to convert to/from its floating-point representation::

  size_t bytes = compressed_size(); // number of bytes of compressed storage
  uchar* compressed_data(); // pointer to compressed data

The array can through this pointer be initialized from offline compressed
storage, but only after its dimensions and rate have been specified (see
above).  For this to work properly, the cache must first be emptied via a
:cpp:func:`array::clear_cache` call (see below).

Through operator overloading, the array can be accessed in one of two ways.
For read accesses, use
::

  double value = a[index]; // fetch value with given flat array index
  double value = a(i, j, k); // fetch value with 3D index (i, j, k)

These access the same value if and only if
:code:`index = i + nx * (j + ny * k)`.
Note that 0 |leq| *i* < *nx*, 0 |leq| *j* < *ny*, and 0 |leq| *k* < *nz*,
and *i* varies faster than *j*, which varies faster than *k*.

Array values may be written and updated using the usual set of C++ assignment
and compound assignment operators.  For example::

  a[index] = value; // set value at flat array index
  a(i, j, k) += value; // increment value with 3D index (i, j, k)

Whereas one might expect these operators to return a (non-const) reference
to an array element, this would allow seating a reference to a value that
currently is cached but is transient, which could be unsafe.  Moreover,
this would preclude detecting when an array element is modified.  Therefore,
the return type of both operators :code:`[]` and :code:`()` is a proxy
reference class, similar to :code:`std::vector<bool>::reference` from the
STL library.  Because read accesses to a mutable object cannot call the
const-qualified accessor, a proxy reference may be returned even for read
calls, e.g. in
::

  a[i - 1] = a[i];

the array :code:`a` clearly must be mutable to allow assignment to
:code:`a[i - 1]`, and therefore the read access :code:`a[i]` returns type
:cpp:class:`array::reference`.  The value associated with the read access
is obtained via an implicit conversion.

Array dimensions *nx*, *ny*, and *nz* can be queried using these functions::

  size_t size(); // total number of elements nx * ny * nz
  uint size_x(); // nx
  uint size_y(); // ny
  uint size_z(); // nz

The array dimensions can also be changed dynamically, e.g. if not known
at time of construction, using
::

  void resize(uint nx, uint ny, uint nz, bool clear = true);

When *clear* = true, the array is explicitly zeroed.  In either case, all
previous contents of the array are lost.  If *nx* = *ny* = *nz* = 0, all
storage is freed.

Finally, the rate supported by the array may be queried via
::

  double rate(); // number of compressed bits per value

and changed using
::

  void set_rate(rate); // change rate

This also destroys prior contents.

As of |zfp| 0.5.2, iterators and proxy objects for pointers and references are
supported.  Note that the decompressed value of an array element exists only
intermittently, when the decompressed value is cached.  It would not be safe
to return a :code:`double&` reference or :code:`double*` pointer to the cached
but transient value since it may be evicted from the cache at any point, thus
invalidating the reference or pointer.  Instead, |zfp| provides proxy objects
for references and pointers that guarantee persistent access by referencing
elements by array object and index.  These classes perform decompression on
demand, much like how Boolean vector references are implemented in the STL.

Iterators for 1D arrays support random access, while 2D and 3D array iterators
are merely forward (sequential) iterators.  All iterators ensure that array
values are visited one block at a time, and are the preferred way of looping
over array elements.  Such block-by-block access is especially useful when
performing write accesses since then complete blocks are updated one at a
time, thus reducing the likelihood of a partially updated block being evicted
from the cache and compressed, perhaps with some values in the block being
uninitialized.  Here is an example of initializing a 3D array::

  for (zfp::array3d::iterator it = a.begin(); it != a.end(); it++) {
    int i = it.i();
    int j = it.j();
    int k = it.k();
    a(i, j, k) = some_function(i, j, k);
  }

Pointers to array elements are available via a special pointer class.  Such
pointers may be a useful way of passing (flattened) |zfp| arrays to functions
that expect uncompressed arrays, e.g. by using the pointer type as template
argument.  For example::

  template <typename double_ptr>
  void sum(double_ptr p, int count)
  {
    double s = 0;
    for (int i = 0; i < count; i++)
      s += p[i];
    return s;
  }

Then the following are equivalent::

  // sum of STL vector elements (double_ptr == double*)
  std::vector<double> vec(nx * ny * nz, 0.0);
  double vecsum = sum(&vec[0], nx * ny * nz);

  // sum of zfp array elements (double_ptr == zfp::array3d::pointer)
  zfp::array3<double> array(nx, ny, nz, rate);
  double zfpsum = sum(&array[0], nx * ny * nz);

As another example,
::

  for (zfp::array1d::pointer p = &a[0]; p - &a[0] < a.size(); p++)
    *p = 0.0;

initializes a 1D array to all-zeros.  Pointers visit arrays in standard
row-major order, i.e.
::

  &a(i, j, k) == &a[0] + i + nx * (j + ny * k)
              == &a[i + nx * (j + ny * k)]

where :code:`&a(i, j, k)` and :code:`&a[0]` are both of type
:cpp:class:`array3d::pointer`.  Thus, iterators and pointers do not
visit arrays in the same order, except for the special case of 1D arrays.
Unlike iterators, pointers support random access for arrays of all
dimensions and behave very much like :code:`float*` and :code:`double*`
built-in pointers.

Proxy objects for array element references have been supported since the
first release of |zfp|, and may for instance be used in place of
:code:`double&`.  Iterators and pointers are implemented in terms of
references.

The following table shows the equivalent zfp type to standard types when
working with 1D arrays::

  double&                         zfp::array1d::reference
  double*                         zfp::array1d::pointer
  std::vector<double>::iterator   zfp::array1d::iterator

.. _tut-caching:

Caching
^^^^^^^

As mentioned above, the array class maintains a software write-back cache
of at least one uncompressed block.  When a block in this cache is evicted
(e.g. due to a conflict), it is compressed back to permanent storage only
if it was modified while stored in the cache.

The size cache to use is specified by the user, and is an important
parameter that needs careful consideration in order to balance the extra
memory usage, performance, and quality (recall that data loss is incurred
only when a block is evicted from the cache and compressed).  Although the
best choice varies from one application to another, we suggest allocating
at least two layers of blocks (2 |times| (*nx* / 4) |times| (*ny* / 4)
blocks) for applications that stream through the array and perform stencil
computations such as gathering data from neighboring elements.  This allows
limiting the cache misses to compulsory ones.  If the *cache_size* parameter
is set to zero bytes, then this default of two layers is used.

The cache size can be set during construction, or can be set at a later
time via
::

  void set_cache_size(bytes); // change cache size

Note that if *bytes* = 0, then the array dimensions must have already been
specified for the default size to be computed correctly.  When the cache
is resized, it is first flushed if not already empty.  The cache can
also be flushed explicitly if desired by calling
::

  void flush_cache(); // empty cache by first compressing any modified blocks

To empty the cache without compressing any cached data, call
::

  void clear_cache(); // empty cache without compression

To query the byte size of the cache, use
::

  size_t cache_size(); // actual cache size in bytes