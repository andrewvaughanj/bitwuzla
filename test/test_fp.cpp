/*  Bitwuzla: Satisfiability Modulo Theories (SMT) solver.
 *
 *  Copyright (C) 2019-2020 Aina Niemetz
 *
 *  This file is part of Bitwuzla.
 *  See COPYING for more information on using this software.
 */

#include <bitset>

#include "test.h"

extern "C" {
#include "bzlabv.h"
#include "bzlaexp.h"
#include "bzlafp.h"
}

class TestFp : public TestBitwuzla
{
};

class TestFpInternal : public TestBzla
{
 protected:
  void SetUp() override
  {
    TestBzla::SetUp();

    d_f16  = bzla_sort_fp(d_bzla, 5, 11);
    d_f32  = bzla_sort_fp(d_bzla, 8, 24);
    d_f64  = bzla_sort_fp(d_bzla, 11, 53);
    d_f128 = bzla_sort_fp(d_bzla, 15, 113);
  }

  void TearDown() override
  {
    bzla_sort_release(d_bzla, d_f16);
    bzla_sort_release(d_bzla, d_f32);
    bzla_sort_release(d_bzla, d_f64);
    bzla_sort_release(d_bzla, d_f128);
    TestBzla::TearDown();
  }

  void test_fp_as_bv(std::string sign, std::string exp, std::string sig)
  {
    assert(sign.size() == 1);

    BzlaSortId sort_fp, sort_sign, sort_exp, sort_sig;
    BzlaNode *node_fp, *node_bv_sign, *node_bv_exp, *node_bv_sig;
    BzlaBitVector *bv_sign, *bv_exp, *bv_sig;
    BzlaBitVector *res_sign, *res_exp, *res_sig;
    BzlaFloatingPoint *fp;
    uint32_t bw_sig, bw_exp;

    bv_sign = bzla_bv_char_to_bv(d_bzla->mm, sign.c_str());
    bv_exp  = bzla_bv_char_to_bv(d_bzla->mm, exp.c_str());
    bv_sig  = bzla_bv_char_to_bv(d_bzla->mm, sig.c_str());

    bw_exp = exp.size();
    bw_sig = sig.size() + 1;

    sort_sign = bzla_sort_bv(d_bzla, 1);
    sort_exp  = bzla_sort_bv(d_bzla, bw_exp);
    sort_sig  = bzla_sort_bv(d_bzla, bw_sig);

    node_bv_sign = bzla_exp_bv_const(d_bzla, bv_sign);
    node_bv_exp  = bzla_exp_bv_const(d_bzla, bv_exp);
    node_bv_sig  = bzla_exp_bv_const(d_bzla, bv_sig);

    sort_fp = bzla_sort_fp(d_bzla, bw_exp, bw_sig);
    node_fp = bzla_exp_fp_const(d_bzla, node_bv_sign, node_bv_exp, node_bv_sig);

    fp = bzla_fp_get_fp(node_fp);
    bzla_fp_as_bv(d_bzla, fp, &res_sign, &res_exp, &res_sig);
    if (bzla_fp_is_nan(d_bzla, fp))
    {
      BzlaFloatingPoint *nan = bzla_fp_nan(d_bzla, sort_fp);
      ASSERT_EQ(bzla_fp_compare(fp, nan), 0);
      bzla_fp_free(d_bzla, nan);
    }
    else
    {
      ASSERT_EQ(bzla_bv_compare(bv_sign, res_sign), 0);
      ASSERT_EQ(bzla_bv_compare(bv_exp, res_exp), 0);
      ASSERT_EQ(bzla_bv_compare(bv_sig, res_sig), 0);
    }

    bzla_node_release(d_bzla, node_fp);
    bzla_sort_release(d_bzla, sort_fp);
    bzla_node_release(d_bzla, node_bv_sig);
    bzla_node_release(d_bzla, node_bv_exp);
    bzla_node_release(d_bzla, node_bv_sign);
    bzla_sort_release(d_bzla, sort_sig);
    bzla_sort_release(d_bzla, sort_exp);
    bzla_sort_release(d_bzla, sort_sign);
    bzla_bv_free(d_bzla->mm, bv_sig);
    bzla_bv_free(d_bzla->mm, bv_exp);
    bzla_bv_free(d_bzla->mm, bv_sign);
    bzla_bv_free(d_bzla->mm, res_sig);
    bzla_bv_free(d_bzla->mm, res_exp);
    bzla_bv_free(d_bzla->mm, res_sign);
  }

  void test_to_fp_from_real(BzlaRoundingMode rm,
                            std::vector<std::vector<const char *>> &expected)
  {
    BzlaMemMgr *mm = d_bzla->mm;
    BzlaFloatingPoint *fp;
    BzlaBitVector *sign, *exp, *sig;
    char *sign_str, *exp_str, *sig_str;

    assert(d_constants.size() == expected.size());
    for (size_t i = 0, n = d_constants.size(); i < n; ++i)
    {
      fp = bzla_fp_convert_from_real(d_bzla, d_f16, rm, d_constants[i]);
      bzla_fp_as_bv(d_bzla, fp, &sign, &exp, &sig);
      sign_str = bzla_bv_to_char(mm, sign);
      exp_str  = bzla_bv_to_char(mm, exp);
      sig_str  = bzla_bv_to_char(mm, sig);
      ASSERT_EQ(strcmp(sign_str, expected[i][0]), 0);
      ASSERT_EQ(strcmp(exp_str, expected[i][1]), 0);
      ASSERT_EQ(strcmp(sig_str, expected[i][2]), 0);

      bzla_mem_freestr(mm, sign_str);
      bzla_mem_freestr(mm, exp_str);
      bzla_mem_freestr(mm, sig_str);
      bzla_bv_free(mm, sign);
      bzla_bv_free(mm, exp);
      bzla_bv_free(mm, sig);
      bzla_fp_free(d_bzla, fp);
    }
  }

  std::vector<const char *> d_constants = {
      "00",
      "0.0",
      "0.0117749388721091",
      "0.01745240643728",
      "0.03489949670250",
      "0.05233595624294",
      "0.0544",
      "0.06975647374412",
      "0.06975647374413",
      "0.08715574274766",
      "0.10452846326765",
      "0.12186934340515",
      "0.13917310096007",
      "0.15643446504023",
      "0.17364817766693",
      "0.19080899537654",
      "0.2",
      "0.20791169081776",
      "0.22495105434386",
      "0.22495105434387",
      "0.24192189559967",
      "0.244",
      "0.25881904510252",
      "0.27563735581700",
      "0.29237170472274",
      "0.3",
      "0.30901699437495",
      "0.32556815445716",
      "0.34202014332567",
      "0.35836794954530",
      "0.37460659341591",
      "0.39073112848927",
      "0.4",
      "0.403",
      "0.40673664307580",
      "0.42261826174070",
      "0.4344376",
      "0.43837114678908",
      "0.45399049973955",
      "0.4677826",
      "0.46947156278589",
      "0.48480962024634",
      "0.5",
      "0.50000000000000",
      "0.51503807491005",
      "0.5179422053046",
      "0.52991926423320",
      "0.52991926423321",
      "0.54463903501503",
      "0.5522073405779",
      "0.55919290347075",
      "0.57357643635105",
      "0.58778525229247",
      "0.60181502315205",
      "0.61566147532566",
      "0.62932039104984",
      "0.64278760968654",
      "0.65605902899051",
      "0.66913060635886",
      "0.6740477",
      "0.68199836006250",
      "0.69465837045900",
      "0.7",
      "0.70710678118655",
      "0.71933980033865",
      "0.73135370161917",
      "0.74314482547739",
      "0.75470958022277",
      "0.76604444311898",
      "0.7700725",
      "0.77714596145697",
      "0.78801075360672",
      "0.79863551004729",
      "0.8",
      "0.80901699437495",
      "0.81915204428899",
      "0.820939679242",
      "0.82903757255504",
      "0.83867056794542",
      "0.84804809615643",
      "0.85716730070211",
      "0.86602540378444",
      "0.87461970713940",
      "0.88294759285893",
      "0.89100652418837",
      "0.89879404629917",
      "0.90630778703665",
      "0.91354545764260",
      "0.92050485345244",
      "0.92718385456679",
      "0.93358042649720",
      "0.93969262078591",
      "0.94551857559932",
      "0.95105651629515",
      "0.95630475596304",
      "0.96126169593832",
      "0.96592582628907",
      "0.97029572627600",
      "0.97437006478524",
      "0.97814760073381",
      "0.98",
      "0.98162718344766",
      "0.98480775301221",
      "0.98768834059514",
      "0.99026806874157",
      "0.99254615164132",
      "0.99452189536827",
      "0.99619469809175",
      "0.99756405025982",
      "0.99862953475457",
      "0.99939082701910",
      "0.99984769515639",
      "1.0",
      "1.00000000000000",
      "1.1",
      "1.3",
      "1.4",
      "1.470767736573",
      "1.5",
      "1.5419",
      "1.633101801841",
      "1.7",
      "1.742319554830",
      "10.0",
      "100.0",
      "10000.0",
      "1000000.0",
      "10000000000.0",
      "1000000000000000000000000000000.0",
      "100000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000.0",
      "11.0",
      "111.0",
      "1130.0",
      "120.0",
      "121.0",
      "15.0",
      "16.0",
      "179000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000.0",
      "180.0",
      "2.0",
      "20.0",
      "24.0",
      "256.0",
      "3.0",
      "30.0",
      "3000.0",
      "33096.0",
      "33554432.0",
      "360.0",
      "5.0",
      "5040.0",
      "6.0",
      "7.0",
      "720.0",
      "77617.0",
      "8.0",
      "85.0",
      "9.0",
      "180",
      "2",
      "20",
      "24",
      "256",
      "3",
      "30",
      "3000",
      "33096",
      "33554432",
      "360",
      "5",
      "5040",
      "6",
      "7",
      "720",
      "77617",
      "8",
      "85",
      "9",
  };

  BzlaSortId d_f16;
  BzlaSortId d_f32;
  BzlaSortId d_f64;
  BzlaSortId d_f128;
};

TEST_F(TestFp, sort_fp)
{
  BitwuzlaSort *f16, *f32, *f64, *f128;

  f16 = bitwuzla_mk_fp_sort(d_bzla, 5, 11);
  ASSERT_TRUE(bitwuzla_sort_is_fp(f16));

  f32 = bitwuzla_mk_fp_sort(d_bzla, 8, 24);
  ASSERT_TRUE(bitwuzla_sort_is_fp(f32));

  f64 = bitwuzla_mk_fp_sort(d_bzla, 11, 53);
  ASSERT_TRUE(bitwuzla_sort_is_fp(f64));

  f128 = bitwuzla_mk_fp_sort(d_bzla, 15, 113);
  ASSERT_TRUE(bitwuzla_sort_is_fp(f128));
}

TEST_F(TestFp, sort_rm)
{
  BitwuzlaSort *rm;

  rm = bitwuzla_mk_rm_sort(d_bzla);
  ASSERT_TRUE(bitwuzla_sort_is_rm(rm));
}

TEST_F(TestFpInternal, fp_as_bv)
{
  for (uint64_t i = 0; i < (1u << 5); ++i)
  {
    std::stringstream ss;
    std::string exp = std::bitset<5>(i).to_string();
    for (uint64_t j = 0; j < (1u << 10); ++j)
    {
      std::stringstream ss;
      std::string sig = std::bitset<10>(j).to_string();
      test_fp_as_bv("0", exp.c_str(), sig.c_str());
      test_fp_as_bv("1", exp.c_str(), sig.c_str());
    }
  }
}

TEST_F(TestFpInternal, fp_is_const)
{
  BzlaSortId sorts[4] = {d_f16, d_f32, d_f64, d_f128};

  for (uint32_t i = 0; i < 4; i++)
  {
    BzlaNode *pzero = bzla_exp_fp_pos_zero(d_bzla, sorts[i]);
    ASSERT_TRUE(bzla_node_is_fp_const_pos_zero(d_bzla, pzero));
    ASSERT_TRUE(!bzla_node_is_fp_const_neg_zero(d_bzla, pzero));
    ASSERT_TRUE(!bzla_node_is_fp_const_pos_inf(d_bzla, pzero));
    ASSERT_TRUE(!bzla_node_is_fp_const_neg_inf(d_bzla, pzero));
    ASSERT_TRUE(!bzla_node_is_fp_const_nan(d_bzla, pzero));
    bzla_node_release(d_bzla, pzero);

    BzlaNode *nzero = bzla_exp_fp_neg_zero(d_bzla, sorts[i]);
    ASSERT_TRUE(!bzla_node_is_fp_const_pos_zero(d_bzla, nzero));
    ASSERT_TRUE(bzla_node_is_fp_const_neg_zero(d_bzla, nzero));
    ASSERT_TRUE(!bzla_node_is_fp_const_pos_inf(d_bzla, nzero));
    ASSERT_TRUE(!bzla_node_is_fp_const_neg_inf(d_bzla, nzero));
    ASSERT_TRUE(!bzla_node_is_fp_const_nan(d_bzla, nzero));
    bzla_node_release(d_bzla, nzero);

    BzlaNode *pinf = bzla_exp_fp_pos_inf(d_bzla, sorts[i]);
    ASSERT_TRUE(!bzla_node_is_fp_const_pos_zero(d_bzla, pinf));
    ASSERT_TRUE(!bzla_node_is_fp_const_neg_zero(d_bzla, pinf));
    ASSERT_TRUE(bzla_node_is_fp_const_pos_inf(d_bzla, pinf));
    ASSERT_TRUE(!bzla_node_is_fp_const_neg_inf(d_bzla, pinf));
    ASSERT_TRUE(!bzla_node_is_fp_const_nan(d_bzla, pinf));
    bzla_node_release(d_bzla, pinf);

    BzlaNode *ninf = bzla_exp_fp_neg_inf(d_bzla, sorts[i]);
    ASSERT_TRUE(!bzla_node_is_fp_const_pos_zero(d_bzla, ninf));
    ASSERT_TRUE(!bzla_node_is_fp_const_neg_zero(d_bzla, ninf));
    ASSERT_TRUE(!bzla_node_is_fp_const_pos_inf(d_bzla, ninf));
    ASSERT_TRUE(bzla_node_is_fp_const_neg_inf(d_bzla, ninf));
    ASSERT_TRUE(!bzla_node_is_fp_const_nan(d_bzla, ninf));
    bzla_node_release(d_bzla, ninf);

    BzlaNode *nan = bzla_exp_fp_nan(d_bzla, sorts[i]);
    ASSERT_TRUE(!bzla_node_is_fp_const_pos_zero(d_bzla, nan));
    ASSERT_TRUE(!bzla_node_is_fp_const_neg_zero(d_bzla, nan));
    ASSERT_TRUE(!bzla_node_is_fp_const_pos_inf(d_bzla, nan));
    ASSERT_TRUE(!bzla_node_is_fp_const_neg_inf(d_bzla, nan));
    ASSERT_TRUE(bzla_node_is_fp_const_nan(d_bzla, nan));
    bzla_node_release(d_bzla, nan);
  }
}

TEST_F(TestFpInternal, fp_from_real_dec_str_rna)
{
  std::vector<std::vector<const char *>> expected = {
      {"0", "00000", "0000000000"}, {"0", "00000", "0000000000"},
      {"0", "01000", "1000000111"}, {"0", "01001", "0001111000"},
      {"0", "01010", "0001111000"}, {"0", "01010", "1010110011"},
      {"0", "01010", "1011110111"}, {"0", "01011", "0001110111"},
      {"0", "01011", "0001110111"}, {"0", "01011", "0110010100"},
      {"0", "01011", "1010110001"}, {"0", "01011", "1111001101"},
      {"0", "01100", "0001110100"}, {"0", "01100", "0100000010"},
      {"0", "01100", "0110001111"}, {"0", "01100", "1000011011"},
      {"0", "01100", "1001100110"}, {"0", "01100", "1010100111"},
      {"0", "01100", "1100110011"}, {"0", "01100", "1100110011"},
      {"0", "01100", "1110111110"}, {"0", "01100", "1111001111"},
      {"0", "01101", "0000100100"}, {"0", "01101", "0001101001"},
      {"0", "01101", "0010101110"}, {"0", "01101", "0011001101"},
      {"0", "01101", "0011110010"}, {"0", "01101", "0100110110"},
      {"0", "01101", "0101111001"}, {"0", "01101", "0110111100"},
      {"0", "01101", "0111111110"}, {"0", "01101", "1001000000"},
      {"0", "01101", "1001100110"}, {"0", "01101", "1001110011"},
      {"0", "01101", "1010000010"}, {"0", "01101", "1011000011"},
      {"0", "01101", "1011110011"}, {"0", "01101", "1100000100"},
      {"0", "01101", "1101000100"}, {"0", "01101", "1101111100"},
      {"0", "01101", "1110000011"}, {"0", "01101", "1111000010"},
      {"0", "01110", "0000000000"}, {"0", "01110", "0000000000"},
      {"0", "01110", "0000011111"}, {"0", "01110", "0000100101"},
      {"0", "01110", "0000111101"}, {"0", "01110", "0000111101"},
      {"0", "01110", "0001011011"}, {"0", "01110", "0001101011"},
      {"0", "01110", "0001111001"}, {"0", "01110", "0010010111"},
      {"0", "01110", "0010110100"}, {"0", "01110", "0011010001"},
      {"0", "01110", "0011101101"}, {"0", "01110", "0100001001"},
      {"0", "01110", "0100100100"}, {"0", "01110", "0101000000"},
      {"0", "01110", "0101011010"}, {"0", "01110", "0101100100"},
      {"0", "01110", "0101110101"}, {"0", "01110", "0110001111"},
      {"0", "01110", "0110011010"}, {"0", "01110", "0110101000"},
      {"0", "01110", "0111000001"}, {"0", "01110", "0111011010"},
      {"0", "01110", "0111110010"}, {"0", "01110", "1000001010"},
      {"0", "01110", "1000100001"}, {"0", "01110", "1000101001"},
      {"0", "01110", "1000111000"}, {"0", "01110", "1001001110"},
      {"0", "01110", "1001100100"}, {"0", "01110", "1001100110"},
      {"0", "01110", "1001111001"}, {"0", "01110", "1010001110"},
      {"0", "01110", "1010010001"}, {"0", "01110", "1010100010"},
      {"0", "01110", "1010110110"}, {"0", "01110", "1011001001"},
      {"0", "01110", "1011011011"}, {"0", "01110", "1011101110"},
      {"0", "01110", "1011111111"}, {"0", "01110", "1100010000"},
      {"0", "01110", "1100100001"}, {"0", "01110", "1100110001"},
      {"0", "01110", "1101000000"}, {"0", "01110", "1101001111"},
      {"0", "01110", "1101011101"}, {"0", "01110", "1101101011"},
      {"0", "01110", "1101111000"}, {"0", "01110", "1110000100"},
      {"0", "01110", "1110010000"}, {"0", "01110", "1110011100"},
      {"0", "01110", "1110100111"}, {"0", "01110", "1110110001"},
      {"0", "01110", "1110111010"}, {"0", "01110", "1111000011"},
      {"0", "01110", "1111001100"}, {"0", "01110", "1111010011"},
      {"0", "01110", "1111010111"}, {"0", "01110", "1111011010"},
      {"0", "01110", "1111100001"}, {"0", "01110", "1111100111"},
      {"0", "01110", "1111101100"}, {"0", "01110", "1111110001"},
      {"0", "01110", "1111110101"}, {"0", "01110", "1111111000"},
      {"0", "01110", "1111111011"}, {"0", "01110", "1111111101"},
      {"0", "01110", "1111111111"}, {"0", "01111", "0000000000"},
      {"0", "01111", "0000000000"}, {"0", "01111", "0000000000"},
      {"0", "01111", "0001100110"}, {"0", "01111", "0100110011"},
      {"0", "01111", "0110011010"}, {"0", "01111", "0111100010"},
      {"0", "01111", "1000000000"}, {"0", "01111", "1000101011"},
      {"0", "01111", "1010001000"}, {"0", "01111", "1011001101"},
      {"0", "01111", "1011111000"}, {"0", "10010", "0100000000"},
      {"0", "10101", "1001000000"}, {"0", "11100", "0011100010"},
      {"0", "11111", "0000000000"}, {"0", "11111", "0000000000"},
      {"0", "11111", "0000000000"}, {"0", "11111", "0000000000"},
      {"0", "10010", "0110000000"}, {"0", "10101", "1011110000"},
      {"0", "11001", "0001101010"}, {"0", "10101", "1110000000"},
      {"0", "10101", "1110010000"}, {"0", "10010", "1110000000"},
      {"0", "10011", "0000000000"}, {"0", "11111", "0000000000"},
      {"0", "10110", "0110100000"}, {"0", "10000", "0000000000"},
      {"0", "10011", "0100000000"}, {"0", "10011", "1000000000"},
      {"0", "10111", "0000000000"}, {"0", "10000", "1000000000"},
      {"0", "10011", "1110000000"}, {"0", "11010", "0111011100"},
      {"0", "11110", "0000001010"}, {"0", "11111", "0000000000"},
      {"0", "10111", "0110100000"}, {"0", "10001", "0100000000"},
      {"0", "11011", "0011101100"}, {"0", "10001", "1000000000"},
      {"0", "10001", "1100000000"}, {"0", "11000", "0110100000"},
      {"0", "11111", "0000000000"}, {"0", "10010", "0000000000"},
      {"0", "10101", "0101010000"}, {"0", "10010", "0010000000"},
      {"0", "10110", "0110100000"}, {"0", "10000", "0000000000"},
      {"0", "10011", "0100000000"}, {"0", "10011", "1000000000"},
      {"0", "10111", "0000000000"}, {"0", "10000", "1000000000"},
      {"0", "10011", "1110000000"}, {"0", "11010", "0111011100"},
      {"0", "11110", "0000001010"}, {"0", "11111", "0000000000"},
      {"0", "10111", "0110100000"}, {"0", "10001", "0100000000"},
      {"0", "11011", "0011101100"}, {"0", "10001", "1000000000"},
      {"0", "10001", "1100000000"}, {"0", "11000", "0110100000"},
      {"0", "11111", "0000000000"}, {"0", "10010", "0000000000"},
      {"0", "10101", "0101010000"}, {"0", "10010", "0010000000"},
  };

  test_to_fp_from_real(BzlaRoundingMode::BZLA_RM_RNA, expected);
}
