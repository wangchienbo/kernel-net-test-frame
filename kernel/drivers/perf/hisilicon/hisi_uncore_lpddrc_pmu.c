// SPDX-License-Identifier: GPL-2.0-only
/*
 * HiSilicon SoC LPDDRC uncore Hardware event counters support
 *
 * Copyright (C) 2017 Hisilicon Limited
 * Author: Shaokun Zhang <zhangshaokun@hisilicon.com>
 *         Anurup M <anurup.m@huawei.com>
 *
 * This code is based on the uncore PMUs like arm-cci and arm-ccn.
 */
#include <linux/acpi.h>
#include <linux/bug.h>
#include <linux/cpuhotplug.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/list.h>
#include <linux/smp.h>

#include "hisi_uncore_pmu.h"

/* LPDDRC register definition in v1 */
#define LPDDRC_PERF_CTRL	0x4930
#define LPDDRC_FLUX_WR		0x4948
#define LPDDRC_FLUX_RD		0x494c
#define LPDDRC_FLUX_WCMD	0x4950
#define LPDDRC_FLUX_RCMD	0x4954
#define LPDDRC_PRE_CMD		0x4984
#define LPDDRC_ACT_CMD		0x4988
#define LPDDRC_RNK_CHG		0x4990
#define LPDDRC_RW_CHG		0x4994
#define LPDDRC_EVENT_CTRL	0x4d60
#define LPDDRC_INT_MASK		0x6c8
#define LPDDRC_INT_STATUS	0x6cc
#define LPDDRC_INT_CLEAR	0x6d0
#define LPDDRC_VERSION		0x710

#define LPDDRC_NR_COUNTERS	0x8
#define LPDDRC_V1_PERF_CTRL_EN	0x1
#define LPDDRC_V1_NR_EVENTS	0x7

/*
 * For PMU v1, there are eight-events and every event has been mapped
 * to fixed-purpose counters which register offset is not consistent.
 * Therefore there is no write event type and we assume that event
 * code (0 to 7) is equal to counter index in PMU driver.
 */
#define GET_LPDDRC_EVENTID(hwc)	(hwc->config_base & 0x7)

static const u32 lpddrc_reg_off[] = {
	LPDDRC_FLUX_WR, LPDDRC_FLUX_RD, LPDDRC_FLUX_WCMD, LPDDRC_FLUX_RCMD,
	LPDDRC_PRE_CMD, LPDDRC_ACT_CMD, LPDDRC_RNK_CHG, LPDDRC_RW_CHG
};

/*
 * Select the counter register offset using the counter index.
 * In PMU v1, there are no programmable counter, the count
 * is read form the statistics counter register itself.
 */
static u32 hisi_lpddrc_pmu_v1_get_counter_offset(int cntr_idx)
{
	return lpddrc_reg_off[cntr_idx];
}

static u64 hisi_lpddrc_pmu_v1_read_counter(struct hisi_pmu *lpddrc_pmu,
				      struct hw_perf_event *hwc)
{
	return readl(lpddrc_pmu->base +
		     hisi_lpddrc_pmu_v1_get_counter_offset(hwc->idx));
}

/*
 * For LPDDRC PMU, event counter should be reset when start counters,
 * reset the prev_count by software, because the counter register was RO.
 */
static void hisi_lpddrc_pmu_v1_write_counter(struct hisi_pmu *lpddrc_pmu,
					struct hw_perf_event *hwc, u64 val)
{
	local64_set(&hwc->prev_count, 0);
}

/*
 * For LPDDRC PMU v1, event has been mapped to fixed-purpose counter by hardware,
 * so there is no need to write event type, while it is programmable counter in
 * PMU v2.
 */
static void hisi_lpddrc_pmu_write_evtype(struct hisi_pmu *hha_pmu, int idx,
				       u32 type)
{
}

static void hisi_lpddrc_pmu_v1_start_counters(struct hisi_pmu *lpddrc_pmu)
{
	u32 val;

	/* Set perf_enable in LPDDRC_PERF_CTRL to start event counting */
	val = readl(lpddrc_pmu->base + LPDDRC_PERF_CTRL);
	val |= LPDDRC_V1_PERF_CTRL_EN;
	writel(val, lpddrc_pmu->base + LPDDRC_PERF_CTRL);
}

static void hisi_lpddrc_pmu_v1_stop_counters(struct hisi_pmu *lpddrc_pmu)
{
	u32 val;

	/* Clear perf_enable in LPDDRC_PERF_CTRL to stop event counting */
	val = readl(lpddrc_pmu->base + LPDDRC_PERF_CTRL);
	val &= ~LPDDRC_V1_PERF_CTRL_EN;
	writel(val, lpddrc_pmu->base + LPDDRC_PERF_CTRL);
}

static void hisi_lpddrc_pmu_v1_enable_counter(struct hisi_pmu *lpddrc_pmu,
					    struct hw_perf_event *hwc)
{
	u32 val;

	/* Set counter index(event code) in LPDDRC_EVENT_CTRL register */
	val = readl(lpddrc_pmu->base + LPDDRC_EVENT_CTRL);
	val |= (1 << GET_LPDDRC_EVENTID(hwc));
	writel(val, lpddrc_pmu->base + LPDDRC_EVENT_CTRL);
}

static void hisi_lpddrc_pmu_v1_disable_counter(struct hisi_pmu *lpddrc_pmu,
					     struct hw_perf_event *hwc)
{
	u32 val;

	/* Clear counter index(event code) in LPDDRC_EVENT_CTRL register */
	val = readl(lpddrc_pmu->base + LPDDRC_EVENT_CTRL);
	val &= ~(1 << GET_LPDDRC_EVENTID(hwc));
	writel(val, lpddrc_pmu->base + LPDDRC_EVENT_CTRL);
}

static int hisi_lpddrc_pmu_v1_get_event_idx(struct perf_event *event)
{
	struct hisi_pmu *lpddrc_pmu = to_hisi_pmu(event->pmu);
	unsigned long *used_mask = lpddrc_pmu->pmu_events.used_mask;
	struct hw_perf_event *hwc = &event->hw;
	/* For LPDDRC PMU, we use event code as counter index */
	int idx = GET_LPDDRC_EVENTID(hwc);

	if (test_bit(idx, used_mask))
		return -EAGAIN;

	set_bit(idx, used_mask);

	return idx;
}

static void hisi_lpddrc_pmu_v1_enable_counter_int(struct hisi_pmu *lpddrc_pmu,
						struct hw_perf_event *hwc)
{
	u32 val;

	/* Write 0 to enable interrupt */
	val = readl(lpddrc_pmu->base + LPDDRC_INT_MASK);
	val &= ~(1 << hwc->idx);
	writel(val, lpddrc_pmu->base + LPDDRC_INT_MASK);
}

static void hisi_lpddrc_pmu_v1_disable_counter_int(struct hisi_pmu *lpddrc_pmu,
						 struct hw_perf_event *hwc)
{
	u32 val;

	/* Write 1 to mask interrupt */
	val = readl(lpddrc_pmu->base + LPDDRC_INT_MASK);
	val |= 1 << hwc->idx;
	writel(val, lpddrc_pmu->base + LPDDRC_INT_MASK);
}

static u32 hisi_lpddrc_pmu_v1_get_int_status(struct hisi_pmu *lpddrc_pmu)
{
	return readl(lpddrc_pmu->base + LPDDRC_INT_STATUS);
}

static void hisi_lpddrc_pmu_v1_clear_int_status(struct hisi_pmu *lpddrc_pmu,
					      int idx)
{
	writel(1 << idx, lpddrc_pmu->base + LPDDRC_INT_CLEAR);
}

static const struct acpi_device_id hisi_lpddrc_pmu_acpi_match[] = {
	{}
};
MODULE_DEVICE_TABLE(acpi, hisi_lpddrc_pmu_acpi_match);

static const struct of_device_id lpddrc_of_match[] = {
	{ .compatible = "hisilicon,lpddrc-pmu", },
	{},
};

static int hisi_lpddrc_pmu_init_data(struct platform_device *pdev,
				   struct hisi_pmu *lpddrc_pmu)
{
	/*
	 * Use the SCCL_ID and LPDDRC channel ID to identify the
	 * LPDDRC PMU, while SCCL_ID is in MPIDR[aff2].
	 */
	if (device_property_read_u32(&pdev->dev, "hisilicon,ch-id",
				     &lpddrc_pmu->index_id)) {
		dev_err(&pdev->dev, "Can not read lpddrc channel-id!\n");
		return -EINVAL;
	}

	if (device_property_read_u32(&pdev->dev, "hisilicon,scl-id",
				     &lpddrc_pmu->sccl_id)) {
		dev_err(&pdev->dev, "Can not read lpddrc sccl-id!\n");
		return -EINVAL;
	}
	/* LPDDRC PMUs only share the same SCCL */
	lpddrc_pmu->ccl_id = -1;

	lpddrc_pmu->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(lpddrc_pmu->base)) {
		dev_err(&pdev->dev, "ioremap failed for lpddrc_pmu resource\n");
		return PTR_ERR(lpddrc_pmu->base);
	}

	lpddrc_pmu->identifier = readl(lpddrc_pmu->base + LPDDRC_VERSION);

	return 0;
}

static struct attribute *hisi_lpddrc_pmu_v1_format_attr[] = {
	HISI_PMU_FORMAT_ATTR(event, "config:0-4"),
	NULL,
};

static const struct attribute_group hisi_lpddrc_pmu_v1_format_group = {
	.name = "format",
	.attrs = hisi_lpddrc_pmu_v1_format_attr,
};

static struct attribute *hisi_lpddrc_pmu_v1_events_attr[] = {
	HISI_PMU_EVENT_ATTR(flux_wr,		0x00),
	HISI_PMU_EVENT_ATTR(flux_rd,		0x01),
	HISI_PMU_EVENT_ATTR(flux_wcmd,		0x02),
	HISI_PMU_EVENT_ATTR(flux_rcmd,		0x03),
	HISI_PMU_EVENT_ATTR(pre_cmd,		0x04),
	HISI_PMU_EVENT_ATTR(act_cmd,		0x05),
	HISI_PMU_EVENT_ATTR(rnk_chg,		0x06),
	HISI_PMU_EVENT_ATTR(rw_chg,		0x07),
	NULL,
};

static const struct attribute_group hisi_lpddrc_pmu_v1_events_group = {
	.name = "events",
	.attrs = hisi_lpddrc_pmu_v1_events_attr,
};

static DEVICE_ATTR(cpumask, 0444, hisi_cpumask_sysfs_show, NULL);

static struct attribute *hisi_lpddrc_pmu_cpumask_attrs[] = {
	&dev_attr_cpumask.attr,
	NULL,
};

static const struct attribute_group hisi_lpddrc_pmu_cpumask_attr_group = {
	.attrs = hisi_lpddrc_pmu_cpumask_attrs,
};

static struct device_attribute hisi_lpddrc_pmu_identifier_attr =
	__ATTR(identifier, 0444, hisi_uncore_pmu_identifier_attr_show, NULL);

static struct attribute *hisi_lpddrc_pmu_identifier_attrs[] = {
	&hisi_lpddrc_pmu_identifier_attr.attr,
	NULL
};

static struct attribute_group hisi_lpddrc_pmu_identifier_group = {
	.attrs = hisi_lpddrc_pmu_identifier_attrs,
};

static const struct attribute_group *hisi_lpddrc_pmu_v1_attr_groups[] = {
	&hisi_lpddrc_pmu_v1_format_group,
	&hisi_lpddrc_pmu_v1_events_group,
	&hisi_lpddrc_pmu_cpumask_attr_group,
	&hisi_lpddrc_pmu_identifier_group,
	NULL,
};

static const struct hisi_uncore_ops hisi_uncore_lpddrc_v1_ops = {
	.write_evtype           = hisi_lpddrc_pmu_write_evtype,
	.get_event_idx		= hisi_lpddrc_pmu_v1_get_event_idx,
	.start_counters		= hisi_lpddrc_pmu_v1_start_counters,
	.stop_counters		= hisi_lpddrc_pmu_v1_stop_counters,
	.enable_counter		= hisi_lpddrc_pmu_v1_enable_counter,
	.disable_counter	= hisi_lpddrc_pmu_v1_disable_counter,
	.enable_counter_int	= hisi_lpddrc_pmu_v1_enable_counter_int,
	.disable_counter_int	= hisi_lpddrc_pmu_v1_disable_counter_int,
	.write_counter		= hisi_lpddrc_pmu_v1_write_counter,
	.read_counter		= hisi_lpddrc_pmu_v1_read_counter,
	.get_int_status		= hisi_lpddrc_pmu_v1_get_int_status,
	.clear_int_status	= hisi_lpddrc_pmu_v1_clear_int_status,
};

static int hisi_lpddrc_pmu_dev_probe(struct platform_device *pdev,
				   struct hisi_pmu *lpddrc_pmu)
{
	int ret;

	ret = hisi_lpddrc_pmu_init_data(pdev, lpddrc_pmu);
	if (ret)
		return ret;

	ret = hisi_uncore_pmu_init_irq(lpddrc_pmu, pdev);
	if (ret)
		return ret;

	lpddrc_pmu->counter_bits = 32;
	lpddrc_pmu->check_event = LPDDRC_V1_NR_EVENTS;
	lpddrc_pmu->pmu_events.attr_groups = hisi_lpddrc_pmu_v1_attr_groups;
	lpddrc_pmu->ops = &hisi_uncore_lpddrc_v1_ops;

	lpddrc_pmu->num_counters = LPDDRC_NR_COUNTERS;
	lpddrc_pmu->dev = &pdev->dev;
	lpddrc_pmu->on_cpu = -1;

	return 0;
}

static int hisi_lpddrc_pmu_probe(struct platform_device *pdev)
{
	struct hisi_pmu *lpddrc_pmu;
	char *name;
	int ret;

	lpddrc_pmu = devm_kzalloc(&pdev->dev, sizeof(*lpddrc_pmu), GFP_KERNEL);
	if (!lpddrc_pmu)
		return -ENOMEM;

	platform_set_drvdata(pdev, lpddrc_pmu);

	ret = hisi_lpddrc_pmu_dev_probe(pdev, lpddrc_pmu);
	if (ret)
		return ret;

	name = devm_kasprintf(&pdev->dev, GFP_KERNEL,
			      "hisi_sccl%u_lpddrc%u", lpddrc_pmu->sccl_id,
			      lpddrc_pmu->index_id);

	lpddrc_pmu->pmu = (struct pmu) {
		.name		= name,
		.module		= THIS_MODULE,
		.task_ctx_nr	= perf_invalid_context,
		.event_init	= hisi_uncore_pmu_event_init,
		.pmu_enable	= hisi_uncore_pmu_enable,
		.pmu_disable	= hisi_uncore_pmu_disable,
		.add		= hisi_uncore_pmu_add,
		.del		= hisi_uncore_pmu_del,
		.start		= hisi_uncore_pmu_start,
		.stop		= hisi_uncore_pmu_stop,
		.read		= hisi_uncore_pmu_read,
		.attr_groups	= lpddrc_pmu->pmu_events.attr_groups,
		.capabilities	= PERF_PMU_CAP_NO_EXCLUDE,
	};

	/* Pick one core to use for cpumask attributes */
	cpumask_set_cpu(smp_processor_id(), &lpddrc_pmu->associated_cpus);

	lpddrc_pmu->on_cpu = cpumask_first(&lpddrc_pmu->associated_cpus);
	if (lpddrc_pmu->on_cpu >= nr_cpu_ids)
		return -EINVAL;

	ret = perf_pmu_register(&lpddrc_pmu->pmu, name, -1);

	return ret;
}

static int hisi_lpddrc_pmu_remove(struct platform_device *pdev)
{
	struct hisi_pmu *lpddrc_pmu = platform_get_drvdata(pdev);

	perf_pmu_unregister(&lpddrc_pmu->pmu);
	return 0;
}

static struct platform_driver hisi_lpddrc_pmu_driver = {
	.driver = {
		.name = "hisi_lpddrc_pmu",
		.acpi_match_table = ACPI_PTR(hisi_lpddrc_pmu_acpi_match),
		.of_match_table = lpddrc_of_match,
		.suppress_bind_attrs = true,
	},
	.probe = hisi_lpddrc_pmu_probe,
	.remove = hisi_lpddrc_pmu_remove,
};

static int __init hisi_lpddrc_pmu_module_init(void)
{
	int ret;

	ret = platform_driver_register(&hisi_lpddrc_pmu_driver);

	return ret;
}
module_init(hisi_lpddrc_pmu_module_init);

static void __exit hisi_lpddrc_pmu_module_exit(void)
{
	platform_driver_unregister(&hisi_lpddrc_pmu_driver);
}
module_exit(hisi_lpddrc_pmu_module_exit);

MODULE_DESCRIPTION("HiSilicon SoC LPDDRC uncore PMU driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Shaokun Zhang <zhangshaokun@hisilicon.com>");
MODULE_AUTHOR("Anurup M <anurup.m@huawei.com>");
