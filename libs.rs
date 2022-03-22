
use std::ffi::{CStr,CString};
use std::os::raw::{c_uchar,c_int,c_char};

#[derive(Debug,Default)]
struct FrameData{
    data:c_uchar,
    width:c_int,
    height:c_int,
}

// gcc -fPIC -shared -lavformat -lavcodec -o format.so format.c 

#[link(name="format")]
extern "C"{
    fn extract(p:*const c_char,f:*mut FrameData)->c_int;
}

fn ex(){
}

#[test]
fn ff(){
    // ex()
    let src = CString::new("./formatVideo/2.mp4").unwrap();
    let f = FrameData::default();
    let box_f = Box::new(f);
    let p_f = Box::into_raw(box_f);
    unsafe {
        let re = extract((&src).as_ptr(), p_f);
        println!("{}", re);
    }
}



#[test]
fn de(){
    #[derive(Debug)]
    struct Tests{
        enable:bool,
        type_id:i32,
    }
    impl Default for Tests {
        fn default()-> Self{
            Tests{
                enable:true,
                type_id:1,
            }
        }
        
    }
    let t = Tests::default();
    println!("{:?}",t);

    let a = Box::new(10);
    // 我们需要先解引用a，再隐式把 & 转换成 *
    // let b: *const i32 = *a;
    // 使用 into_raw 方法
    let c: *const i32 = Box::into_raw(a);

}


