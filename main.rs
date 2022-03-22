
use std::ffi::{
    // CStr,
    CString
};
use std::os::raw::{c_uchar,c_int,c_char};

#[repr(C)]
#[derive(Debug,Default)]
struct FrameData{
    data:[c_uchar;3],
    width:c_int,
    height:c_int,
}

// gcc -fPIC -shared -lavformat -lavcodec -o format.so format.c 

// #[link(name="libformat")]
#[link(name="format")]
extern "C"{
    fn extract(p:*const c_char,f:*mut FrameData)->c_int;
}

fn main(){
    let src = CString::new("../formatVideo/2.mp4").unwrap();
    let f = FrameData::default();
    let box_f = Box::new(f);
    let p_f = Box::into_raw(box_f);
    unsafe {
        let re = extract((&src).as_ptr(), p_f);
        println!("返回{}", re);
        println!("rust height: {}",(*p_f).height);
        println!("rust width: {}",(*p_f).width);
        println!("rust data1: {}",(*p_f).data[0]);
    };
}