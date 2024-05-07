use ark_bn254::Fr;
use p3_goldilocks::{DiffusionMatrixGoldilocks, Goldilocks};
use p3_poseidon2::{
    DiffusionPermutation, MdsLightPermutation, Poseidon2, Poseidon2ExternalMatrixGeneral,
};
use std::{mem, slice, time::Instant};
use tachyon_rs::math::elliptic_curves::bn::bn254::Fr as CppFr;

#[no_mangle]
pub extern "C" fn run_poseidon_plonky3(
    pre_images: *const CppFr,
    absorbing_num: usize,
    squeezing_num: usize,
    duration: *mut u64,
) -> *mut CppFr {
    let external_constants = vec![[0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7]];
    let internal_constants = vec![[0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7]];

    let poseidon = Poseidon2::<
        Goldilocks,
        Poseidon2ExternalMatrixGeneral,
        DiffusionMatrixGoldilocks,
        8,
        7,
    >::new(
        8,
        external_constants,
        Poseidon2ExternalMatrixGeneral::default(),
        22,
        internal_constants,
        DiffusionMatrixGoldilocks::default(),
    );

    let pre_images = unsafe {
        let pre_images: &[CppFr] = slice::from_raw_parts(pre_images, absorbing_num);
        let pre_images: &[Fr] = mem::transmute(pre_images);

        pre_images
    };

    let start = Instant::now();
    for pre_image in pre_images {
        sponge.absorb(pre_image);
    }
    let squeezed_elements = sponge.squeeze_native_field_elements(squeezing_num);
    unsafe {
        duration.write(start.elapsed().as_micros() as u64);
    }

    Box::into_raw(Box::new(squeezed_elements[0])) as *mut CppFr
}
