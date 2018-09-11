#include "src/encode2f.c"

#include "constants/2dFloat.h"
#include "utils/hash32.h"
#include "cudaExecBase.c"

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(when_seededRandomSmoothDataGenerated_expect_ChecksumMatches),

    /* strided */
    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatReversedArray_when_ZfpCompressFixedRate_expect_BitstreamChecksumMatches, setupReversed, teardown),
    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatReversedArray_when_ZfpDecompressFixedRate_expect_ArrayChecksumMatches, setupReversed, teardown),
    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatInterleavedArray_when_ZfpCompressFixedRate_expect_BitstreamChecksumMatches, setupInterleaved, teardown),
    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatInterleavedArray_when_ZfpDecompressFixedRate_expect_ArrayChecksumMatches, setupInterleaved, teardown),
    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatPermutedArray_when_ZfpCompressFixedRate_expect_BitstreamChecksumMatches, setupPermuted, teardown),
    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatPermutedArray_when_ZfpDecompressFixedRate_expect_ArrayChecksumMatches, setupPermuted, teardown),

    /* fixed-rate */
    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatArray_when_ZfpCompressFixedRate_expect_BitstreamChecksumMatches, setupFixedRate0Param, teardown),
    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatArray_when_ZfpDecompressFixedRate_expect_ArrayChecksumMatches, setupFixedRate0Param, teardown),

    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatArray_when_ZfpCompressFixedRate_expect_BitstreamChecksumMatches, setupFixedRate1Param, teardown),
    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatArray_when_ZfpDecompressFixedRate_expect_ArrayChecksumMatches, setupFixedRate1Param, teardown),

    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatArray_when_ZfpCompressFixedRate_expect_BitstreamChecksumMatches, setupFixedRate2Param, teardown),
    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatArray_when_ZfpDecompressFixedRate_expect_ArrayChecksumMatches, setupFixedRate2Param, teardown),

    /* non fixed-rate */
    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatArray_when_ZfpCompressNonFixedRate_expect_BitstreamUntouchedAndReturnsZero, setupFixedPrec1Param, teardown),
    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatArray_when_ZfpDecompressNonFixedRate_expect_BitstreamUntouchedAndReturnsZero, setupFixedPrec1Param, teardown),

    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatArray_when_ZfpCompressNonFixedRate_expect_BitstreamUntouchedAndReturnsZero, setupFixedAcc1Param, teardown),
    cmocka_unit_test_setup_teardown(given_Cuda_2dFloatArray_when_ZfpDecompressNonFixedRate_expect_BitstreamUntouchedAndReturnsZero, setupFixedAcc1Param, teardown),
  };

  return cmocka_run_group_tests(tests, setupRandomData, teardownRandomData);
}
