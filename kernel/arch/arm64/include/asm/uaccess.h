/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Based on arch/arm/include/asm/uaccess.h
 *
 * Copyright (C) 2012 ARM Ltd.
 */
#ifndef __ASM_UACCESS_H
#define __ASM_UACCESS_H

#include <asm/alternative.h>
#include <asm/kernel-pgtable.h>
#include <asm/sysreg.h>

/*
 * User space memory access functions
 */
#include <linux/bitops.h>
#include <linux/kasan-checks.h>
#include <linux/string.h>

#include <asm/cpufeature.h>
#include <asm/mmu.h>
#include <asm/ptrace.h>
#include <asm/memory.h>
#include <asm/extable.h>

#define HAVE_GET_KERNEL_NOFAULT

/*
 * Test whether a block of memory is a valid user space address.
 * Returns 1 if the range is valid, 0 otherwise.
 *
 * This is equivalent to the following test:
 * (u65)addr + (u65)size <= (u65)TASK_SIZE_MAX
 */
static inline unsigned long __range_ok(const void __user *addr, unsigned long size)
{
	unsigned long ret, limit = TASK_SIZE_MAX - 1;

	/*
	 * Asynchronous I/O running in a kernel thread does not have the
	 * TIF_TAGGED_ADDR flag of the process owning the mm, so always untag
	 * the user address before checking.
	 */
	if (IS_ENABLED(CONFIG_ARM64_TAGGED_ADDR_ABI) &&
	    (current->flags & PF_KTHREAD || test_thread_flag(TIF_TAGGED_ADDR)))
		addr = untagged_addr(addr);

	__chk_user_ptr(addr);
	asm volatile(
	// A + B <= C + 1 for all A,B,C, in four easy steps:
	// 1: X = A + B; X' = X % 2^64
	"	adds	%0, %3, %2\n"
	// 2: Set C = 0 if X > 2^64, to guarantee X' > C in step 4
	"	csel	%1, xzr, %1, hi\n"
	// 3: Set X' = ~0 if X >= 2^64. For X == 2^64, this decrements X'
	//    to compensate for the carry flag being set in step 4. For
	//    X > 2^64, X' merely has to remain nonzero, which it does.
	"	csinv	%0, %0, xzr, cc\n"
	// 4: For X < 2^64, this gives us X' - C - 1 <= 0, where the -1
	//    comes from the carry in being clear. Otherwise, we are
	//    testing X' - C == 0, subject to the previous adjustments.
	"	sbcs	xzr, %0, %1\n"
	"	cset	%0, ls\n"
	: "=&r" (ret), "+r" (limit) : "Ir" (size), "0" (addr) : "cc");

	return ret;
}

#define access_ok(addr, size)	__range_ok(addr, size)

#define _ASM_EXTABLE(from, to)						\
	"	.pushsection	__ex_table, \"a\"\n"			\
	"	.align		3\n"					\
	"	.long		(" #from " - .), (" #to " - .)\n"	\
	"	.popsection\n"

#ifdef CONFIG_ARCH_HAS_MC_EXTABLE
#define _ASM_MC_EXTABLE(from, to)					\
	"	.pushsection	__mc_ex_table, \"a\"\n"			\
	"	.align		3\n"					\
	"	.long		(" #from " - .), (" #to " - .)\n"	\
	"	.popsection\n"
#else
#define _ASM_MC_EXTABLE(from, to)
#endif

#define _ASM_KACCESS_EXTABLE(from, to)	_ASM_EXTABLE(from, to)
#define _ASM_UACCESS_EXTABLE(from, to)		\
		_ASM_EXTABLE(from, to)		\
		_ASM_MC_EXTABLE(from, to)

/*
 * User access enabling/disabling.
 */
#ifdef CONFIG_ARM64_SW_TTBR0_PAN
static inline void __uaccess_ttbr0_disable(void)
{
	unsigned long flags, ttbr;

	local_irq_save(flags);
	ttbr = read_sysreg(ttbr1_el1);
	ttbr &= ~TTBR_ASID_MASK;
	/* reserved_pg_dir placed before swapper_pg_dir */
	write_sysreg(ttbr - PAGE_SIZE, ttbr0_el1);
	isb();
	/* Set reserved ASID */
	write_sysreg(ttbr, ttbr1_el1);
	isb();
	local_irq_restore(flags);
}

static inline void __uaccess_ttbr0_enable(void)
{
	unsigned long flags, ttbr0, ttbr1;

	/*
	 * Disable interrupts to avoid preemption between reading the 'ttbr0'
	 * variable and the MSR. A context switch could trigger an ASID
	 * roll-over and an update of 'ttbr0'.
	 */
	local_irq_save(flags);
	ttbr0 = READ_ONCE(current_thread_info()->ttbr0);

	/* Restore active ASID */
	ttbr1 = read_sysreg(ttbr1_el1);
	ttbr1 &= ~TTBR_ASID_MASK;		/* safety measure */
	ttbr1 |= ttbr0 & TTBR_ASID_MASK;
	write_sysreg(ttbr1, ttbr1_el1);
	isb();

	/* Restore user page table */
	write_sysreg(ttbr0, ttbr0_el1);
	isb();
	local_irq_restore(flags);
}

static inline bool uaccess_ttbr0_disable(void)
{
	if (!system_uses_ttbr0_pan())
		return false;
	__uaccess_ttbr0_disable();
	return true;
}

static inline bool uaccess_ttbr0_enable(void)
{
	if (!system_uses_ttbr0_pan())
		return false;
	__uaccess_ttbr0_enable();
	return true;
}
#else
static inline bool uaccess_ttbr0_disable(void)
{
	return false;
}

static inline bool uaccess_ttbr0_enable(void)
{
	return false;
}
#endif

static inline void __uaccess_disable_hw_pan(void)
{
	asm(ALTERNATIVE("nop", SET_PSTATE_PAN(0), ARM64_HAS_PAN,
			CONFIG_ARM64_PAN));
}

static inline void __uaccess_enable_hw_pan(void)
{
	asm(ALTERNATIVE("nop", SET_PSTATE_PAN(1), ARM64_HAS_PAN,
			CONFIG_ARM64_PAN));
}

static inline void uaccess_disable_privileged(void)
{
	if (uaccess_ttbr0_disable())
		return;

	__uaccess_enable_hw_pan();
}

static inline void uaccess_enable_privileged(void)
{
	if (uaccess_ttbr0_enable())
		return;

	__uaccess_disable_hw_pan();
}

/*
 * Sanitise a uaccess pointer such that it becomes NULL if above the maximum
 * user address. In case the pointer is tagged (has the top byte set), untag
 * the pointer before checking.
 */
#define uaccess_mask_ptr(ptr) (__typeof__(ptr))__uaccess_mask_ptr(ptr)
static inline void __user *__uaccess_mask_ptr(const void __user *ptr)
{
	void __user *safe_ptr;

	asm volatile(
	"	bics	xzr, %3, %2\n"
	"	csel	%0, %1, xzr, eq\n"
	: "=&r" (safe_ptr)
	: "r" (ptr), "r" (TASK_SIZE_MAX - 1),
	  "r" (untagged_addr(ptr))
	: "cc");

	csdb();
	return safe_ptr;
}

/*
 * The "__xxx" versions of the user access functions do not verify the address
 * space - it must have been done previously with a separate "access_ok()"
 * call.
 *
 * The "__xxx_error" versions set the third argument to -EFAULT if an error
 * occurs, and leave it unchanged on success.
 */
#define __get_mem_asm(load, reg, x, addr, err, type)			\
	asm volatile(							\
	"1:	" load "	" reg "1, [%2]\n"			\
	"2:\n"								\
	"	.section .fixup, \"ax\"\n"				\
	"	.align	2\n"						\
	"3:	mov	%w0, %3\n"					\
	"	mov	%1, #0\n"					\
	"	b	2b\n"						\
	"	.previous\n"						\
	_ASM_##type##ACCESS_EXTABLE(1b, 3b)				\
	: "+r" (err), "=&r" (x)						\
	: "r" (addr), "i" (-EFAULT))

#define __raw_get_mem(ldr, x, ptr, err, type)				\
do {									\
	unsigned long __gu_val;						\
	switch (sizeof(*(ptr))) {					\
	case 1:								\
		__get_mem_asm(ldr "b", "%w", __gu_val, (ptr), (err), type);	\
		break;							\
	case 2:								\
		__get_mem_asm(ldr "h", "%w", __gu_val, (ptr), (err), type);	\
		break;							\
	case 4:								\
		__get_mem_asm(ldr, "%w", __gu_val, (ptr), (err), type);	\
		break;							\
	case 8:								\
		__get_mem_asm(ldr, "%x",  __gu_val, (ptr), (err), type);	\
		break;							\
	default:							\
		BUILD_BUG();						\
	}								\
	(x) = (__force __typeof__(*(ptr)))__gu_val;			\
} while (0)

#define __raw_get_user(x, ptr, err)					\
do {									\
	__chk_user_ptr(ptr);						\
	uaccess_ttbr0_enable();						\
	__raw_get_mem("ldtr", x, ptr, err, U);				\
	uaccess_ttbr0_disable();					\
} while (0)

#define __get_user_error(x, ptr, err)					\
do {									\
	__typeof__(*(ptr)) __user *__p = (ptr);				\
	might_fault();							\
	if (access_ok(__p, sizeof(*__p))) {				\
		__p = uaccess_mask_ptr(__p);				\
		__raw_get_user((x), __p, (err));			\
	} else {							\
		(x) = (__force __typeof__(x))0; (err) = -EFAULT;	\
	}								\
} while (0)

#define __get_user(x, ptr)						\
({									\
	int __gu_err = 0;						\
	__get_user_error((x), (ptr), __gu_err);				\
	__gu_err;							\
})

#define get_user	__get_user

#define __get_kernel_nofault(dst, src, type, err_label)			\
do {									\
	int __gkn_err = 0;						\
									\
	__raw_get_mem("ldr", *((type *)(dst)),				\
		      (__force type *)(src), __gkn_err, K);		\
	if (unlikely(__gkn_err))					\
		goto err_label;						\
} while (0)

#define __put_mem_asm(store, reg, x, addr, err, type)			\
	asm volatile(							\
	"1:	" store "	" reg "1, [%2]\n"			\
	"2:\n"								\
	"	.section .fixup,\"ax\"\n"				\
	"	.align	2\n"						\
	"3:	mov	%w0, %3\n"					\
	"	b	2b\n"						\
	"	.previous\n"						\
	_ASM_##type##ACCESS_EXTABLE(1b, 3b)				\
	: "+r" (err)							\
	: "r" (x), "r" (addr), "i" (-EFAULT))

#define __raw_put_mem(str, x, ptr, err, type)				\
do {									\
	__typeof__(*(ptr)) __pu_val = (x);				\
	switch (sizeof(*(ptr))) {					\
	case 1:								\
		__put_mem_asm(str "b", "%w", __pu_val, (ptr), (err), type);	\
		break;							\
	case 2:								\
		__put_mem_asm(str "h", "%w", __pu_val, (ptr), (err), type);	\
		break;							\
	case 4:								\
		__put_mem_asm(str, "%w", __pu_val, (ptr), (err), type);	\
		break;							\
	case 8:								\
		__put_mem_asm(str, "%x", __pu_val, (ptr), (err), type);	\
		break;							\
	default:							\
		BUILD_BUG();						\
	}								\
} while (0)

#define __raw_put_user(x, ptr, err)					\
do {									\
	__chk_user_ptr(ptr);						\
	uaccess_ttbr0_enable();						\
	__raw_put_mem("sttr", x, ptr, err, U);				\
	uaccess_ttbr0_disable();					\
} while (0)

#define __put_user_error(x, ptr, err)					\
do {									\
	__typeof__(*(ptr)) __user *__p = (ptr);				\
	might_fault();							\
	if (access_ok(__p, sizeof(*__p))) {				\
		__p = uaccess_mask_ptr(__p);				\
		__raw_put_user((x), __p, (err));			\
	} else	{							\
		(err) = -EFAULT;					\
	}								\
} while (0)

#define __put_user(x, ptr)						\
({									\
	int __pu_err = 0;						\
	__put_user_error((x), (ptr), __pu_err);				\
	__pu_err;							\
})

#define put_user	__put_user

#define __put_kernel_nofault(dst, src, type, err_label)			\
do {									\
	int __pkn_err = 0;						\
									\
	__raw_put_mem("str", *((type *)(src)),				\
		      (__force type *)(dst), __pkn_err, K);		\
	if (unlikely(__pkn_err))					\
		goto err_label;						\
} while(0)

extern unsigned long __must_check __arch_copy_from_user(void *to, const void __user *from, unsigned long n);
#define raw_copy_from_user(to, from, n)					\
({									\
	unsigned long __acfu_ret;					\
	uaccess_ttbr0_enable();						\
	__acfu_ret = __arch_copy_from_user((to),			\
				      __uaccess_mask_ptr(from), (n));	\
	uaccess_ttbr0_disable();					\
	__acfu_ret;							\
})

extern unsigned long __must_check __arch_copy_to_user(void __user *to, const void *from, unsigned long n);
#define raw_copy_to_user(to, from, n)					\
({									\
	unsigned long __actu_ret;					\
	uaccess_ttbr0_enable();						\
	__actu_ret = __arch_copy_to_user(__uaccess_mask_ptr(to),	\
				    (from), (n));			\
	uaccess_ttbr0_disable();					\
	__actu_ret;							\
})

extern unsigned long __must_check __arch_copy_in_user(void __user *to, const void __user *from, unsigned long n);
#define raw_copy_in_user(to, from, n)					\
({									\
	unsigned long __aciu_ret;					\
	uaccess_ttbr0_enable();						\
	__aciu_ret = __arch_copy_in_user(__uaccess_mask_ptr(to),	\
				    __uaccess_mask_ptr(from), (n));	\
	uaccess_ttbr0_disable();					\
	__aciu_ret;							\
})

#define INLINE_COPY_TO_USER
#define INLINE_COPY_FROM_USER

extern unsigned long __must_check __arch_clear_user(void __user *to, unsigned long n);
static inline unsigned long __must_check __clear_user(void __user *to, unsigned long n)
{
	if (access_ok(to, n)) {
		uaccess_ttbr0_enable();
		n = __arch_clear_user(__uaccess_mask_ptr(to), n);
		uaccess_ttbr0_disable();
	}
	return n;
}
#define clear_user	__clear_user

extern long strncpy_from_user(char *dest, const char __user *src, long count);

extern __must_check long strnlen_user(const char __user *str, long n);

#ifdef CONFIG_ARCH_HAS_UACCESS_FLUSHCACHE
struct page;
void memcpy_page_flushcache(char *to, struct page *page, size_t offset, size_t len);
extern unsigned long __must_check __copy_user_flushcache(void *to, const void __user *from, unsigned long n);

static inline int __copy_from_user_flushcache(void *dst, const void __user *src, unsigned size)
{
	kasan_check_write(dst, size);
	return __copy_user_flushcache(dst, __uaccess_mask_ptr(src), size);
}
#endif

#ifdef CONFIG_ARCH_HAS_COPY_MC
/**
 * copy_mc_to_kernel - memory copy that handles source exceptions
 *
 * @dst:	destination address
 * @src:	source address
 * @len:	number of bytes to copy
 *
 * Return 0 for success, or number of bytes not copied if there was an
 * exception.
 */
static inline unsigned long __must_check
copy_mc_to_kernel(void *to, const void *from, unsigned long size)
{
	return (unsigned long)memcpy_mcs(to, from, size);
}
#define copy_mc_to_kernel copy_mc_to_kernel
#endif

#endif /* __ASM_UACCESS_H */
