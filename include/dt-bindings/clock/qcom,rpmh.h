/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (c) 2018, 2020, The Linux Foundation. All rights reserved. */


#ifndef _DT_BINDINGS_CLK_MSM_RPMH_H
#define _DT_BINDINGS_CLK_MSM_RPMH_H

/* RPMh controlled clocks */
#define RPMH_CXO_CLK				0
#define RPMH_CXO_CLK_A				1
#define RPMH_LN_BB_CLK2				2
#define RPMH_LN_BB_CLK2_A			3
#define RPMH_LN_BB_CLK3				4
#define RPMH_LN_BB_CLK3_A			5
#define RPMH_RF_CLK1				6
#define RPMH_RF_CLK1_A				7
#define RPMH_RF_CLK2				8
#define RPMH_RF_CLK2_A				9
#define RPMH_RF_CLK3				10
#define RPMH_RF_CLK3_A				11
#define RPMH_IPA_CLK				12
#define RPMH_LN_BB_CLK1				13
#define RPMH_LN_BB_CLK1_A			14

/*
 * LITO / LAGOON 专用 RPMh 时钟。
 * 5.10 上游只保留了 sdm845/sm8150/sm8250 需要的编号，这里从 15 起追加，
 * 避免改动上面已有的编号（clk-rpmh.c 的 rpmh_clocks[] 依赖其顺序）。
 */
#define RPMH_RF_CLKD3				15
#define RPMH_RF_CLKD3_A				16
#define RPMH_RF_CLKD4				17
#define RPMH_RF_CLKD4_A				18
#define RPMH_RF_CLK4				19
#define RPMH_RF_CLK4_A				20
#define RPMH_QLINK_CLK				21
#define RPMH_QLINK_CLK_A			22
#define RPMH_RF_CLK5				23
#define RPMH_RF_CLK5_A				24
#define RPMH_LN_BB3				25
#define RPMH_LN_BB3_A				26

#endif
