// ###############
// cfp_array tests
// ###############

static void
_catFunc3(given_, CFP_ARRAY_TYPE, _when_ctor_expect_paramsSet)(void **state)
{
  struct setupVars *bundle = *state;
  size_t csize = 300;
  CFP_ARRAY_TYPE cfpArr = CFP_NAMESPACE.SUB_NAMESPACE.ctor(bundle->dataSideLen, bundle->dataSideLen, bundle->dataSideLen, bundle->dataSideLen, bundle->rate, bundle->dataArr, csize);
  assert_non_null(cfpArr.object);

  assert_int_equal(CFP_NAMESPACE.SUB_NAMESPACE.size(cfpArr), bundle->totalDataLen);

  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.rate(cfpArr) >= bundle->rate);

  uchar* compressedPtr = CFP_NAMESPACE.SUB_NAMESPACE.compressed_data(cfpArr);
  size_t compressedSize = CFP_NAMESPACE.SUB_NAMESPACE.compressed_size(cfpArr);
  assert_int_not_equal(hashBitstream((uint64*)compressedPtr, compressedSize), 0);

  // sets a minimum cache size
  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.cache_size(cfpArr) >= csize);

  CFP_NAMESPACE.SUB_NAMESPACE.dtor(cfpArr);
}

static void
_catFunc3(given_, CFP_ARRAY_TYPE, _when_resize_expect_sizeChanged)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;

  size_t newSizeX = 81, newSizeY = 123, newSizeZ = 14, newSizeW;
  assert_int_not_equal(CFP_NAMESPACE.SUB_NAMESPACE.size(cfpArr), newSizeX * newSizeY * newSizeZ * newSizeW);

  CFP_NAMESPACE.SUB_NAMESPACE.resize(cfpArr, newSizeX, newSizeY, newSizeZ, newSizeW, 1);

  assert_int_equal(CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr), newSizeX);
  assert_int_equal(CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr), newSizeY);
  assert_int_equal(CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr), newSizeZ);
  assert_int_equal(CFP_NAMESPACE.SUB_NAMESPACE.size_w(cfpArr), newSizeW);
  assert_int_equal(CFP_NAMESPACE.SUB_NAMESPACE.size(cfpArr), newSizeX * newSizeY * newSizeZ * newSizeW);
}

static void
_catFunc3(given_, CFP_ARRAY_TYPE, _when_set_expect_entryWrittenToCacheOnly)(void **state)
{
  struct setupVars *bundle = *state;

  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;

  // getting the ptr automatically flushes cache, so do this before setting an entry
  uchar* compressedDataPtr = CFP_NAMESPACE.SUB_NAMESPACE.compressed_data(cfpArr);
  size_t compressedSize = CFP_NAMESPACE.SUB_NAMESPACE.compressed_size(cfpArr);

  uchar* oldMemory = malloc(compressedSize * sizeof(uchar));
  memcpy(oldMemory, compressedDataPtr, compressedSize);

  CFP_NAMESPACE.SUB_NAMESPACE.set(cfpArr, 1, 1, 1, 1, (SCALAR)VAL);

  assert_memory_equal(compressedDataPtr, oldMemory, compressedSize);
  free(oldMemory);
}

static void
_catFunc3(given_, CFP_ARRAY_TYPE, _when_get_expect_entryReturned)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i = 1, j = 2, k = 1, l = 1;
  CFP_NAMESPACE.SUB_NAMESPACE.set(cfpArr, i, j, k, l, (SCALAR)VAL);

  // dirty cache doesn't immediately apply compression
  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.get(cfpArr, i, j, k, l) == (SCALAR)VAL);
}

static void
_catFunc3(given_, CFP_ARRAY_TYPE, _when_ref_expect_arrayObjectValid)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i = 1, j = 2, k = 1, l = 1;
  CFP_REF_TYPE cfpArrRef = CFP_NAMESPACE.SUB_NAMESPACE.ref(cfpArr, i, j, k, l);

  assert_ptr_equal(cfpArrRef.array.object, cfpArr.object);
}

static void
_catFunc3(given_, CFP_ARRAY_TYPE, _when_ptr_expect_arrayObjectValid)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i = 1, j = 2, k = 1, l = 1;
  CFP_PTR_TYPE cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i, j, k, l);

  assert_ptr_equal(cfpArrPtr.reference.array.object, cfpArr.object);
}

static void
_catFunc3(given_, CFP_ARRAY_TYPE, _when_begin_expect_objectValid)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  CFP_ITER_TYPE cfpArrIter = CFP_NAMESPACE.SUB_NAMESPACE.begin(cfpArr);

  assert_ptr_equal(cfpArrIter.array.object, cfpArr.object);
  assert_int_equal(cfpArrIter.i, 0);
  assert_int_equal(cfpArrIter.j, 0);
  assert_int_equal(cfpArrIter.k, 0);
  assert_int_equal(cfpArrIter.l, 0);
}

static void
_catFunc3(given_, CFP_ARRAY_TYPE, _when_end_expect_objectValid)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  CFP_ITER_TYPE cfpArrIter = CFP_NAMESPACE.SUB_NAMESPACE.end(cfpArr);

  assert_ptr_equal(cfpArrIter.array.object, cfpArr.object);
  assert_int_equal(cfpArrIter.i, 0);
  assert_int_equal(cfpArrIter.j, 0);
  assert_int_equal(cfpArrIter.k, 0);
  assert_int_equal(cfpArrIter.l, SIZE_W);
}


// #############
// cfp_ref tests
// #############

static void
_catFunc3(given_, CFP_REF_TYPE, _when_get_expect_entryReturned)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i = 1, j = 2, k = 1, l = 1;
  CFP_REF_TYPE cfpArrRef = CFP_NAMESPACE.SUB_NAMESPACE.ref(cfpArr, i, j, k, l);
  CFP_NAMESPACE.SUB_NAMESPACE.set(cfpArr, i, j, k, l, VAL);

  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.reference.get(cfpArrRef) == (SCALAR)VAL);
}

static void
_catFunc3(given_, CFP_REF_TYPE, _when_set_expect_arrayUpdated)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i = 1, j = 2, k = 1, l = 1;
  CFP_REF_TYPE cfpArrRef = CFP_NAMESPACE.SUB_NAMESPACE.ref(cfpArr, i, j, k, l);
  CFP_NAMESPACE.SUB_NAMESPACE.reference.set(cfpArrRef, VAL);

  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.get(cfpArr, i, j, k, l) == (SCALAR)VAL);
}

static void
_catFunc3(given_, CFP_REF_TYPE, _when_copy_expect_arrayUpdated)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i1 = 1, j1 = 2, k1 = 1, l1 = 1, i2 = 2, j2 = 1, k2 = 2, l2 = 2;
  CFP_NAMESPACE.SUB_NAMESPACE.set(cfpArr, i1, j1, k1, l1, VAL);
  CFP_REF_TYPE cfpArrRef_a = CFP_NAMESPACE.SUB_NAMESPACE.ref(cfpArr, i1, j1, k1, l1);
  CFP_REF_TYPE cfpArrRef_b = CFP_NAMESPACE.SUB_NAMESPACE.ref(cfpArr, i2, j2, k2, l2);
  CFP_NAMESPACE.SUB_NAMESPACE.reference.copy(cfpArrRef_b, cfpArrRef_a);

  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.get(cfpArr, i2, j2, k2, l2) == (SCALAR)VAL);
}

static void
_catFunc3(given_, CFP_REF_TYPE, _when_ptr_expect_addressMatches)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i = 1, j = 2, k = 1, l = 1;
  CFP_REF_TYPE cfpArrRef = CFP_NAMESPACE.SUB_NAMESPACE.ref(cfpArr, i, j, k, l);
  CFP_PTR_TYPE cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.reference.ptr(cfpArrRef);

  assert_ptr_equal(cfpArrRef.array.object, cfpArrPtr.reference.array.object);
}


// #############
// cfp_ptr tests
// #############

static void
_catFunc3(given_, CFP_PTR_TYPE, _when_ref_expect_addressMatches)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i = 1, j = 2, k = 1, l = 1;
  CFP_PTR_TYPE cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i, j, k, l);
  CFP_REF_TYPE cfpArrRef = CFP_NAMESPACE.SUB_NAMESPACE.pointer.ref(cfpArrPtr);

  assert_ptr_equal(cfpArrPtr.reference.array.object, cfpArrRef.array.object);
}

static void
_catFunc3(given_, CFP_PTR_TYPE, _when_ref_at_expect_correct)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i = 1, j = 2, k = 1, l = 1;
  uint oi = 10;
  CFP_PTR_TYPE cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i, j, k, l);
  CFP_REF_TYPE cfpArrRef = CFP_NAMESPACE.SUB_NAMESPACE.pointer.ref_at(cfpArrPtr, oi);

  assert_int_equal(cfpArrPtr.reference.i + oi, cfpArrRef.i);
  assert_ptr_equal(cfpArrPtr.reference.array.object, cfpArrRef.array.object);
}

static void
_catFunc3(given_, CFP_PTR_TYPE, _when_lt_expect_less)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i1 = 1, i2 = 2;
  uint j1 = 1, j2 = 2;
  uint k1 = 1, k2 = 2;
  uint l1 = 1, l2 = 2;
  CFP_PTR_TYPE cfpArrPtrA = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i1, j1, k1, l1);
  CFP_PTR_TYPE cfpArrPtrB = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i2, j2, k2, l2);

  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.pointer.lt(cfpArrPtrA, cfpArrPtrB));
}

static void
_catFunc3(given_, CFP_PTR_TYPE, _when_gt_expect_more)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i1 = 1, i2 = 2;
  uint j1 = 1, j2 = 2;
  uint k1 = 1, k2 = 2;
  uint l1 = 1, l2 = 2;
  CFP_PTR_TYPE cfpArrPtrA = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i1, j1, k1, l1);
  CFP_PTR_TYPE cfpArrPtrB = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i2, j2, k2, l2);

  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.pointer.gt(cfpArrPtrB, cfpArrPtrA));
}

static void
_catFunc3(given_, CFP_PTR_TYPE, _when_leq_expect_less_or_eq)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i1 = 1, i2 = 2;
  uint j1 = 1, j2 = 2;
  uint k1 = 1, k2 = 2;
  uint l1 = 1, l2 = 2;
  CFP_PTR_TYPE cfpArrPtrA = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i1, j1, k1, l1);
  CFP_PTR_TYPE cfpArrPtrB = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i2, j2, k2, l2);

  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.pointer.leq(cfpArrPtrA, cfpArrPtrB));
  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.pointer.leq(cfpArrPtrA, cfpArrPtrA));
}

static void
_catFunc3(given_, CFP_PTR_TYPE, _when_geq_expect_more_or_eq)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i1 = 1, i2 = 2;
  uint j1 = 1, j2 = 2;
  uint k1 = 1, k2 = 2;
  uint l1 = 1, l2 = 2;
  CFP_PTR_TYPE cfpArrPtrA = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i1, j1, k1, l1);
  CFP_PTR_TYPE cfpArrPtrB = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i2, j2, k2, l2);

  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.pointer.geq(cfpArrPtrA, cfpArrPtrA));
  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.pointer.geq(cfpArrPtrB, cfpArrPtrA));
}

static void
_catFunc3(given_, CFP_PTR_TYPE, _when_eq_expect_same)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i1 = 1, j1 = 2, k1 = 1, l1 = 1;
  CFP_PTR_TYPE cfpArrPtrA = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i1, j1, k1, l1);

  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.pointer.eq(cfpArrPtrA, cfpArrPtrA));
}

static void
_catFunc3(given_, CFP_PTR_TYPE, _when_neq_expect_different)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i1 = 1, i2 = 2;
  uint j1 = 2, j2 = 1;
  uint k1 = 1, k2 = 2;
  uint l1 = 1, l2 = 2;
  CFP_PTR_TYPE cfpArrPtrA = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i1, j1, k1, l1);
  CFP_PTR_TYPE cfpArrPtrB = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i2, j2, k2, l2);

  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.pointer.neq(cfpArrPtrA, cfpArrPtrB));
}

static void
_catFunc3(given_, CFP_PTR_TYPE, _when_distance_expect_correct)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i1 = 1, i2 = 2, k1 = 1, l1 = 1;
  uint j1 = 2, j2 = 1, k2 = 2, l2 = 2;
  CFP_PTR_TYPE cfpArrPtrA = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i1, j1, k1, l1);
  CFP_PTR_TYPE cfpArrPtrB = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i2, j2, k2, l2);

  assert_int_equal((int)CFP_NAMESPACE.SUB_NAMESPACE.pointer.distance(cfpArrPtrA, cfpArrPtrB),
                   (int)(i2 +
                         j2*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) +
                         k2*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) +
                         l2*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr)) -
                   (int)(i1 +
                         j1*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) +
                         k1*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) + 
                         l1*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr)));
  assert_ptr_equal(cfpArrPtrA.reference.array.object, cfpArrPtrB.reference.array.object);
}

static void
_catFunc3(given_, CFP_PTR_TYPE, _when_next_expect_correct)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i = 1, j = 2, k = 1, l = 1, oi = 10;
  CFP_PTR_TYPE cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i, j, k, l);
  cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.pointer.next(cfpArrPtr, oi);

  uint idx = (int)(i + 
                   j*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) + 
                   k*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) + 
                   l*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr)) + 
                   oi;
  uint x = idx % CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr);
  uint y = ((idx - x) / CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)) %  CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr);
  uint z = ((idx - y * CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) - x) / (CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr)))            % CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr); 
  uint w = (idx - z * CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) - y * CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) - x)            / (CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr)); 

  assert_int_equal(cfpArrPtr.reference.i, x);
  assert_int_equal(cfpArrPtr.reference.j, y);
  assert_int_equal(cfpArrPtr.reference.k, z);
  assert_int_equal(cfpArrPtr.reference.l, w);
  assert_ptr_equal(cfpArrPtr.reference.array.object, CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i, j, k, l).reference.array.object);
}

static void
_catFunc3(given_, CFP_PTR_TYPE, _when_prev_expect_correct)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i = 8, j = 4, k = 1, l = 1, oi = 10;
  CFP_PTR_TYPE cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i, j, k, l);
  cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.pointer.prev(cfpArrPtr, oi);

  uint idx = (int)(i + 
                   j*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) + 
                   k*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) + 
                   l*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr)) - 
                   oi;
  uint x = idx % CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr);
  uint y = ((idx - x) / CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)) %  CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr);
  uint z = ((idx - y * CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) - x) / (CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr)))            % CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr); 
  uint w = (idx - z * CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) - y * CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) - x)            / (CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr)); 

  assert_int_equal(cfpArrPtr.reference.i, x);
  assert_int_equal(cfpArrPtr.reference.j, y);
  assert_int_equal(cfpArrPtr.reference.k, z);
  assert_int_equal(cfpArrPtr.reference.l, w);
  assert_ptr_equal(cfpArrPtr.reference.array.object, CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i, j, k, l).reference.array.object);
}

static void
_catFunc3(given_, CFP_PTR_TYPE, _when_inc_expect_correct)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i = 1, j = 2, k = 1, l = 1;
  CFP_PTR_TYPE cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i, j, k, l);
  cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.pointer.inc(cfpArrPtr);

  uint idx = (int)(i + 
                   j*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) + 
                   k*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) + 
                   l*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr)) + 
                   1;
  uint x = idx % CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr);
  uint y = ((idx - x) / CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)) %  CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr);
  uint z = ((idx - y * CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) - x) / (CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr)))            % CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr); 
  uint w = (idx - z * CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) - y * CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) - x)            / (CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr)); 

  assert_int_equal(cfpArrPtr.reference.i, x);
  assert_int_equal(cfpArrPtr.reference.j, y);
  assert_int_equal(cfpArrPtr.reference.k, z);
  assert_int_equal(cfpArrPtr.reference.l, w);
  assert_ptr_equal(cfpArrPtr.reference.array.object, CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i, j, k, l).reference.array.object);
}

static void
_catFunc3(given_, CFP_PTR_TYPE, _when_dec_expect_correct)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  uint i = 1, j = 2, k = 1, l = 1;
  CFP_PTR_TYPE cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i, j, k, l);
  cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.pointer.dec(cfpArrPtr);

  uint idx = (int)(i + 
                   j*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) + 
                   k*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) + 
                   l*CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr)*CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr)) - 
                   1;
  uint x = idx % CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr);
  uint y = ((idx - x) / CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr)) %  CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr);
  uint z = ((idx - y * CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) - x) / (CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr)))            % CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr); 
  uint w = (idx - z * CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) - y * CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) - x)            / (CFP_NAMESPACE.SUB_NAMESPACE.size_x(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_y(cfpArr) * CFP_NAMESPACE.SUB_NAMESPACE.size_z(cfpArr)); 

  assert_int_equal(cfpArrPtr.reference.i, x);
  assert_int_equal(cfpArrPtr.reference.j, y);
  assert_int_equal(cfpArrPtr.reference.k, z);
  assert_int_equal(cfpArrPtr.reference.l, w);
  assert_ptr_equal(cfpArrPtr.reference.array.object, CFP_NAMESPACE.SUB_NAMESPACE.ptr(cfpArr, i, j, k, l).reference.array.object);
}


// ##############
// cfp_iter tests
// ##############

static void
_catFunc3(given_, CFP_ITER_TYPE, _when_get_set_expect_correct)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;

  SCALAR val = 5;

  CFP_ITER_TYPE cfpArrIter = CFP_NAMESPACE.SUB_NAMESPACE.begin(cfpArr);
  CFP_NAMESPACE.SUB_NAMESPACE.iterator.set(cfpArrIter, val);

  assert_int_equal(CFP_NAMESPACE.SUB_NAMESPACE.iterator.get(cfpArrIter), val);
}

static void
_catFunc3(given_, CFP_ITER_TYPE, _when_ref_expect_correct)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;

  CFP_ITER_TYPE cfpArrIter = CFP_NAMESPACE.SUB_NAMESPACE.begin(cfpArr);
  CFP_REF_TYPE cfpArrRef = CFP_NAMESPACE.SUB_NAMESPACE.iterator.ref(cfpArrIter);

  assert_int_equal(cfpArrRef.i, 0);
  assert_ptr_equal(cfpArrRef.array.object, cfpArr.object);
}

static void
_catFunc3(given_, CFP_ITER_TYPE, _when_ptr_expect_correct)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;

  CFP_ITER_TYPE cfpArrIter = CFP_NAMESPACE.SUB_NAMESPACE.begin(cfpArr);
  CFP_PTR_TYPE cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.iterator.ptr(cfpArrIter);

  assert_int_equal(cfpArrPtr.reference.i, 0);
  assert_int_equal(cfpArrPtr.reference.j, 0);
  assert_int_equal(cfpArrPtr.reference.k, 0);
  assert_int_equal(cfpArrPtr.reference.l, 0);
  assert_ptr_equal(cfpArrPtr.reference.array.object, cfpArr.object);
}

static void
_catFunc3(given_, CFP_ITER_TYPE, _when_inc_expect_correct)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;

  CFP_ITER_TYPE cfpArrIter = CFP_NAMESPACE.SUB_NAMESPACE.begin(cfpArr);
  cfpArrIter = CFP_NAMESPACE.SUB_NAMESPACE.iterator.inc(cfpArrIter);

  assert_int_equal(cfpArrIter.i, 1);
  assert_ptr_equal(cfpArrIter.array.object, cfpArr.object);
}

static void
_catFunc3(given_, CFP_ITER_TYPE, _when_iterate_touch_all)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;
  CFP_ITER_TYPE cfpArrIter;
  CFP_PTR_TYPE cfpArrPtr;

  SCALAR val = -1;

  for (cfpArrIter = CFP_NAMESPACE.SUB_NAMESPACE.begin(cfpArr);
       CFP_NAMESPACE.SUB_NAMESPACE.iterator.neq(cfpArrIter, CFP_NAMESPACE.SUB_NAMESPACE.end(cfpArr));
       cfpArrIter = CFP_NAMESPACE.SUB_NAMESPACE.iterator.inc(cfpArrIter))
  {
    CFP_NAMESPACE.SUB_NAMESPACE.iterator.set(cfpArrIter, val);
  }

  for (cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.ptr_flat(cfpArr, 0);
       CFP_NAMESPACE.SUB_NAMESPACE.pointer.leq(cfpArrPtr, CFP_NAMESPACE.SUB_NAMESPACE.ptr_flat(cfpArr, CFP_NAMESPACE.SUB_NAMESPACE.size(cfpArr) - 1));
       cfpArrPtr = CFP_NAMESPACE.SUB_NAMESPACE.pointer.inc(cfpArrPtr))
  {
    assert_true(CFP_NAMESPACE.SUB_NAMESPACE.pointer.get(cfpArrPtr) - val < 1e-12);
    assert_true(CFP_NAMESPACE.SUB_NAMESPACE.pointer.get(cfpArrPtr) - val > -1e-12);
  }
}

static void
_catFunc3(given_, CFP_ITER_TYPE, _when_compare_expect_valid)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;

  CFP_ITER_TYPE cfpArrIter1 = CFP_NAMESPACE.SUB_NAMESPACE.begin(cfpArr);
  CFP_ITER_TYPE cfpArrIter2 = CFP_NAMESPACE.SUB_NAMESPACE.begin(cfpArr);
  cfpArrIter2.i += 4;

  assert_true(CFP_NAMESPACE.SUB_NAMESPACE.iterator.eq(cfpArrIter1, cfpArrIter1));
  assert_false(CFP_NAMESPACE.SUB_NAMESPACE.iterator.eq(cfpArrIter1, cfpArrIter2));
}

static void
_catFunc3(given_, CFP_ITER_TYPE, _when_get_index_expect_correct)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE cfpArr = bundle->cfpArr;

  CFP_ITER_TYPE cfpArrIter = CFP_NAMESPACE.SUB_NAMESPACE.begin(cfpArr);
  cfpArrIter.i = 1;
  cfpArrIter.j = 3;
  cfpArrIter.k = 2;
  cfpArrIter.l = 1;

  int i_idx = CFP_NAMESPACE.SUB_NAMESPACE.iterator.i(cfpArrIter);
  int j_idx = CFP_NAMESPACE.SUB_NAMESPACE.iterator.j(cfpArrIter);
  int k_idx = CFP_NAMESPACE.SUB_NAMESPACE.iterator.k(cfpArrIter);
  int l_idx = CFP_NAMESPACE.SUB_NAMESPACE.iterator.l(cfpArrIter);

  assert_int_equal(i_idx, 1);
  assert_int_equal(j_idx, 3);
  assert_int_equal(k_idx, 2);
  assert_int_equal(l_idx, 1);
}