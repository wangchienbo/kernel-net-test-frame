/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_SW64_KVM_ASM_H
#define _ASM_SW64_KVM_ASM_H

#define SW64_KVM_EXIT_HOST_INTR		0
#define SW64_KVM_EXIT_IO		1
#define SW64_KVM_MIGRATION_SET_DIRTY    2
#define SW64_KVM_MIGRATION_SET_DIRTY_HM 3
#define SW64_KVM_EXIT_HALT		10
#define SW64_KVM_EXIT_SHUTDOWN		12
#define SW64_KVM_EXIT_TIMER		13
#define SW64_KVM_EXIT_IPI		14
#define SW64_KVM_EXIT_RESTART		17
#define SW64_KVM_EXIT_FATAL_ERROR	22

#ifdef CONFIG_KVM_MEMHOTPLUG
#define SW64_KVM_EXIT_MEMHOTPLUG	23
#endif

#define kvm_sw64_exception_type	\
	{0, "HOST_INTR" },	\
	{1, "IO" },		\
	{10, "HALT" },		\
	{12, "SHUTDOWN" },	\
	{13, "TIMER" },		\
	{14, "IPI" },		\
	{17, "RESTART" },	\
	{22, "FATAL_ERROR" },	\
	{23, "MEMHOTPLUG" }

#endif /* _ASM_SW64_KVM_ASM_H */
