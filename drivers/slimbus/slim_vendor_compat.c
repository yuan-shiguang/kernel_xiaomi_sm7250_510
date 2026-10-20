// SPDX-License-Identifier: GPL-2.0
/*
 * [VENDOR-PORT Phase7] SLIMbus vendor compatibility layer for 5.10 GKI.
 *
 * The 4.19 vendor slimbus.c implemented a legacy (SLIMbus 1.x) channel/value
 * API using a vendor struct slim_controller that is incompatible with the
 * upstream GKI core. This file re-implements the same external API surface
 * against the GKI core:
 *   - value-element transfers use the public slim_xfer_msg()
 *   - channel/port configuration uses slim_do_transfer() with the manager
 *     reconfiguration messages, mirroring drivers/slimbus/stream.c (the
 *     SLIMbus 2.0 "Next" reconfiguration model supported by slim-qcom-ngd).
 *
 * This replaces the weak no-op stubs previously provided by
 * drivers/soc/qcom/vendor_port_stubs.c.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/idr.h>
#include <linux/slimbus.h>	/* public: struct slim_ch, slim_ele_access, API decls */
#include "slimbus.h"		/* internal: struct slim_controller, slim_msg_txn, MC, macros */

/* ---- copied from stream.c (static there) for seg-dist / presence-rate calc --- */
struct segdist_code {
	int ratem;
	int seg_interval;
	int segdist_code;
	u32 seg_offset_mask;
};

static const struct segdist_code slim_vendor_segdist_codes[] = {
	{1,	1536,	0x200,	0xdff},
	{2,	768,	0x100,	0xcff},
	{4,	384,	0x080,	0xc7f},
	{8,	192,	0x040,	0xc3f},
	{16,	96,	0x020,	0xc1f},
	{32,	48,	0x010,	0xc0f},
	{64,	24,	0x008,	0xc07},
	{128,	12,	0x004,	0xc03},
	{256,	6,	0x002,	0xc01},
	{512,	3,	0x001,	0xc00},
	{3,	512,	0xe00,	0x1ff},
	{6,	256,	0xd00,	0x0ff},
	{12,	128,	0xc80,	0x07f},
	{24,	64,	0xc40,	0x03f},
	{48,	32,	0xc20,	0x01f},
	{96,	16,	0xc10,	0x00f},
	{192,	8,	0xc08,	0x007},
	{364,	4,	0xc04,	0x003},
	{768,	2,	0xc02,	0x001},
};

static const int slim_vendor_presence_rate_table[] = {
	0, 12000, 24000, 48000, 96000, 192000, 384000, 768000,
	0, 11025, 22050, 44100, 88200, 176400, 352800, 705600,
	4000, 8000, 16000, 32000, 64000, 128000, 256000, 512000,
};

static int slim_vendor_get_segdist_code(int ratem)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(slim_vendor_segdist_codes); i++)
		if (slim_vendor_segdist_codes[i].ratem == ratem)
			return slim_vendor_segdist_codes[i].segdist_code;
	return -EINVAL;
}

static int slim_vendor_get_prate_code(int rate)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(slim_vendor_presence_rate_table); i++)
		if (rate == slim_vendor_presence_rate_table[i])
			return i;
	return -EINVAL;
}

/* map legacy vendor prot enum -> GKI SLIM_PROTO_* */
static u8 slim_vendor_proto_map(enum slim_ch_proto p)
{
	switch (p) {
	case SLIM_HARD_ISO:
	case SLIM_AUTO_ISO:
		return SLIM_PROTO_ISO;
	case SLIM_PUSH:
		return SLIM_PROTO_PUSH;
	case SLIM_PULL:
		return SLIM_PROTO_PULL;
	default:
		return SLIM_PROTO_ASYNC_SMPLX;
	}
}

static int slim_vendor_baser_hz(enum slim_ch_rate r)
{
	switch (r) {
	case SLIM_RATE_1HZ:
		return 1;
	case SLIM_RATE_4000HZ:
		return 4000;
	case SLIM_RATE_11025HZ:
		return 11025;
	default:
		return 4000;
	}
}

/* ---- per-controller channel/group bookkeeping ---- */
struct slim_vendor_chan {
	struct list_head node;
	struct slim_controller *ctrl;
	u16 ch_num;
	u16 grp_id;
};

struct slim_vendor_grp {
	struct list_head node;
	u16 grp_id;
	u16 ch_num[16];
	u8 nchan;
};

static DEFINE_MUTEX(slim_vendor_lock);
static LIST_HEAD(slim_vendor_chans);
static LIST_HEAD(slim_vendor_grps);
static DEFINE_IDA(slim_vendor_chan_ida);
static DEFINE_IDA(slim_vendor_grp_ida);

static struct slim_vendor_chan *slim_vendor_chan_find(struct slim_controller *ctrl,
						      u16 ch_num)
{
	struct slim_vendor_chan *c;
	list_for_each_entry(c, &slim_vendor_chans, node)
		if (c->ctrl == ctrl && c->ch_num == ch_num)
			return c;
	return NULL;
}

static struct slim_vendor_grp *slim_vendor_grp_find(u16 grp_id)
{
	struct slim_vendor_grp *g;
	list_for_each_entry(g, &slim_vendor_grps, node)
		if (g->grp_id == grp_id)
			return g;
	return NULL;
}

/* ---- value element management (real, via public slim_xfer_msg) ---- */
int slim_request_val_element(struct slim_device *sb, struct slim_ele_access *msg,
			     u8 *buf, u8 len)
{
	struct slim_val_inf inf = {
		.start_offset = msg->start_offset,
		.num_bytes = len,
		.rbuf = buf,
		.wbuf = NULL,
		.comp = msg->comp,
	};
	return slim_xfer_msg(sb, &inf, SLIM_MSG_MC_REQUEST_VALUE);
}
EXPORT_SYMBOL(slim_request_val_element);

int slim_change_val_element(struct slim_device *sb, struct slim_ele_access *msg,
			    const u8 *buf, u8 len)
{
	struct slim_val_inf inf = {
		.start_offset = msg->start_offset,
		.num_bytes = len,
		.rbuf = NULL,
		.wbuf = buf,
		.comp = msg->comp,
	};
	return slim_xfer_msg(sb, &inf, SLIM_MSG_MC_CHANGE_VALUE);
}
EXPORT_SYMBOL(slim_change_val_element);

/* ---- port handle encoding (trivial, no bus access) ---- */
int slim_get_slaveport(u8 la, int idx, u32 *rh, enum slim_port_flow flw)
{
	if (!rh || idx < 0 || idx > 0xFF)
		return -EINVAL;
	*rh = SLIM_PORT_HDL(la, flw, (u8)idx);
	return 0;
}
EXPORT_SYMBOL(slim_get_slaveport);

/* ---- query/allocate a channel number (manager-assigned) ---- */
int slim_query_ch(struct slim_device *sb, u8 ch, u16 *chanh)
{
	struct slim_vendor_chan *c;
	int num;

	if (!sb || !chanh)
		return -EINVAL;
	num = ida_alloc_range(&slim_vendor_chan_ida, 1, 255, GFP_KERNEL);
	if (num < 0)
		return num;
	c = kzalloc(sizeof(*c), GFP_KERNEL);
	if (!c) {
		ida_free(&slim_vendor_chan_ida, num);
		return -ENOMEM;
	}
	c->ctrl = sb->ctrl;
	c->ch_num = (u16)num;
	c->grp_id = 0;
	mutex_lock(&slim_vendor_lock);
	list_add_tail(&c->node, &slim_vendor_chans);
	mutex_unlock(&slim_vendor_lock);
	*chanh = (u16)num;
	return 0;
}
EXPORT_SYMBOL(slim_query_ch);

/* ---- define channel(s): NEXT_DEFINE_CHANNEL + NEXT_DEFINE_CONTENT ---- */
int slim_define_ch(struct slim_device *sb, struct slim_ch *prop, u16 *chanh,
		   u8 nchan, bool grp, u16 *grph)
{
	struct slim_controller *ctrl;
	u8 wbuf[4];
	int i, ret, prrate, seg;
	u8 proto;
	DEFINE_SLIM_BCAST_TXN(begin, SLIM_MSG_MC_BEGIN_RECONFIGURATION, 3,
			      SLIM_LA_MANAGER, NULL);
	DEFINE_SLIM_BCAST_TXN(reconf, SLIM_MSG_MC_RECONFIGURE_NOW, 3,
			      SLIM_LA_MANAGER, NULL);

	if (!sb || !prop || !chanh || !nchan)
		return -EINVAL;
	ctrl = sb->ctrl;
	if (!ctrl)
		return -EINVAL;

	proto = slim_vendor_proto_map(prop->prot);
	seg = slim_vendor_get_segdist_code(prop->ratem);
	if (seg < 0)
		seg = 0x200; /* fallback: ratem=1 segdist */
	prrate = slim_vendor_get_prate_code(slim_vendor_baser_hz(prop->baser) *
					    prop->ratem);
	if (prrate < 0)
		prrate = 0;

	ret = slim_do_transfer(ctrl, &begin);
	if (ret)
		return ret;

	for (i = 0; i < nchan; i++) {
		u16 ch_num = chanh[i];
		struct slim_val_inf msg = {0, 4, NULL, wbuf, NULL};
		DEFINE_SLIM_LDEST_TXN(dtxn, SLIM_MSG_MC_NEXT_DEFINE_CHANNEL, 8,
				      sb->laddr, &msg);

		wbuf[0] = (u8)ch_num;
		wbuf[1] = seg & 0xFF;
		wbuf[2] = (proto << 4) | ((seg & 0xF00) >> 8);
		wbuf[3] = (proto == SLIM_PROTO_ISO)
			? (prop->sampleszbits / SLIM_SLOT_LEN_BITS)
			: (prop->sampleszbits / SLIM_SLOT_LEN_BITS + 1);
		ret = slim_do_transfer(ctrl, &dtxn);
		if (ret)
			goto out_reconf;

		{
			struct slim_val_inf cmsg = {0, 4, NULL, wbuf, NULL};
			DEFINE_SLIM_LDEST_TXN(ctxn,
					      SLIM_MSG_MC_NEXT_DEFINE_CONTENT, 8,
					      sb->laddr, &cmsg);
			wbuf[0] = (u8)ch_num;
			wbuf[1] = (u8)prrate;
			if (proto != SLIM_PROTO_ISO)
				wbuf[1] |= SLIM_CHANNEL_CONTENT_FL;
			wbuf[2] = (u8)prop->dataf | ((u8)prop->auxf << 4);
			wbuf[3] = prop->sampleszbits / SLIM_SLOT_LEN_BITS;
			ret = slim_do_transfer(ctrl, &ctxn);
			if (ret)
				goto out_reconf;
		}
	}

	if (grp && grph) {
		struct slim_vendor_grp *g;
		int gid = ida_alloc_range(&slim_vendor_grp_ida, 1, 255,
					  GFP_KERNEL);
		if (gid < 0) {
			ret = gid;
			goto out_reconf;
		}
		g = kzalloc(sizeof(*g), GFP_KERNEL);
		if (!g) {
			ida_free(&slim_vendor_grp_ida, gid);
			ret = -ENOMEM;
			goto out_reconf;
		}
		g->grp_id = (u16)gid;
		g->nchan = nchan;
		for (i = 0; i < nchan && i < (int)ARRAY_SIZE(g->ch_num); i++)
			g->ch_num[i] = chanh[i];
		mutex_lock(&slim_vendor_lock);
		list_add_tail(&g->node, &slim_vendor_grps);
		for (i = 0; i < nchan; i++) {
			struct slim_vendor_chan *c =
				slim_vendor_chan_find(ctrl, chanh[i]);
			if (c)
				c->grp_id = (u16)gid;
		}
		mutex_unlock(&slim_vendor_lock);
		*grph = (u16)gid;
	}

	ret = 0;
out_reconf:
	slim_do_transfer(ctrl, &reconf);
	return ret;
}
EXPORT_SYMBOL(slim_define_ch);

/* ---- connect a source port to a channel ---- */
int slim_connect_src(struct slim_device *sb, u32 srch, u16 chanh)
{
	struct slim_controller *ctrl;
	u8 wbuf[2];
	struct slim_val_inf msg = {0, 2, NULL, wbuf, NULL};
	DEFINE_SLIM_LDEST_TXN(txn, SLIM_MSG_MC_CONNECT_SOURCE, 6, sb->laddr,
			      &msg);
	DEFINE_SLIM_BCAST_TXN(begin, SLIM_MSG_MC_BEGIN_RECONFIGURATION, 3,
			      SLIM_LA_MANAGER, NULL);
	DEFINE_SLIM_BCAST_TXN(reconf, SLIM_MSG_MC_RECONFIGURE_NOW, 3,
			      SLIM_LA_MANAGER, NULL);
	int ret;

	if (!sb || !sb->ctrl)
		return -EINVAL;
	ctrl = sb->ctrl;
	wbuf[0] = (u8)SLIM_HDL_TO_PORT(srch);
	wbuf[1] = (u8)chanh;

	ret = slim_do_transfer(ctrl, &begin);
	if (ret)
		return ret;
	ret = slim_do_transfer(ctrl, &txn);
	slim_do_transfer(ctrl, &reconf);
	return ret;
}
EXPORT_SYMBOL(slim_connect_src);

/* ---- connect sink port(s) to a channel ---- */
int slim_connect_sink(struct slim_device *sb, u32 *sinkh, int nsink, u16 chanh)
{
	struct slim_controller *ctrl;
	DEFINE_SLIM_BCAST_TXN(begin, SLIM_MSG_MC_BEGIN_RECONFIGURATION, 3,
			      SLIM_LA_MANAGER, NULL);
	DEFINE_SLIM_BCAST_TXN(reconf, SLIM_MSG_MC_RECONFIGURE_NOW, 3,
			      SLIM_LA_MANAGER, NULL);
	int i, ret;

	if (!sb || !sb->ctrl || !sinkh || nsink <= 0)
		return -EINVAL;
	ctrl = sb->ctrl;

	ret = slim_do_transfer(ctrl, &begin);
	if (ret)
		return ret;
	for (i = 0; i < nsink; i++) {
		u8 wbuf[2];
		struct slim_val_inf msg = {0, 2, NULL, wbuf, NULL};
		DEFINE_SLIM_LDEST_TXN(txn, SLIM_MSG_MC_CONNECT_SINK, 6,
				      sb->laddr, &msg);
		wbuf[0] = (u8)SLIM_HDL_TO_PORT(sinkh[i]);
		wbuf[1] = (u8)chanh;
		ret = slim_do_transfer(ctrl, &txn);
		if (ret)
			break;
	}
	slim_do_transfer(ctrl, &reconf);
	return ret;
}
EXPORT_SYMBOL(slim_connect_sink);

/* ---- disconnect port(s) ---- */
int slim_disconnect_ports(struct slim_device *sb, u32 *ph, int nph)
{
	struct slim_controller *ctrl;
	DEFINE_SLIM_BCAST_TXN(begin, SLIM_MSG_MC_BEGIN_RECONFIGURATION, 3,
			      SLIM_LA_MANAGER, NULL);
	DEFINE_SLIM_BCAST_TXN(reconf, SLIM_MSG_MC_RECONFIGURE_NOW, 3,
			      SLIM_LA_MANAGER, NULL);
	int i, ret;

	if (!sb || !sb->ctrl || !ph || nph <= 0)
		return -EINVAL;
	ctrl = sb->ctrl;

	ret = slim_do_transfer(ctrl, &begin);
	if (ret)
		return ret;
	for (i = 0; i < nph; i++) {
		u8 wbuf[1];
		struct slim_val_inf msg = {0, 1, NULL, wbuf, NULL};
		DEFINE_SLIM_LDEST_TXN(txn, SLIM_MSG_MC_DISCONNECT_PORT, 5,
				      sb->laddr, &msg);
		wbuf[0] = (u8)SLIM_HDL_TO_PORT(ph[i]);
		ret = slim_do_transfer(ctrl, &txn);
		if (ret)
			break;
	}
	slim_do_transfer(ctrl, &reconf);
	return ret;
}
EXPORT_SYMBOL(slim_disconnect_ports);

/* ---- control a channel: activate / suspend / remove ---- */
int slim_control_ch(struct slim_device *sb, u16 chanh,
		    enum slim_ch_control chctrl, bool commit)
{
	struct slim_controller *ctrl;
	DEFINE_SLIM_BCAST_TXN(begin, SLIM_MSG_MC_BEGIN_RECONFIGURATION, 3,
			      SLIM_LA_MANAGER, NULL);
	DEFINE_SLIM_BCAST_TXN(reconf, SLIM_MSG_MC_RECONFIGURE_NOW, 3,
			      SLIM_LA_MANAGER, NULL);
	struct slim_vendor_grp *grp = NULL;
	u16 chans[16];
	u8 nchan = 0;
	int i, ret;

	if (!sb || !sb->ctrl)
		return -EINVAL;
	ctrl = sb->ctrl;

	mutex_lock(&slim_vendor_lock);
	grp = slim_vendor_grp_find(chanh);
	if (grp) {
		nchan = grp->nchan;
		for (i = 0; i < nchan && i < (int)ARRAY_SIZE(chans); i++)
			chans[i] = grp->ch_num[i];
	} else {
		chans[0] = chanh;
		nchan = 1;
	}
	mutex_unlock(&slim_vendor_lock);

	ret = slim_do_transfer(ctrl, &begin);
	if (ret)
		return ret;

	if (chctrl == SLIM_CH_ACTIVATE) {
		for (i = 0; i < nchan; i++) {
			u8 wbuf[1];
			struct slim_val_inf msg = {0, 1, NULL, wbuf, NULL};
			DEFINE_SLIM_LDEST_TXN(txn,
					      SLIM_MSG_MC_NEXT_ACTIVATE_CHANNEL,
					      5, sb->laddr, &msg);
			wbuf[0] = (u8)chans[i];
			ret = slim_do_transfer(ctrl, &txn);
			if (ret)
				break;
		}
	} else if (chctrl == SLIM_CH_SUSPEND) {
		for (i = 0; i < nchan; i++) {
			u8 wbuf[1];
			struct slim_val_inf msg = {0, 1, NULL, wbuf, NULL};
			DEFINE_SLIM_LDEST_TXN(txn,
					      SLIM_MSG_MC_NEXT_DEACTIVATE_CHANNEL,
					      5, sb->laddr, &msg);
			wbuf[0] = (u8)chans[i];
			ret = slim_do_transfer(ctrl, &txn);
			if (ret)
				break;
		}
	} else { /* SLIM_CH_REMOVE */
		for (i = 0; i < nchan; i++) {
			u8 wbuf[1];
			struct slim_val_inf amsg = {0, 1, NULL, wbuf, NULL};
			struct slim_val_inf rmsg = {0, 1, NULL, wbuf, NULL};
			DEFINE_SLIM_LDEST_TXN(atxn,
					      SLIM_MSG_MC_NEXT_DEACTIVATE_CHANNEL,
					      5, sb->laddr, &amsg);
			DEFINE_SLIM_LDEST_TXN(rtxn,
					      SLIM_MSG_MC_NEXT_REMOVE_CHANNEL,
					      5, sb->laddr, &rmsg);
			wbuf[0] = (u8)chans[i];
			ret = slim_do_transfer(ctrl, &atxn);
			if (ret)
				break;
			ret = slim_do_transfer(ctrl, &rtxn);
			if (ret)
				break;
		}
		mutex_lock(&slim_vendor_lock);
		for (i = 0; i < nchan; i++) {
			struct slim_vendor_chan *c =
				slim_vendor_chan_find(ctrl, chans[i]);
			if (c) {
				list_del(&c->node);
				ida_free(&slim_vendor_chan_ida, c->ch_num);
				kfree(c);
			}
		}
		if (grp) {
			list_del(&grp->node);
			ida_free(&slim_vendor_grp_ida, grp->grp_id);
			kfree(grp);
		}
		mutex_unlock(&slim_vendor_lock);
	}

	slim_do_transfer(ctrl, &reconf);
	(void)commit;
	return ret;
}
EXPORT_SYMBOL(slim_control_ch);
