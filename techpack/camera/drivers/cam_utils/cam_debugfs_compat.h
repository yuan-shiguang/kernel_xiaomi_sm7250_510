/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2021 XiaoMi, Inc.
 *
 * 5.10 内核的 debugfs_create_file()/debugfs_create_u32()/debugfs_create_bool()
 * 等接口返回值由 struct dentry * 改为 void，无法再判断是否创建成功。
 * 这里提供一个恒为真的包装，让原有的错误处理分支保持语法有效（实际不会命中）。
 */
#ifndef __CAM_DEBUGFS_COMPAT_H__
#define __CAM_DEBUGFS_COMPAT_H__

#define CAM_DEBUGFS_OK(expr) ({ (void)(expr); 1; })

#endif /* __CAM_DEBUGFS_COMPAT_H__ */
