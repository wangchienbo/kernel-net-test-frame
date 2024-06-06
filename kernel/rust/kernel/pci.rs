// SPDX-License-Identifier: GPL-2.0

//! PCI devices and drivers.
//!
//! C header: [`include/linux/pci.h`](../../../../include/linux/pci.h)

#![allow(dead_code)]

use crate::{
    bindings, c_types, device, driver,
    error::{from_kernel_result, Result},
    io_mem::Resource,
    str::CStr,
    to_result,
    types::PointerWrapper,
    ThisModule,
};

/// An adapter for the registration of PCI drivers.
pub struct Adapter<T: Driver>(T);

impl<T: Driver> driver::DriverOps for Adapter<T> {
    type RegType = bindings::pci_driver;

    unsafe fn register(
        reg: *mut bindings::pci_driver,
        name: &'static CStr,
        module: &'static ThisModule,
    ) -> Result {
        let pdrv: &mut bindings::pci_driver = unsafe { &mut *reg };

        pdrv.name = name.as_char_ptr();
        pdrv.probe = Some(Self::probe_callback);
        pdrv.remove = Some(Self::remove_callback);
        pdrv.id_table = T::PCI_ID_TABLE.as_ref();
        to_result(|| unsafe { bindings::__pci_register_driver(reg, module.0, name.as_char_ptr()) })
    }

    unsafe fn unregister(reg: *mut bindings::pci_driver) {
        unsafe { bindings::pci_unregister_driver(reg) }
    }
}

impl<T: Driver> Adapter<T> {
    extern "C" fn probe_callback(
        pdev: *mut bindings::pci_dev,
        id: *const bindings::pci_device_id,
    ) -> c_types::c_int {
        from_kernel_result! {
            let mut dev = unsafe { Device::from_ptr(pdev) };

            // SAFETY: `id` is a pointer within the static table, so it's always valid.
            let offset = unsafe {(*id).driver_data};
            // SAFETY: The offset comes from a previous call to `offset_from` in `IdArray::new`, which
            // guarantees that the resulting pointer is within the table.
            let info = {
                let ptr = unsafe {id.cast::<u8>().offset(offset as _).cast::<Option<T::IdInfo>>()};
                unsafe {(&*ptr).as_ref()}
            };
            let data = T::probe(&mut dev, info)?;
            unsafe { bindings::pci_set_drvdata(pdev, data.into_pointer() as _) };
            Ok(0)
        }
    }

    extern "C" fn remove_callback(pdev: *mut bindings::pci_dev) {
        let ptr = unsafe { bindings::pci_get_drvdata(pdev) };
        let data = unsafe { T::Data::from_pointer(ptr) };
        T::remove(&data);
        <T::Data as driver::DeviceRemoval>::device_remove(&data);
    }
}

/// Abstraction for bindings::pci_device_id.
#[derive(Clone, Copy)]
pub struct DeviceId {
    /// Vendor ID
    pub vendor: u32,
    /// Device ID
    pub device: u32,
    /// Subsystem vendor ID
    pub subvendor: u32,
    /// Subsystem device ID
    pub subdevice: u32,
    /// Device class and subclass
    pub class: u32,
    /// Limit which sub-fields of the class
    pub class_mask: u32,
}

impl DeviceId {
    const PCI_ANY_ID: u32 = !0;

    /// PCI_DEVICE macro.
    pub const fn new(vendor: u32, device: u32) -> Self {
        Self {
            vendor,
            device,
            subvendor: DeviceId::PCI_ANY_ID,
            subdevice: DeviceId::PCI_ANY_ID,
            class: 0,
            class_mask: 0,
        }
    }

    /// PCI_DEVICE_CLASS macro.
    pub const fn with_class(class: u32, class_mask: u32) -> Self {
        Self {
            vendor: DeviceId::PCI_ANY_ID,
            device: DeviceId::PCI_ANY_ID,
            subvendor: DeviceId::PCI_ANY_ID,
            subdevice: DeviceId::PCI_ANY_ID,
            class,
            class_mask,
        }
    }
}

// SAFETY: `ZERO` is all zeroed-out and `to_rawid` stores `offset` in `pci_device_id::driver_data`.
unsafe impl const driver::RawDeviceId for DeviceId {
    type RawType = bindings::pci_device_id;

    const ZERO: Self::RawType = bindings::pci_device_id {
        vendor: 0,
        device: 0,
        subvendor: 0,
        subdevice: 0,
        class: 0,
        class_mask: 0,
        driver_data: 0,
    };

    fn to_rawid(&self, offset: isize) -> Self::RawType {
        bindings::pci_device_id {
            vendor: self.vendor,
            device: self.device,
            subvendor: self.subvendor,
            subdevice: self.subdevice,
            class: self.class,
            class_mask: self.class_mask,
            driver_data: offset as _,
        }
    }
}

/// Define a const pci device id table
///
/// # Examples
///
/// ```ignore
/// # use kernel::{pci, define_pci_id_table};
/// #
/// struct MyDriver;
/// impl pci::Driver for MyDriver {
///     // [...]
/// #   fn probe(_dev: &mut pci::Device, _id_info: Option<&Self::IdInfo>) -> Result {
/// #       Ok(())
/// #   }
/// #   define_pci_id_table! {u32, [
/// #       (pci::DeviceId::new(0x010800, 0xffffff), None),
/// #       (pci::DeviceId::with_class(0x010802, 0xfffff), Some(0x10)),
/// #   ]}
/// }
/// ```
#[macro_export]
macro_rules! define_pci_id_table {
    ($data_type:ty, $($t:tt)*) => {
        type IdInfo = $data_type;
        const PCI_ID_TABLE: $crate::driver::IdTable<'static, $crate::pci::DeviceId, $data_type> = {
            $crate::define_id_array!(ARRAY, $crate::pci::DeviceId, $data_type, $($t)* );
            ARRAY.as_table()
        };
    };
}

/// A PCI driver
pub trait Driver {
    /// Data stored on device by driver.
    ///
    /// Corresponds to the data set or retrieved via the kernel's
    /// `pci_{set,get}_drvdata()` functions.
    ///
    /// Require that `Data` implements `PointerWrapper`. We guarantee to
    /// never move the underlying wrapped data structure. This allows
    type Data: PointerWrapper + Send + Sync + driver::DeviceRemoval = ();

    /// The type holding information about each device id supported by the driver.
    type IdInfo: 'static = ();

    /// The table of device ids supported by the driver.
    const PCI_ID_TABLE: driver::IdTable<'static, DeviceId, Self::IdInfo>;

    /// PCI driver probe.
    ///
    /// Called when a new platform device is added or discovered.
    /// Implementers should attempt to initialize the device here.
    fn probe(dev: &mut Device, id: Option<&Self::IdInfo>) -> Result<Self::Data>;

    /// PCI driver remove.
    ///
    /// Called when a platform device is removed.
    /// Implementers should prepare the device for complete removal here.
    fn remove(_data: &Self::Data) {}
}

/// A PCI device.
///
/// # Invariants
///
/// The field `ptr` is non-null and valid for the lifetime of the object.
pub struct Device {
    ptr: *mut bindings::pci_dev,
    res: Option<Resource>,
}

impl Device {
    /// Creates a new device from the given pointer.
    ///
    /// # Safety
    ///
    /// `ptr` must be non-null and valid. It must remain valid for the lifetime of the returned
    /// instance.
    unsafe fn from_ptr(ptr: *mut bindings::pci_dev) -> Self {
        // SAFETY: The safety requirements of the function ensure that `ptr` is valid.
        let dev = unsafe { &mut *ptr };
        // INVARIANT: The safety requirements of the function ensure the lifetime invariant.
        Self {
            ptr,
            res: Resource::new(dev.resource[0].start, dev.resource[0].end),
        }
    }

    /// Returns the io mem resource associated with the device, if there is one.
    ///
    /// Ownership of the resource is transferred to the caller, so subsequent calls to this
    /// function will return [`None`].
    pub fn take_resource(&mut self) -> Option<Resource> {
        self.res.take()
    }

    pub fn pci_enable_sriov(&self, nr_virtfn: i32) -> Result {
        to_result(||
            // SAFETY: The existence of the shared references mean `self.0`is valid.
            unsafe{bindings::pci_enable_sriov(self.ptr, nr_virtfn)})
    }

    fn pci_disable_sriov(&self) {
        // SAFETY: The existence of the shared references mean `self.0`is valid.
        unsafe { bindings::pci_disable_sriov(self.ptr) }
    }
}

unsafe impl device::RawDevice for Device {
    fn raw_device(&self) -> *mut bindings::device {
        // SAFETY: By the type invariants, we know that `self.ptr` is non-null and valid.
        unsafe { &mut (*self.ptr).dev }
    }
}

/// Declares a kernel module that exposes a single pci driver.
///
/// # Examples
///
/// ```ignore
/// # use kernel::{pci, define_pci_id_table, module_pci_driver};
/// #
/// struct MyDriver;
/// impl pci::Driver for MyDriver {
///     // [...]
/// #   fn probe(_dev: &mut pci::Device, _id_info: Option<&Self::IdInfo>) -> Result {
/// #       Ok(())
/// #   }
/// #   define_pci_id_table! {u32, [
/// #       (pci::DeviceId::new(0x177d, 0xa018), None),
/// #   ]}
/// }
///
/// module_pci_driver! {
///     type: MyDriver,
///     name: b"module_name",
///     author: b"Author name",
///     license: b"GPL",
/// }
/// ```
#[macro_export]
macro_rules! module_pci_driver {
    ($($f:tt)*) => {
        $crate::module_driver!(<T>, $crate::pci::Adapter<T>, { $($f)* });
    };
}
