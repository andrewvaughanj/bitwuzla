(set-logic QF_FP)
(set-option :produce-models true)
(define-fun rne ()  RoundingMode roundNearestTiesToEven)
(define-fun rna () RoundingMode roundNearestTiesToAway)
(define-fun rtp () RoundingMode roundTowardPositive)
(define-fun rtn () RoundingMode roundTowardNegative)
(define-fun rtz () RoundingMode roundTowardZero)
(define-fun _rne () RoundingMode RNE)
(define-fun _rna () RoundingMode RNA)
(define-fun _rtp () RoundingMode RTP)
(define-fun _rtn () RoundingMode RTN)
(define-fun _rtz () RoundingMode RTZ)
(assert (= rne _rne))
(assert (= rna _rna))
(assert (= rtp _rtp))
(assert (= rtn _rtn))
(assert (= rtz _rtz))
(assert (= rne _rna))
(check-sat)

