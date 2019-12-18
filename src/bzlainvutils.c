/*  Boolector: Satisfiability Modulo Theories (SMT) solver.
 *
 *  Copyright (C) 2018 Mathias Preiner.
 *  Copyright (C) 2018-2019 Aina Niemetz.
 *
 *  This file is part of Boolector.
 *  See COPYING for more information on using this software.
 *
 *  Bit-vector operator invertibility checks based on [1].
 *
 *  [1] Aina Niemetz, Mathias Preiner, Andrew Reynolds, Clark Barrett, Cesare
 *      Tinelli: Solving Quantified Bit-Vectors Using Invertibility Conditions.
 *      CAV (2) 2018: 236-255
 *
 */

#include "bzlainvutils.h"

#include <assert.h>

/* -------------------------------------------------------------------------- */
/* Check invertibility without considering constant bits in x.                */
/* -------------------------------------------------------------------------- */

/**
 * Check invertibility condition (without considering const bits in x) for:
 *
 * x + s = t
 * s + x = t
 *
 * IC: true
 */
bool
bzla_is_inv_add(BzlaMemMgr *mm,
                const BzlaBvDomain *x,
                const BzlaBitVector *t,
                const BzlaBitVector *s,
                uint32_t pos_x)
{
  assert(mm);
  assert(t);
  assert(s);
  (void) mm;
  (void) x;
  (void) t;
  (void) s;
  (void) pos_x;
  return true;
}

/**
 * Check invertibility condition (without considering const bits in x) for:
 *
 * x & s = t
 * s & x = t
 *
 * IC: t & s = t
 */
bool
bzla_is_inv_and(BzlaMemMgr *mm,
                const BzlaBvDomain *x,
                const BzlaBitVector *t,
                const BzlaBitVector *s,
                uint32_t pos_x)
{
  assert(mm);
  assert(t);
  assert(s);
  (void) x;
  (void) pos_x;

  BzlaBitVector *t_and_s = bzla_bv_and(mm, t, s);
  BzlaBitVector *eq_t    = bzla_bv_eq(mm, t_and_s, t);
  bool res               = bzla_bv_is_true(eq_t);
  bzla_bv_free(mm, t_and_s);
  bzla_bv_free(mm, eq_t);
  return res;
}

/**
 * Check invertibility condition (without considering const bits in x) for:
 *
 * pos_x = 0:
 * x o s = t
 * IC: s = t[bw(s) - 1 : 0]
 *
 * pos_x = 1:
 * s o x = t
 * IC: s = t[bw(t) - 1 : bw(t) - bw(s)]
 */
bool
bzla_is_inv_concat(BzlaMemMgr *mm,
                   const BzlaBvDomain *x,
                   const BzlaBitVector *t,
                   const BzlaBitVector *s,
                   uint32_t pos_x)
{
  assert(mm);
  assert(t);
  assert(s);
  (void) x;

  BzlaBitVector *slice;
  bool res;
  uint32_t bw_s, bw_t;

  bw_s = bzla_bv_get_width(s);
  bw_t = bzla_bv_get_width(t);
  if (pos_x == 0)
  {
    slice = bzla_bv_slice(mm, t, bw_s - 1, 0);
  }
  else
  {
    assert(pos_x == 1);
    slice = bzla_bv_slice(mm, t, bw_t - 1, bw_t - bw_s);
  }
  BzlaBitVector *s_eq_slice = bzla_bv_eq(mm, s, slice);
  res                       = bzla_bv_is_true(s_eq_slice);
  bzla_bv_free(mm, slice);
  bzla_bv_free(mm, s_eq_slice);
  return res;
}

/**
 * Check invertibility condition (without considering const bits in x) for:
 *
 * x == s = t
 * s == x = t
 *
 * IC: true
 */
bool
bzla_is_inv_eq(BzlaMemMgr *mm,
               const BzlaBvDomain *x,
               const BzlaBitVector *t,
               const BzlaBitVector *s,
               uint32_t pos_x)
{
  assert(mm);
  assert(t);
  assert(s);
  (void) mm;
  (void) x;
  (void) t;
  (void) s;
  (void) pos_x;
  return true;
}

/**
 * Check invertibility condition (without considering const bits in x) for:
 *
 * x * s = t
 * s * x = t
 *
 * IC: (-s | s ) & t = t
 */
bool
bzla_is_inv_mul(BzlaMemMgr *mm,
                const BzlaBvDomain *x,
                const BzlaBitVector *t,
                const BzlaBitVector *s,
                uint32_t pos_x)
{
  assert(mm);
  assert(t);
  assert(s);
  (void) x;
  (void) pos_x;

  BzlaBitVector *neg_s      = bzla_bv_neg(mm, s);
  BzlaBitVector *neg_s_or_s = bzla_bv_or(mm, neg_s, s);
  BzlaBitVector *and_t      = bzla_bv_and(mm, neg_s_or_s, t);
  BzlaBitVector *eq_t       = bzla_bv_eq(mm, and_t, t);
  bool res                  = bzla_bv_is_true(eq_t);
  bzla_bv_free(mm, neg_s);
  bzla_bv_free(mm, neg_s_or_s);
  bzla_bv_free(mm, and_t);
  bzla_bv_free(mm, eq_t);
  return res;
}

/**
 * Check invertibility condition (without considering const bits in x) for:
 *
 * x | s = t
 * s | x = t
 *
 * IC: t | s = t
 */
bool
bzla_is_inv_or(BzlaMemMgr *mm,
               const BzlaBvDomain *x,
               const BzlaBitVector *t,
               const BzlaBitVector *s,
               uint32_t pos_x)
{
  assert(mm);
  assert(t);
  assert(s);
  (void) x;
  (void) pos_x;

  BzlaBitVector *t_or_s = bzla_bv_or(mm, t, s);
  BzlaBitVector *eq_t   = bzla_bv_eq(mm, t_or_s, t);
  bool res              = bzla_bv_is_true(eq_t);
  bzla_bv_free(mm, t_or_s);
  bzla_bv_free(mm, eq_t);
  return res;
}

/**
 * Check invertibility condition (without considering const bits in x) for:
 *
 * pos_x = 0:
 * x << s = t
 * IC: (t >> s) << s = t
 *
 * pos_x = 1:
 * s << x = t
 * IC: (\/ s << i = t)  i = 0..bw(s)-1
 */
bool
bzla_is_inv_sll(BzlaMemMgr *mm,
                const BzlaBvDomain *x,
                const BzlaBitVector *t,
                const BzlaBitVector *s,
                uint32_t pos_x)
{
  assert(mm);
  assert(t);
  assert(s);
  (void) x;

  bool res;
  if (pos_x == 0)
  {
    BzlaBitVector *t_srl_s = bzla_bv_srl(mm, t, s);
    BzlaBitVector *sll_s   = bzla_bv_sll(mm, t_srl_s, s);
    BzlaBitVector *eq_t    = bzla_bv_eq(mm, sll_s, t);
    res                    = bzla_bv_is_true(eq_t);
    bzla_bv_free(mm, t_srl_s);
    bzla_bv_free(mm, sll_s);
    bzla_bv_free(mm, eq_t);
  }
  else
  {
    assert(pos_x == 1);
    res = false;
    for (uint32_t i = 0, bw_s = bzla_bv_get_width(s); i <= bw_s && !res; i++)
    {
      BzlaBitVector *bv_i    = bzla_bv_uint64_to_bv(mm, i, bw_s);
      BzlaBitVector *s_sll_i = bzla_bv_sll(mm, s, bv_i);
      BzlaBitVector *eq_t    = bzla_bv_eq(mm, s_sll_i, t);
      res                    = bzla_bv_is_true(eq_t);

      bzla_bv_free(mm, bv_i);
      bzla_bv_free(mm, s_sll_i);
      bzla_bv_free(mm, eq_t);
    }
  }
  return res;
}

/**
 * Check invertibility condition (without considering const bits in x) for:
 *
 * pos_x = 0:
 * x >>a s = t
 * IC: (s < bw(s) -> (t << s) >>a s = t) /\ (s >= bw(s) -> (t = ~0 \/ t = 0))
 *
 * pos_x = 1:
 * s <<a x = t
 * IC: (\/ s >>a i = t)  i = 0..bw(s)-1
 */
bool
bzla_is_inv_sra(BzlaMemMgr *mm,
                const BzlaBvDomain *x,
                const BzlaBitVector *t,
                const BzlaBitVector *s,
                uint32_t pos_x)
{
  assert(mm);
  assert(t);
  assert(s);
  (void) x;

  bool res;
  uint32_t bw_s = bzla_bv_get_width(s);

  if (pos_x == 0)
  {
    BzlaBitVector *bv_bw_s    = bzla_bv_uint64_to_bv(mm, bw_s, bw_s);
    BzlaBitVector *s_ult_bw_s = bzla_bv_ult(mm, s, bv_bw_s);

    res = true;
    if (bzla_bv_is_true(s_ult_bw_s))
    {
      BzlaBitVector *t_sll_s = bzla_bv_sll(mm, t, s);
      BzlaBitVector *sra_s   = bzla_bv_sra(mm, t_sll_s, s);
      BzlaBitVector *eq_t    = bzla_bv_eq(mm, sra_s, t);
      res                    = bzla_bv_is_true(eq_t);
      bzla_bv_free(mm, t_sll_s);
      bzla_bv_free(mm, sra_s);
      bzla_bv_free(mm, eq_t);
    }
    bzla_bv_free(mm, s_ult_bw_s);
    if (res)
    {
      BzlaBitVector *s_uge_bw_s = bzla_bv_ulte(mm, bv_bw_s, s);
      res = (!bzla_bv_is_true(s_uge_bw_s) || bzla_bv_is_ones(t)
             || bzla_bv_is_zero(t));
      bzla_bv_free(mm, s_uge_bw_s);
    }
    bzla_bv_free(mm, bv_bw_s);
  }
  else
  {
    assert(pos_x == 1);
    res = false;
    for (uint32_t i = 0; i < bw_s && !res; i++)
    {
      BzlaBitVector *bv_i    = bzla_bv_uint64_to_bv(mm, i, bw_s);
      BzlaBitVector *s_sra_i = bzla_bv_sra(mm, s, bv_i);
      BzlaBitVector *eq_t    = bzla_bv_eq(mm, s_sra_i, t);
      res                    = bzla_bv_is_true(eq_t);
      bzla_bv_free(mm, bv_i);
      bzla_bv_free(mm, s_sra_i);
      bzla_bv_free(mm, eq_t);
    }
  }
  return res;
}

/**
 * Check invertibility condition (without considering const bits in x) for:
 *
 * pos_x = 0:
 * x >> s = t
 * IC: (t << s) >> s = t
 *
 * pos_x = 1:
 * s >> x = t
 * IC: (\/ s >> i = t)  i = 0..bw(s)-1
 */
bool
bzla_is_inv_srl(BzlaMemMgr *mm,
                const BzlaBvDomain *x,
                const BzlaBitVector *t,
                const BzlaBitVector *s,
                uint32_t pos_x)
{
  assert(mm);
  assert(t);
  assert(s);
  (void) x;

  bool res;
  if (pos_x == 0)
  {
    BzlaBitVector *t_sll_s = bzla_bv_sll(mm, t, s);
    BzlaBitVector *srl_s   = bzla_bv_srl(mm, t_sll_s, s);
    BzlaBitVector *eq_t    = bzla_bv_eq(mm, srl_s, t);
    res                    = bzla_bv_is_true(eq_t);
    bzla_bv_free(mm, t_sll_s);
    bzla_bv_free(mm, srl_s);
    bzla_bv_free(mm, eq_t);
  }
  else
  {
    assert(pos_x == 1);
    res = false;
    for (uint32_t i = 0, bw_s = bzla_bv_get_width(s); i <= bw_s && !res; i++)
    {
      BzlaBitVector *bv_i    = bzla_bv_uint64_to_bv(mm, i, bw_s);
      BzlaBitVector *s_srl_i = bzla_bv_srl(mm, s, bv_i);
      BzlaBitVector *eq_t    = bzla_bv_eq(mm, s_srl_i, t);
      res                    = bzla_bv_is_true(eq_t);
      bzla_bv_free(mm, bv_i);
      bzla_bv_free(mm, s_srl_i);
      bzla_bv_free(mm, eq_t);
    }
  }
  return res;
}

/**
 * Check invertibility condition (without considering const bits in x) for:
 *
 * pos_x = 0:
 * x < s = t
 * IC: t = 0 || s != 0
 *
 * pos_x = 1:
 * s < x = t
 * IC: t = 0 || s != ~0
 */
bool
bzla_is_inv_ult(BzlaMemMgr *mm,
                const BzlaBvDomain *x,
                const BzlaBitVector *t,
                const BzlaBitVector *s,
                uint32_t pos_x)
{
  assert(mm);
  assert(t);
  assert(s);
  (void) mm;
  (void) x;

  bool res;
  if (pos_x == 0)
  {
    res = bzla_bv_is_zero(t) || !bzla_bv_is_zero(s);
  }
  else
  {
    assert(pos_x == 1);
    res = bzla_bv_is_zero(t) || !bzla_bv_is_ones(s);
  }
  return res;
}

/**
 * Check invertibility condition (without considering const bits in x) for:
 *
 * pos_x = 0:
 * x / s = t
 * IC: (s * t) / s = t
 *
 * pos_x = 1:
 * s / x = t
 * IC: s / (s / t) = t
 */
bool
bzla_is_inv_udiv(BzlaMemMgr *mm,
                 const BzlaBvDomain *x,
                 const BzlaBitVector *t,
                 const BzlaBitVector *s,
                 uint32_t pos_x)
{
  assert(mm);
  assert(t);
  assert(s);
  (void) x;

  BzlaBitVector *udiv;
  bool res;
  if (pos_x == 0)
  {
    BzlaBitVector *s_mul_t = bzla_bv_mul(mm, s, t);
    udiv                   = bzla_bv_udiv(mm, s_mul_t, s);
    bzla_bv_free(mm, s_mul_t);
  }
  else
  {
    assert(pos_x == 1);
    BzlaBitVector *s_udiv_t = bzla_bv_udiv(mm, s, t);
    udiv                    = bzla_bv_udiv(mm, s, s_udiv_t);
    bzla_bv_free(mm, s_udiv_t);
  }
  BzlaBitVector *eq_t = bzla_bv_eq(mm, udiv, t);
  res                 = bzla_bv_is_true(eq_t);
  bzla_bv_free(mm, udiv);
  bzla_bv_free(mm, eq_t);
  return res;
}

/**
 * Check invertibility condition (without considering const bits in x) for:
 *
 * pos_x = 0:
 * x % s = t
 * IC: ~(-s) >= t
 *
 * pos_x = 1:
 * s % x = t
 * IC: (t + t - s) & s >= t
 */
bool
bzla_is_inv_urem(BzlaMemMgr *mm,
                 const BzlaBvDomain *x,
                 const BzlaBitVector *t,
                 const BzlaBitVector *s,
                 uint32_t pos_x)
{
  assert(mm);
  assert(t);
  assert(s);
  (void) x;

  bool res;
  BzlaBitVector *neg_s = bzla_bv_neg(mm, s);
  if (pos_x == 0)
  {
    BzlaBitVector *not_neg_s = bzla_bv_not(mm, neg_s);
    BzlaBitVector *uge_t     = bzla_bv_ulte(mm, t, not_neg_s);
    res                      = bzla_bv_is_true(uge_t);
    bzla_bv_free(mm, not_neg_s);
    bzla_bv_free(mm, uge_t);
  }
  else
  {
    assert(pos_x == 1);
    BzlaBitVector *t_add_t = bzla_bv_add(mm, t, t);
    BzlaBitVector *sub_s   = bzla_bv_add(mm, t_add_t, neg_s);
    BzlaBitVector *and_s   = bzla_bv_and(mm, sub_s, s);
    BzlaBitVector *uge_t   = bzla_bv_ulte(mm, t, and_s);
    res                    = bzla_bv_is_true(uge_t);
    bzla_bv_free(mm, t_add_t);
    bzla_bv_free(mm, sub_s);
    bzla_bv_free(mm, and_s);
    bzla_bv_free(mm, uge_t);
  }
  bzla_bv_free(mm, neg_s);
  return res;
}

/* -------------------------------------------------------------------------- */
/* Check invertibility while considering constant bits in x.                  */
/* -------------------------------------------------------------------------- */

/**
 * Check invertibility condition with respect to const bits in x for:
 *
 * x + s = t
 * s + x = t
 *
 * IC: (((t - s) & hi_x) | lo_x) = t - s
 */
bool
bzla_is_inv_add_const(BzlaMemMgr *mm,
                      const BzlaBvDomain *x,
                      const BzlaBitVector *t,
                      const BzlaBitVector *s,
                      uint32_t pos_x)
{
  assert(mm);
  assert(x);
  assert(t);
  assert(s);
  (void) pos_x;

  bool res;
  BzlaBitVector *sub, *and, * or ;

  sub = bzla_bv_sub(mm, t, s);
  and = bzla_bv_and(mm, sub, x->hi);
  or  = bzla_bv_or(mm, and, x->lo);
  res = bzla_bv_compare(or, sub) == 0;
  bzla_bv_free(mm, or);
  bzla_bv_free(mm, and);
  bzla_bv_free(mm, sub);
  return res;
}

/**
 * Check invertibility condition with respect to const bits in x for:
 *
 * x & s = t
 * s & x = t
 *
 * IC: ((s & t & hi_x) | lo_x) = t
 */
bool
bzla_is_inv_and_const(BzlaMemMgr *mm,
                      const BzlaBvDomain *x,
                      const BzlaBitVector *t,
                      const BzlaBitVector *s,
                      uint32_t pos_x)
{
  assert(mm);
  assert(x);
  assert(t);
  assert(s);
  (void) pos_x;

  bool res;
  BzlaBitVector *and1, *and2, * or ;

  and1 = bzla_bv_and(mm, s, t);
  and2 = bzla_bv_and(mm, and1, x->hi);
  or   = bzla_bv_or(mm, and2, x->lo);
  res  = bzla_bv_compare(or, t) == 0;
  bzla_bv_free(mm, or);
  bzla_bv_free(mm, and2);
  bzla_bv_free(mm, and1);
  return res;
}

bool
bzla_is_inv_concat_const(BzlaMemMgr *mm,
                         const BzlaBvDomain *x,
                         const BzlaBitVector *t,
                         const BzlaBitVector *s,
                         uint32_t pos_x)
{
  assert(mm);
  assert(x);
  assert(t);
  assert(s);
  (void) pos_x;
  return true;
}

bool
bzla_is_inv_eq_const(BzlaMemMgr *mm,
                     const BzlaBvDomain *x,
                     const BzlaBitVector *t,
                     const BzlaBitVector *s,
                     uint32_t pos_x)
{
  assert(mm);
  assert(x);
  assert(t);
  assert(s);
  (void) pos_x;
  return true;
}

bool
bzla_is_inv_mul_const(BzlaMemMgr *mm,
                      const BzlaBvDomain *x,
                      const BzlaBitVector *t,
                      const BzlaBitVector *s,
                      uint32_t pos_x)
{
  assert(mm);
  assert(x);
  assert(t);
  assert(s);
  (void) pos_x;
  return true;
}

bool
bzla_is_inv_or_const(BzlaMemMgr *mm,
                     const BzlaBvDomain *x,
                     const BzlaBitVector *t,
                     const BzlaBitVector *s,
                     uint32_t pos_x)
{
  assert(mm);
  assert(x);
  assert(t);
  assert(s);
  (void) pos_x;
  return true;
}

bool
bzla_is_inv_sll_const(BzlaMemMgr *mm,
                      const BzlaBvDomain *x,
                      const BzlaBitVector *t,
                      const BzlaBitVector *s,
                      uint32_t pos_x)
{
  assert(mm);
  assert(x);
  assert(t);
  assert(s);
  (void) pos_x;
  return true;
}

bool
bzla_is_inv_sra_const(BzlaMemMgr *mm,
                      const BzlaBvDomain *x,
                      const BzlaBitVector *t,
                      const BzlaBitVector *s,
                      uint32_t pos_x)
{
  assert(mm);
  assert(x);
  assert(t);
  assert(s);
  (void) pos_x;
  return true;
}

bool
bzla_is_inv_srl_const(BzlaMemMgr *mm,
                      const BzlaBvDomain *x,
                      const BzlaBitVector *t,
                      const BzlaBitVector *s,
                      uint32_t pos_x)
{
  assert(mm);
  assert(x);
  assert(t);
  assert(s);
  (void) pos_x;
  return true;
}

bool
bzla_is_inv_udiv_const(BzlaMemMgr *mm,
                       const BzlaBvDomain *x,
                       const BzlaBitVector *t,
                       const BzlaBitVector *s,
                       uint32_t pos_x)
{
  assert(mm);
  assert(x);
  assert(t);
  assert(s);
  (void) pos_x;
  return true;
}

bool
bzla_is_inv_ult_const(BzlaMemMgr *mm,
                      const BzlaBvDomain *x,
                      const BzlaBitVector *t,
                      const BzlaBitVector *s,
                      uint32_t pos_x)
{
  assert(mm);
  assert(x);
  assert(t);
  assert(s);
  (void) pos_x;
  return true;
}

bool
bzla_is_inv_urem_const(BzlaMemMgr *mm,
                       const BzlaBvDomain *x,
                       const BzlaBitVector *t,
                       const BzlaBitVector *s,
                       uint32_t pos_x)
{
  assert(mm);
  assert(x);
  assert(t);
  assert(s);
  (void) pos_x;
  return true;
}
