/*
 * Copyright (C) 2018 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#define PR_FMT_HEADER_MUST_BE_INCLUDED_BEFORE_ALL_HDRS
#include "private/tmem_pr_fmt.h" PR_FMT_HEADER_MUST_BE_INCLUDED_BEFORE_ALL_HDRS

#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <linux/types.h>
#include <linux/slab.h>
#if defined(CONFIG_MTK_SSMR) || (defined(CONFIG_CMA) && defined(CONFIG_MTK_SVP))
#include <memory_ssmr.h>
#endif

#include "private/mld_helper.h"
#include "private/tmem_error.h"
#include "private/tmem_priv.h"
#include "private/tmem_utils.h"
#include "tee_impl/tee_priv.h"
#include "tee_impl/tee_common.h"

#define SECMEM_2DFR_DEVICE_NAME "SECMEM_2DFR"

static struct trusted_mem_configs fr_smem_configs = {
	.session_keep_alive_enable = false,
	.minimal_chunk_size = SIZE_64K,
	.phys_mem_shift_bits = 6,
	.phys_limit_min_alloc_size = (1 << 6),
	.min_size_check_enable = false,
	.alignment_check_enable = true,
	.caps = 0,
};

static int __init fr_smem_init(void)
{
	int ret = TMEM_OK;
	struct trusted_mem_device *t_device;

	pr_info("%s:%d\n", __func__, __LINE__);

	t_device =
		create_trusted_mem_device(TRUSTED_MEM_2D_FR, &fr_smem_configs);
	if (INVALID(t_device)) {
		pr_err("create 2DFR_SMEM device failed\n");
		return TMEM_CREATE_DEVICE_FAILED;
	}

	get_tee_peer_ops(&t_device->peer_ops);
	get_tee_peer_priv_data(TEE_MEM_2D_FR, &t_device->peer_priv);

	snprintf(t_device->name, MAX_DEVICE_NAME_LEN, "%s",
		 SECMEM_2DFR_DEVICE_NAME);
#if defined(CONFIG_MTK_SSMR) || (defined(CONFIG_CMA) && defined(CONFIG_MTK_SVP))
	t_device->ssmr_feature_id = SSMR_FEAT_2D_FR;
#endif
	t_device->mem_type = TRUSTED_MEM_2D_FR;

	ret = register_trusted_mem_device(TRUSTED_MEM_2D_FR, t_device);
	if (ret) {
		destroy_trusted_mem_device(t_device);
		pr_err("register 2DFR_SMEM device failed\n");
		return ret;
	}

	pr_info("%s:%d (end)\n", __func__, __LINE__);
	return TMEM_OK;
}

static void __exit fr_smem_exit(void)
{
}

module_init(fr_smem_init);
module_exit(fr_smem_exit);

MODULE_AUTHOR("MediaTek Inc.");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MediaTek 2D_FR Secure Memory Driver");